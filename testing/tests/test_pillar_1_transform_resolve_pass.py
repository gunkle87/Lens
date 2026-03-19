import ctypes
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE_PATH = ROOT / "src" / "render_transform_resolver.c"
INCLUDE_PATH = ROOT / "include"
RENDER_INVALID_HANDLE = 0xFFFFFFFF
RENDER_INVALID_INDEX = 0xFFFFFFFF
RENDER_MATRIX_4X4_ELEMENT_COUNT = 16

RENDER_TRANSFORM_RESOLVE_RESULT_OK = 0
RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT = 1
RENDER_TRANSFORM_RESOLVE_RESULT_OUT_OF_RANGE_PARENT = 2
RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_PARENT_LINK = 3
RENDER_TRANSFORM_RESOLVE_RESULT_CYCLE_DETECTED = 4


class RenderRecordId(ctypes.Structure):
    _fields_ = [
        ("handle", ctypes.c_uint32),
        ("index", ctypes.c_uint32),
    ]


class RenderMatrix4x4(ctypes.Structure):
    _fields_ = [
        ("elements", ctypes.c_float * RENDER_MATRIX_4X4_ELEMENT_COUNT),
    ]


class RenderTransformRecord(ctypes.Structure):
    _fields_ = [
        ("transform_id", RenderRecordId),
        ("parent_transform_id", RenderRecordId),
        ("local_matrix", RenderMatrix4x4),
        ("transform_flags", ctypes.c_uint32),
    ]


class RenderResolvedTransformRecord(ctypes.Structure):
    _fields_ = [
        ("transform_id", RenderRecordId),
        ("resolved_matrix", RenderMatrix4x4),
        ("resolved_depth", ctypes.c_uint32),
        ("source_transform_flags", ctypes.c_uint32),
    ]


class RenderTransformResolver(ctypes.Structure):
    _fields_ = [
        ("input_records", ctypes.POINTER(RenderTransformRecord)),
        ("resolved_records", ctypes.POINTER(RenderResolvedTransformRecord)),
        ("resolve_states", ctypes.POINTER(ctypes.c_int)),
        ("count", ctypes.c_uint32),
    ]


def find_compiler() -> str:
    for compiler_name in ("gcc", "clang", "cc"):
        compiler_path = shutil.which(compiler_name)
        if compiler_path:
            return compiler_path
    raise RuntimeError("No supported C compiler found for transform resolver tests.")


def make_matrix(elements: list[float]) -> RenderMatrix4x4:
    return RenderMatrix4x4((ctypes.c_float * RENDER_MATRIX_4X4_ELEMENT_COUNT)(*elements))


def make_identity_matrix() -> RenderMatrix4x4:
    return make_matrix(
        [
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0,
        ]
    )


def make_translation_matrix(x: float, y: float, z: float) -> RenderMatrix4x4:
    return make_matrix(
        [
            1.0, 0.0, 0.0, x,
            0.0, 1.0, 0.0, y,
            0.0, 0.0, 1.0, z,
            0.0, 0.0, 0.0, 1.0,
        ]
    )


def make_scale_matrix(x: float, y: float, z: float) -> RenderMatrix4x4:
    return make_matrix(
        [
            x, 0.0, 0.0, 0.0,
            0.0, y, 0.0, 0.0,
            0.0, 0.0, z, 0.0,
            0.0, 0.0, 0.0, 1.0,
        ]
    )


def make_record_id(handle: int, index: int) -> RenderRecordId:
    return RenderRecordId(handle=handle, index=index)


def make_transform_record(
    handle: int,
    index: int,
    parent_handle: int,
    parent_index: int,
    local_matrix: RenderMatrix4x4,
    flags: int,
) -> RenderTransformRecord:
    return RenderTransformRecord(
        transform_id=make_record_id(handle, index),
        parent_transform_id=make_record_id(parent_handle, parent_index),
        local_matrix=local_matrix,
        transform_flags=flags,
    )


def matrix_to_list(matrix: RenderMatrix4x4) -> list[float]:
    return [float(element) for element in matrix.elements]


def multiply_matrices(left: RenderMatrix4x4, right: RenderMatrix4x4) -> list[float]:
    output_elements = [0.0] * RENDER_MATRIX_4X4_ELEMENT_COUNT

    for row_index in range(4):
        for column_index in range(4):
            element_value = 0.0
            for element_index in range(4):
                element_value += (
                    left.elements[(row_index * 4) + element_index]
                    * right.elements[(element_index * 4) + column_index]
                )
            output_elements[(row_index * 4) + column_index] = element_value

    return output_elements


class TransformResolvePassTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.temp_dir = tempfile.TemporaryDirectory(prefix="lens_transform_resolver_")
        cls.library_path = Path(cls.temp_dir.name) / "render_transform_resolver_test.dll"
        compiler_path = find_compiler()
        # TEST-CORE-ATTACHMENT-EXCEPTION: EXC-20260318-p1-runtime-core-ctypes
        # Temporary direct core attachment for Pillar 1 runtime validation.
        # Removal steps:
        # 1) Replace this direct gcc/ctypes runtime harness with the approved
        #    non-attached native test adapter when available.
        # 2) Delete this subprocess gcc invocation and ctypes.CDLL load path.
        # 3) Remove EXC-20260318-p1-runtime-core-ctypes from
        #    testing/TEST_CORE_ATTACHMENT_EXCEPTIONS.md after revalidation.
        compile_command = [
            compiler_path,
            "-shared",
            "-std=c11",
            "-O0",
            "-static-libgcc",
            "-I",
            str(INCLUDE_PATH),
            str(SOURCE_PATH),
            "-o",
            str(cls.library_path),
        ]
        subprocess.run(
            compile_command,
            check=True,
            capture_output=True,
            text=True,
        )
        # TEST-CORE-ATTACHMENT-EXCEPTION: EXC-20260318-p1-runtime-core-ctypes
        cls.library = ctypes.CDLL(str(cls.library_path))
        cls.configure_prototypes()

    @classmethod
    def tearDownClass(cls) -> None:
        if hasattr(cls, "library"):
            kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
            kernel32.FreeLibrary.argtypes = [ctypes.c_void_p]
            kernel32.FreeLibrary.restype = ctypes.c_int
            kernel32.FreeLibrary(ctypes.c_void_p(cls.library._handle))
        cls.temp_dir.cleanup()

    @classmethod
    def configure_prototypes(cls) -> None:
        cls.library.render_transform_resolver_initialize.argtypes = [
            ctypes.POINTER(RenderTransformResolver),
            ctypes.POINTER(RenderTransformRecord),
            ctypes.POINTER(RenderResolvedTransformRecord),
            ctypes.POINTER(ctypes.c_int),
            ctypes.c_uint32,
        ]
        cls.library.render_transform_resolver_initialize.restype = ctypes.c_int

        cls.library.render_transform_resolver_reset.argtypes = [
            ctypes.POINTER(RenderTransformResolver),
        ]
        cls.library.render_transform_resolver_reset.restype = None

        cls.library.render_transform_resolver_resolve_all.argtypes = [
            ctypes.POINTER(RenderTransformResolver),
        ]
        cls.library.render_transform_resolver_resolve_all.restype = ctypes.c_int

        cls.library.render_transform_resolver_get_resolved_record.argtypes = [
            ctypes.POINTER(RenderTransformResolver),
            ctypes.c_uint32,
        ]
        cls.library.render_transform_resolver_get_resolved_record.restype = (
            ctypes.POINTER(RenderResolvedTransformRecord)
        )

    def initialize_resolver(self, input_records: list[RenderTransformRecord]):
        input_array = (RenderTransformRecord * len(input_records))(*input_records)
        resolved_array = (RenderResolvedTransformRecord * len(input_records))()
        resolve_states = (ctypes.c_int * len(input_records))()
        resolver = RenderTransformResolver()
        result = self.library.render_transform_resolver_initialize(
            ctypes.byref(resolver),
            input_array,
            resolved_array,
            resolve_states,
            len(input_records),
        )
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_OK, result)
        return resolver, input_array, resolved_array, resolve_states

    def assert_matrix_equal(
        self,
        expected_elements: list[float],
        actual_matrix: RenderMatrix4x4,
    ) -> None:
        actual_elements = matrix_to_list(actual_matrix)
        self.assertEqual(len(expected_elements), len(actual_elements))
        for expected_element, actual_element in zip(expected_elements, actual_elements):
            self.assertAlmostEqual(expected_element, actual_element, places=5)

    def test_resolves_output_in_stable_input_index_order(self) -> None:
        records = [
            make_transform_record(
                handle=21,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                local_matrix=make_identity_matrix(),
                flags=100,
            ),
            make_transform_record(
                handle=9,
                index=1,
                parent_handle=21,
                parent_index=0,
                local_matrix=make_translation_matrix(5.0, 0.0, 0.0),
                flags=200,
            ),
            make_transform_record(
                handle=77,
                index=2,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                local_matrix=make_scale_matrix(3.0, 3.0, 1.0),
                flags=300,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)
        self.assertFalse(
            self.library.render_transform_resolver_get_resolved_record(
                ctypes.byref(resolver),
                0,
            )
        )

        result = self.library.render_transform_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_OK, result)

        first_record = self.library.render_transform_resolver_get_resolved_record(
            ctypes.byref(resolver),
            0,
        )
        second_record = self.library.render_transform_resolver_get_resolved_record(
            ctypes.byref(resolver),
            1,
        )
        third_record = self.library.render_transform_resolver_get_resolved_record(
            ctypes.byref(resolver),
            2,
        )

        self.assertTrue(first_record)
        self.assertTrue(second_record)
        self.assertTrue(third_record)
        self.assertEqual(21, first_record.contents.transform_id.handle)
        self.assertEqual(0, first_record.contents.transform_id.index)
        self.assertEqual(9, second_record.contents.transform_id.handle)
        self.assertEqual(1, second_record.contents.transform_id.index)
        self.assertEqual(77, third_record.contents.transform_id.handle)
        self.assertEqual(2, third_record.contents.transform_id.index)
        self.assertEqual(100, first_record.contents.source_transform_flags)
        self.assertEqual(200, second_record.contents.source_transform_flags)
        self.assertEqual(300, third_record.contents.source_transform_flags)

    def test_resolves_parent_chain_with_parent_times_local_matrix_composition(self) -> None:
        parent_matrix = make_translation_matrix(3.0, 0.0, 0.0)
        child_matrix = make_scale_matrix(2.0, 2.0, 1.0)
        grandchild_matrix = make_translation_matrix(0.0, 4.0, 0.0)
        records = [
            make_transform_record(
                handle=11,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                local_matrix=parent_matrix,
                flags=10,
            ),
            make_transform_record(
                handle=12,
                index=1,
                parent_handle=11,
                parent_index=0,
                local_matrix=child_matrix,
                flags=20,
            ),
            make_transform_record(
                handle=13,
                index=2,
                parent_handle=12,
                parent_index=1,
                local_matrix=grandchild_matrix,
                flags=30,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_transform_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_OK, result)

        parent_record = self.library.render_transform_resolver_get_resolved_record(
            ctypes.byref(resolver),
            0,
        )
        child_record = self.library.render_transform_resolver_get_resolved_record(
            ctypes.byref(resolver),
            1,
        )
        grandchild_record = self.library.render_transform_resolver_get_resolved_record(
            ctypes.byref(resolver),
            2,
        )

        expected_child_matrix = multiply_matrices(parent_matrix, child_matrix)
        expected_grandchild_matrix = multiply_matrices(
            make_matrix(expected_child_matrix),
            grandchild_matrix,
        )

        self.assert_matrix_equal(matrix_to_list(parent_matrix), parent_record.contents.resolved_matrix)
        self.assert_matrix_equal(expected_child_matrix, child_record.contents.resolved_matrix)
        self.assert_matrix_equal(
            expected_grandchild_matrix,
            grandchild_record.contents.resolved_matrix,
        )
        self.assertEqual(0, parent_record.contents.resolved_depth)
        self.assertEqual(1, child_record.contents.resolved_depth)
        self.assertEqual(2, grandchild_record.contents.resolved_depth)

    def test_rejects_invalid_arguments(self) -> None:
        resolver = RenderTransformResolver()
        result = self.library.render_transform_resolver_initialize(
            None,
            None,
            None,
            None,
            1,
        )
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT, result)

        result = self.library.render_transform_resolver_initialize(
            ctypes.byref(resolver),
            None,
            None,
            None,
            1,
        )
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT, result)

        result = self.library.render_transform_resolver_resolve_all(None)
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT, result)
        self.assertFalse(self.library.render_transform_resolver_get_resolved_record(None, 0))

    def test_detects_invalid_parent_links(self) -> None:
        records = [
            make_transform_record(
                handle=44,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                local_matrix=make_identity_matrix(),
                flags=1,
            ),
            make_transform_record(
                handle=45,
                index=1,
                parent_handle=999,
                parent_index=0,
                local_matrix=make_identity_matrix(),
                flags=2,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_transform_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_PARENT_LINK, result)
        self.assertFalse(
            self.library.render_transform_resolver_get_resolved_record(
                ctypes.byref(resolver),
                1,
            )
        )

    def test_detects_out_of_range_parent_references(self) -> None:
        records = [
            make_transform_record(
                handle=55,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                local_matrix=make_identity_matrix(),
                flags=1,
            ),
            make_transform_record(
                handle=56,
                index=1,
                parent_handle=55,
                parent_index=8,
                local_matrix=make_identity_matrix(),
                flags=2,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_transform_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_OUT_OF_RANGE_PARENT, result)
        self.assertFalse(
            self.library.render_transform_resolver_get_resolved_record(
                ctypes.byref(resolver),
                1,
            )
        )

    def test_detects_cycles(self) -> None:
        records = [
            make_transform_record(
                handle=70,
                index=0,
                parent_handle=71,
                parent_index=1,
                local_matrix=make_identity_matrix(),
                flags=1,
            ),
            make_transform_record(
                handle=71,
                index=1,
                parent_handle=70,
                parent_index=0,
                local_matrix=make_identity_matrix(),
                flags=2,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_transform_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_TRANSFORM_RESOLVE_RESULT_CYCLE_DETECTED, result)
        self.assertFalse(
            self.library.render_transform_resolver_get_resolved_record(
                ctypes.byref(resolver),
                0,
            )
        )
        self.assertFalse(
            self.library.render_transform_resolver_get_resolved_record(
                ctypes.byref(resolver),
                1,
            )
        )


if __name__ == "__main__":
    unittest.main()
