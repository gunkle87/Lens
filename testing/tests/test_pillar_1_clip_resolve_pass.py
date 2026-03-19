import ctypes
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE_PATH = ROOT / "src" / "render_clip_resolver.c"
INCLUDE_PATH = ROOT / "include"
RENDER_INVALID_HANDLE = 0xFFFFFFFF
RENDER_INVALID_INDEX = 0xFFFFFFFF

RENDER_CLIP_RESOLVE_RESULT_OK = 0
RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT = 1
RENDER_CLIP_RESOLVE_RESULT_OUT_OF_RANGE_PARENT = 2
RENDER_CLIP_RESOLVE_RESULT_INVALID_PARENT_LINK = 3
RENDER_CLIP_RESOLVE_RESULT_CYCLE_DETECTED = 4

DEFAULT_MIN = ctypes.c_float(-3.402823466e38).value
DEFAULT_MAX = ctypes.c_float(3.402823466e38).value


class RenderRecordId(ctypes.Structure):
    _fields_ = [
        ("handle", ctypes.c_uint32),
        ("index", ctypes.c_uint32),
    ]


class RenderClipRect(ctypes.Structure):
    _fields_ = [
        ("min_x", ctypes.c_float),
        ("min_y", ctypes.c_float),
        ("max_x", ctypes.c_float),
        ("max_y", ctypes.c_float),
    ]


class RenderClipRecord(ctypes.Structure):
    _fields_ = [
        ("clip_id", RenderRecordId),
        ("parent_clip_id", RenderRecordId),
        ("clip_rect", RenderClipRect),
        ("clip_flags", ctypes.c_uint32),
    ]


class RenderResolvedClipRecord(ctypes.Structure):
    _fields_ = [
        ("clip_id", RenderRecordId),
        ("resolved_clip_rect", RenderClipRect),
        ("resolved_depth", ctypes.c_uint32),
        ("source_clip_flags", ctypes.c_uint32),
    ]


class RenderClipResolver(ctypes.Structure):
    _fields_ = [
        ("input_records", ctypes.POINTER(RenderClipRecord)),
        ("resolved_records", ctypes.POINTER(RenderResolvedClipRecord)),
        ("resolve_states", ctypes.POINTER(ctypes.c_int)),
        ("count", ctypes.c_uint32),
    ]


def find_compiler() -> str:
    for compiler_name in ("gcc", "clang", "cc"):
        compiler_path = shutil.which(compiler_name)
        if compiler_path:
            return compiler_path
    raise RuntimeError("No supported C compiler found for clip resolver tests.")


def make_record_id(handle: int, index: int) -> RenderRecordId:
    return RenderRecordId(handle=handle, index=index)


def make_clip_rect(min_x: float, min_y: float, max_x: float, max_y: float) -> RenderClipRect:
    return RenderClipRect(min_x=min_x, min_y=min_y, max_x=max_x, max_y=max_y)


def make_clip_record(
    handle: int,
    index: int,
    parent_handle: int,
    parent_index: int,
    clip_rect: RenderClipRect,
    flags: int,
) -> RenderClipRecord:
    return RenderClipRecord(
        clip_id=make_record_id(handle, index),
        parent_clip_id=make_record_id(parent_handle, parent_index),
        clip_rect=clip_rect,
        clip_flags=flags,
    )


def intersect_clip_rects(parent_rect: RenderClipRect, local_rect: RenderClipRect) -> tuple[float, float, float, float]:
    return (
        max(parent_rect.min_x, local_rect.min_x),
        max(parent_rect.min_y, local_rect.min_y),
        min(parent_rect.max_x, local_rect.max_x),
        min(parent_rect.max_y, local_rect.max_y),
    )


class ClipResolvePassTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.temp_dir = tempfile.TemporaryDirectory(prefix="lens_clip_resolver_")
        cls.library_path = Path(cls.temp_dir.name) / "render_clip_resolver_test.dll"
        compile_command = [
            find_compiler(),
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
        cls.library = ctypes.CDLL(str(cls.library_path))
        cls.configure_prototypes()

    @classmethod
    def tearDownClass(cls) -> None:
        if hasattr(cls, "library"):
            ctypes.WinDLL("kernel32", use_last_error=True).FreeLibrary(cls.library._handle)
        cls.temp_dir.cleanup()

    @classmethod
    def configure_prototypes(cls) -> None:
        cls.library.render_clip_resolver_initialize.argtypes = [
            ctypes.POINTER(RenderClipResolver),
            ctypes.POINTER(RenderClipRecord),
            ctypes.POINTER(RenderResolvedClipRecord),
            ctypes.POINTER(ctypes.c_int),
            ctypes.c_uint32,
        ]
        cls.library.render_clip_resolver_initialize.restype = ctypes.c_int

        cls.library.render_clip_resolver_reset.argtypes = [
            ctypes.POINTER(RenderClipResolver),
        ]
        cls.library.render_clip_resolver_reset.restype = None

        cls.library.render_clip_resolver_resolve_all.argtypes = [
            ctypes.POINTER(RenderClipResolver),
        ]
        cls.library.render_clip_resolver_resolve_all.restype = ctypes.c_int

        cls.library.render_clip_resolver_get_resolved_record.argtypes = [
            ctypes.POINTER(RenderClipResolver),
            ctypes.c_uint32,
        ]
        cls.library.render_clip_resolver_get_resolved_record.restype = (
            ctypes.POINTER(RenderResolvedClipRecord)
        )

    def initialize_resolver(self, input_records: list[RenderClipRecord]):
        input_array = (RenderClipRecord * len(input_records))(*input_records)
        resolved_array = (RenderResolvedClipRecord * len(input_records))()
        resolve_states = (ctypes.c_int * len(input_records))()
        resolver = RenderClipResolver()
        result = self.library.render_clip_resolver_initialize(
            ctypes.byref(resolver),
            input_array,
            resolved_array,
            resolve_states,
            len(input_records),
        )
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_OK, result)
        return resolver, input_array, resolved_array, resolve_states

    def assert_clip_rect_equal(
        self,
        clip_rect: RenderClipRect,
        expected_values: tuple[float, float, float, float],
    ) -> None:
        self.assertAlmostEqual(expected_values[0], clip_rect.min_x, places=5)
        self.assertAlmostEqual(expected_values[1], clip_rect.min_y, places=5)
        self.assertAlmostEqual(expected_values[2], clip_rect.max_x, places=5)
        self.assertAlmostEqual(expected_values[3], clip_rect.max_y, places=5)

    def test_resolves_output_in_stable_input_index_order(self) -> None:
        records = [
            make_clip_record(
                handle=4,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                clip_rect=make_clip_rect(0.0, 0.0, 10.0, 10.0),
                flags=100,
            ),
            make_clip_record(
                handle=2,
                index=1,
                parent_handle=4,
                parent_index=0,
                clip_rect=make_clip_rect(2.0, 2.0, 8.0, 8.0),
                flags=200,
            ),
            make_clip_record(
                handle=9,
                index=2,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                clip_rect=make_clip_rect(-5.0, -5.0, 5.0, 5.0),
                flags=300,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)
        self.assertFalse(
            self.library.render_clip_resolver_get_resolved_record(
                ctypes.byref(resolver),
                0,
            )
        )

        result = self.library.render_clip_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_OK, result)

        first_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            0,
        )
        second_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            1,
        )
        third_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            2,
        )

        self.assertTrue(first_record)
        self.assertTrue(second_record)
        self.assertTrue(third_record)
        self.assertEqual(4, first_record.contents.clip_id.handle)
        self.assertEqual(0, first_record.contents.clip_id.index)
        self.assertEqual(2, second_record.contents.clip_id.handle)
        self.assertEqual(1, second_record.contents.clip_id.index)
        self.assertEqual(9, third_record.contents.clip_id.handle)
        self.assertEqual(2, third_record.contents.clip_id.index)
        self.assertEqual(100, first_record.contents.source_clip_flags)
        self.assertEqual(200, second_record.contents.source_clip_flags)
        self.assertEqual(300, third_record.contents.source_clip_flags)

    def test_resolves_parent_clip_inheritance_by_intersection(self) -> None:
        root_rect = make_clip_rect(0.0, 0.0, 10.0, 10.0)
        child_rect = make_clip_rect(2.0, -2.0, 8.0, 12.0)
        grandchild_rect = make_clip_rect(4.0, 1.0, 12.0, 6.0)
        records = [
            make_clip_record(
                handle=10,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                clip_rect=root_rect,
                flags=10,
            ),
            make_clip_record(
                handle=11,
                index=1,
                parent_handle=10,
                parent_index=0,
                clip_rect=child_rect,
                flags=20,
            ),
            make_clip_record(
                handle=12,
                index=2,
                parent_handle=11,
                parent_index=1,
                clip_rect=grandchild_rect,
                flags=30,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_clip_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_OK, result)

        root_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            0,
        )
        child_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            1,
        )
        grandchild_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            2,
        )

        expected_child = intersect_clip_rects(root_rect, child_rect)
        expected_grandchild = (
            max(expected_child[0], grandchild_rect.min_x),
            max(expected_child[1], grandchild_rect.min_y),
            min(expected_child[2], grandchild_rect.max_x),
            min(expected_child[3], grandchild_rect.max_y),
        )

        self.assert_clip_rect_equal(
            root_record.contents.resolved_clip_rect,
            (0.0, 0.0, 10.0, 10.0),
        )
        self.assert_clip_rect_equal(
            child_record.contents.resolved_clip_rect,
            expected_child,
        )
        self.assert_clip_rect_equal(
            grandchild_record.contents.resolved_clip_rect,
            expected_grandchild,
        )
        self.assertEqual(0, root_record.contents.resolved_depth)
        self.assertEqual(1, child_record.contents.resolved_depth)
        self.assertEqual(2, grandchild_record.contents.resolved_depth)

    def test_root_and_default_clip_behavior_are_explicit(self) -> None:
        records = [
            make_clip_record(
                handle=15,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                clip_rect=make_clip_rect(1.0, 1.0, 9.0, 9.0),
                flags=77,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        self.assertEqual(RENDER_INVALID_HANDLE, resolved_array[0].clip_id.handle)
        self.assertEqual(RENDER_INVALID_INDEX, resolved_array[0].clip_id.index)
        self.assertAlmostEqual(DEFAULT_MIN, resolved_array[0].resolved_clip_rect.min_x, places=2)
        self.assertAlmostEqual(DEFAULT_MIN, resolved_array[0].resolved_clip_rect.min_y, places=2)
        self.assertAlmostEqual(DEFAULT_MAX, resolved_array[0].resolved_clip_rect.max_x, places=2)
        self.assertAlmostEqual(DEFAULT_MAX, resolved_array[0].resolved_clip_rect.max_y, places=2)

        result = self.library.render_clip_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_OK, result)
        resolved_record = self.library.render_clip_resolver_get_resolved_record(
            ctypes.byref(resolver),
            0,
        )
        self.assert_clip_rect_equal(
            resolved_record.contents.resolved_clip_rect,
            (1.0, 1.0, 9.0, 9.0),
        )

    def test_rejects_invalid_arguments(self) -> None:
        resolver = RenderClipResolver()
        result = self.library.render_clip_resolver_initialize(
            None,
            None,
            None,
            None,
            1,
        )
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT, result)

        result = self.library.render_clip_resolver_initialize(
            ctypes.byref(resolver),
            None,
            None,
            None,
            1,
        )
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT, result)

        result = self.library.render_clip_resolver_resolve_all(None)
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT, result)
        self.assertFalse(self.library.render_clip_resolver_get_resolved_record(None, 0))

    def test_detects_invalid_parent_links(self) -> None:
        records = [
            make_clip_record(
                handle=21,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                clip_rect=make_clip_rect(0.0, 0.0, 10.0, 10.0),
                flags=1,
            ),
            make_clip_record(
                handle=22,
                index=1,
                parent_handle=999,
                parent_index=0,
                clip_rect=make_clip_rect(1.0, 1.0, 9.0, 9.0),
                flags=2,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_clip_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_INVALID_PARENT_LINK, result)
        self.assertFalse(
            self.library.render_clip_resolver_get_resolved_record(
                ctypes.byref(resolver),
                1,
            )
        )

    def test_detects_out_of_range_parent_references(self) -> None:
        records = [
            make_clip_record(
                handle=30,
                index=0,
                parent_handle=RENDER_INVALID_HANDLE,
                parent_index=RENDER_INVALID_INDEX,
                clip_rect=make_clip_rect(0.0, 0.0, 10.0, 10.0),
                flags=1,
            ),
            make_clip_record(
                handle=31,
                index=1,
                parent_handle=30,
                parent_index=9,
                clip_rect=make_clip_rect(2.0, 2.0, 8.0, 8.0),
                flags=2,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_clip_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_OUT_OF_RANGE_PARENT, result)
        self.assertFalse(
            self.library.render_clip_resolver_get_resolved_record(
                ctypes.byref(resolver),
                1,
            )
        )

    def test_detects_cycles(self) -> None:
        records = [
            make_clip_record(
                handle=40,
                index=0,
                parent_handle=41,
                parent_index=1,
                clip_rect=make_clip_rect(0.0, 0.0, 5.0, 5.0),
                flags=1,
            ),
            make_clip_record(
                handle=41,
                index=1,
                parent_handle=40,
                parent_index=0,
                clip_rect=make_clip_rect(1.0, 1.0, 4.0, 4.0),
                flags=2,
            ),
        ]
        resolver, input_array, resolved_array, resolve_states = self.initialize_resolver(records)

        result = self.library.render_clip_resolver_resolve_all(ctypes.byref(resolver))
        self.assertEqual(RENDER_CLIP_RESOLVE_RESULT_CYCLE_DETECTED, result)
        self.assertFalse(
            self.library.render_clip_resolver_get_resolved_record(
                ctypes.byref(resolver),
                0,
            )
        )
        self.assertFalse(
            self.library.render_clip_resolver_get_resolved_record(
                ctypes.byref(resolver),
                1,
            )
        )


if __name__ == "__main__":
    unittest.main()
