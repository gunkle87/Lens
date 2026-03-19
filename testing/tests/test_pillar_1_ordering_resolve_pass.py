import ctypes
import subprocess
import tempfile
import unittest
from pathlib import Path


class RenderRecordId(ctypes.Structure):
    _fields_ = [
        ("handle", ctypes.c_uint32),
        ("index", ctypes.c_uint32),
    ]


class RenderLayerOrderInputRecord(ctypes.Structure):
    _fields_ = [
        ("layer_order_id", RenderRecordId),
        ("draw_item_id", RenderRecordId),
        ("layer_index", ctypes.c_int32),
        ("order_key", ctypes.c_int32),
        ("submission_index", ctypes.c_uint32),
        ("order_flags", ctypes.c_uint32),
    ]


class RenderResolvedOrderRecord(ctypes.Structure):
    _fields_ = [
        ("layer_order_id", RenderRecordId),
        ("draw_item_id", RenderRecordId),
        ("layer_index", ctypes.c_int32),
        ("order_key", ctypes.c_int32),
        ("submission_index", ctypes.c_uint32),
        ("source_input_index", ctypes.c_uint32),
        ("source_order_flags", ctypes.c_uint32),
    ]


class RenderOrderingResolver(ctypes.Structure):
    _fields_ = [
        ("input_records", ctypes.POINTER(RenderLayerOrderInputRecord)),
        ("resolved_records", ctypes.POINTER(RenderResolvedOrderRecord)),
        ("count", ctypes.c_uint32),
    ]


class RenderOrderingResolveResult:
    OK = 0
    INVALID_ARGUMENT = 1
    INVALID_RECORD_REFERENCE = 2


class Pillar1OrderingResolvePassTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        repo_root = Path(__file__).resolve().parents[2]
        source_file = repo_root / "src" / "render_ordering_resolver.c"
        include_dir = repo_root / "include"
        cls._temporary_directory = tempfile.TemporaryDirectory()
        library_path = Path(cls._temporary_directory.name) / "render_ordering_resolver.dll"

        subprocess.run(
            [
                "gcc",
                "-shared",
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-pedantic",
                "-I",
                str(include_dir),
                str(source_file),
                "-o",
                str(library_path),
            ],
            check=True,
            capture_output=True,
            text=True,
        )

        cls._library = ctypes.CDLL(str(library_path))
        cls._library_handle = cls._library._handle

        cls._library.render_ordering_resolver_initialize.argtypes = [
            ctypes.POINTER(RenderOrderingResolver),
            ctypes.POINTER(RenderLayerOrderInputRecord),
            ctypes.POINTER(RenderResolvedOrderRecord),
            ctypes.c_uint32,
        ]
        cls._library.render_ordering_resolver_initialize.restype = ctypes.c_int

        cls._library.render_ordering_resolver_reset.argtypes = [
            ctypes.POINTER(RenderOrderingResolver)
        ]
        cls._library.render_ordering_resolver_reset.restype = None

        cls._library.render_ordering_resolver_resolve_all.argtypes = [
            ctypes.POINTER(RenderOrderingResolver)
        ]
        cls._library.render_ordering_resolver_resolve_all.restype = ctypes.c_int

        cls._library.render_ordering_resolver_get_resolved_record.argtypes = [
            ctypes.POINTER(RenderOrderingResolver),
            ctypes.c_uint32,
        ]
        cls._library.render_ordering_resolver_get_resolved_record.restype = (
            ctypes.POINTER(RenderResolvedOrderRecord)
        )

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "_library_handle"):
            ctypes.WinDLL("kernel32").FreeLibrary(ctypes.c_void_p(cls._library_handle))
        if hasattr(cls, "_temporary_directory"):
            cls._temporary_directory.cleanup()

    def _record_id(self, handle, index):
        return RenderRecordId(handle=handle, index=index)

    def _build_input_record(
        self,
        input_index,
        draw_item_index,
        layer_index,
        order_key,
        submission_index,
        order_flags=0,
        layer_order_handle=None,
        draw_item_handle=None,
        layer_order_index=None,
        draw_item_id_index=None,
    ):
        if layer_order_handle is None:
            layer_order_handle = 100 + input_index
        if draw_item_handle is None:
            draw_item_handle = 200 + draw_item_index
        if layer_order_index is None:
            layer_order_index = input_index
        if draw_item_id_index is None:
            draw_item_id_index = draw_item_index

        return RenderLayerOrderInputRecord(
            layer_order_id=self._record_id(layer_order_handle, layer_order_index),
            draw_item_id=self._record_id(draw_item_handle, draw_item_id_index),
            layer_index=layer_index,
            order_key=order_key,
            submission_index=submission_index,
            order_flags=order_flags,
        )

    def _create_resolver(self, records):
        input_array_type = RenderLayerOrderInputRecord * len(records)
        resolved_array_type = RenderResolvedOrderRecord * len(records)
        input_array = input_array_type(*records)
        resolved_array = resolved_array_type()
        resolver = RenderOrderingResolver()

        result = self._library.render_ordering_resolver_initialize(
            ctypes.byref(resolver),
            input_array,
            resolved_array,
            len(records),
        )

        self.assertEqual(result, RenderOrderingResolveResult.OK)
        return resolver, input_array, resolved_array

    def test_ordering_resolver_runtime_behavior(self):
        records = [
            self._build_input_record(0, 0, 1, 5, 2, order_flags=10),
            self._build_input_record(1, 1, 0, 7, 4, order_flags=11),
            self._build_input_record(2, 2, 1, 5, 1, order_flags=12),
            self._build_input_record(3, 3, 1, 5, 1, order_flags=13),
            self._build_input_record(4, 4, 0, 7, 4, order_flags=14),
        ]

        resolver, input_array, resolved_array = self._create_resolver(records)

        first_result = self._library.render_ordering_resolver_resolve_all(
            ctypes.byref(resolver)
        )
        self.assertEqual(first_result, RenderOrderingResolveResult.OK)

        first_run_indices = []
        for output_index, expected_source_index in enumerate([1, 4, 2, 3, 0]):
            resolved_record_pointer = self._library.render_ordering_resolver_get_resolved_record(
                ctypes.byref(resolver),
                output_index,
            )
            self.assertTrue(bool(resolved_record_pointer))
            resolved_record = resolved_record_pointer.contents
            first_run_indices.append(resolved_record.source_input_index)
            self.assertEqual(resolved_record.source_input_index, expected_source_index)
            self.assertEqual(
                resolved_record.layer_order_id.index,
                expected_source_index,
            )
            self.assertEqual(
                resolved_record.source_order_flags,
                records[expected_source_index].order_flags,
            )

        self._library.render_ordering_resolver_reset(ctypes.byref(resolver))

        second_result = self._library.render_ordering_resolver_resolve_all(
            ctypes.byref(resolver)
        )
        self.assertEqual(second_result, RenderOrderingResolveResult.OK)

        second_run_indices = []
        for output_index in range(len(records)):
            resolved_record_pointer = self._library.render_ordering_resolver_get_resolved_record(
                ctypes.byref(resolver),
                output_index,
            )
            self.assertTrue(bool(resolved_record_pointer))
            second_run_indices.append(resolved_record_pointer.contents.source_input_index)

        self.assertEqual(first_run_indices, second_run_indices)

        out_of_range_record_pointer = self._library.render_ordering_resolver_get_resolved_record(
            ctypes.byref(resolver),
            len(records),
        )
        self.assertFalse(bool(out_of_range_record_pointer))

        invalid_initialize_result = self._library.render_ordering_resolver_initialize(
            None,
            None,
            None,
            0,
        )
        self.assertEqual(
            invalid_initialize_result,
            RenderOrderingResolveResult.INVALID_ARGUMENT,
        )

        invalid_storage_resolver = RenderOrderingResolver()
        invalid_storage_resolver.count = 1
        invalid_resolve_result = self._library.render_ordering_resolver_resolve_all(
            ctypes.byref(invalid_storage_resolver)
        )
        self.assertEqual(
            invalid_resolve_result,
            RenderOrderingResolveResult.INVALID_ARGUMENT,
        )

        invalid_reference_records = [
            self._build_input_record(0, 0, 0, 0, 0, layer_order_index=1),
        ]
        invalid_reference_resolver, invalid_reference_input_array, invalid_reference_resolved_array = self._create_resolver(
            invalid_reference_records
        )
        invalid_reference_result = self._library.render_ordering_resolver_resolve_all(
            ctypes.byref(invalid_reference_resolver)
        )
        self.assertEqual(
            invalid_reference_result,
            RenderOrderingResolveResult.INVALID_RECORD_REFERENCE,
        )

        invalid_draw_item_records = [
            self._build_input_record(
                0,
                0,
                0,
                0,
                0,
                draw_item_handle=0xFFFFFFFF,
            ),
        ]
        invalid_draw_item_resolver, invalid_draw_item_input_array, invalid_draw_item_resolved_array = self._create_resolver(
            invalid_draw_item_records
        )
        invalid_draw_item_result = self._library.render_ordering_resolver_resolve_all(
            ctypes.byref(invalid_draw_item_resolver)
        )
        self.assertEqual(
            invalid_draw_item_result,
            RenderOrderingResolveResult.INVALID_RECORD_REFERENCE,
        )

        self.assertIsNotNone(input_array)
        self.assertIsNotNone(resolved_array)
        self.assertIsNotNone(invalid_reference_input_array)
        self.assertIsNotNone(invalid_reference_resolved_array)
        self.assertIsNotNone(invalid_draw_item_input_array)
        self.assertIsNotNone(invalid_draw_item_resolved_array)


if __name__ == "__main__":
    unittest.main()
