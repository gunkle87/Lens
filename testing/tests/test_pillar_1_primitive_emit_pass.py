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


class RenderMatrix4x4(ctypes.Structure):
    _fields_ = [("elements", ctypes.c_float * 16)]


class RenderClipRect(ctypes.Structure):
    _fields_ = [
        ("min_x", ctypes.c_float),
        ("min_y", ctypes.c_float),
        ("max_x", ctypes.c_float),
        ("max_y", ctypes.c_float),
    ]


class RenderDrawItemRecord(ctypes.Structure):
    _fields_ = [
        ("draw_item_id", RenderRecordId),
        ("transform_id", RenderRecordId),
        ("clip_id", RenderRecordId),
        ("layer_order_id", RenderRecordId),
        ("source_handle", ctypes.c_uint32),
        ("draw_kind", ctypes.c_uint32),
        ("draw_flags", ctypes.c_uint32),
    ]


class RenderResolvedTransformRecord(ctypes.Structure):
    _fields_ = [
        ("transform_id", RenderRecordId),
        ("resolved_matrix", RenderMatrix4x4),
        ("resolved_depth", ctypes.c_uint32),
        ("source_transform_flags", ctypes.c_uint32),
    ]


class RenderResolvedClipRecord(ctypes.Structure):
    _fields_ = [
        ("clip_id", RenderRecordId),
        ("resolved_clip_rect", RenderClipRect),
        ("resolved_depth", ctypes.c_uint32),
        ("source_clip_flags", ctypes.c_uint32),
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


class RenderPrimitiveBatchRecord(ctypes.Structure):
    _fields_ = [
        ("draw_item_id", RenderRecordId),
        ("transform_id", RenderRecordId),
        ("clip_id", RenderRecordId),
        ("layer_order_id", RenderRecordId),
        ("resolved_transform_matrix", RenderMatrix4x4),
        ("resolved_clip_rect", RenderClipRect),
        ("layer_index", ctypes.c_int32),
        ("order_key", ctypes.c_int32),
        ("submission_index", ctypes.c_uint32),
        ("source_order_input_index", ctypes.c_uint32),
        ("source_handle", ctypes.c_uint32),
        ("draw_kind", ctypes.c_uint32),
        ("draw_flags", ctypes.c_uint32),
        ("source_transform_flags", ctypes.c_uint32),
        ("source_clip_flags", ctypes.c_uint32),
        ("source_order_flags", ctypes.c_uint32),
    ]


class RenderPrimitiveEmitter(ctypes.Structure):
    _fields_ = [
        ("draw_item_records", ctypes.POINTER(RenderDrawItemRecord)),
        ("resolved_transform_records", ctypes.POINTER(RenderResolvedTransformRecord)),
        ("resolved_clip_records", ctypes.POINTER(RenderResolvedClipRecord)),
        ("resolved_order_records", ctypes.POINTER(RenderResolvedOrderRecord)),
        ("primitive_batch_records", ctypes.POINTER(RenderPrimitiveBatchRecord)),
        ("draw_item_count", ctypes.c_uint32),
        ("transform_count", ctypes.c_uint32),
        ("clip_count", ctypes.c_uint32),
        ("order_count", ctypes.c_uint32),
    ]


class RenderPrimitiveEmitResult:
    OK = 0
    INVALID_ARGUMENT = 1
    INVALID_INPUT_REFERENCE = 2
    MISSING_RESOLVED_INPUT = 3


class Pillar1PrimitiveEmitPassTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        repo_root = Path(__file__).resolve().parents[2]
        source_file = repo_root / "src" / "render_primitive_emitter.c"
        include_dir = repo_root / "include"
        cls._temporary_directory = tempfile.TemporaryDirectory()
        library_path = Path(cls._temporary_directory.name) / "render_primitive_emitter.dll"

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
        cls._library.render_primitive_emitter_initialize.argtypes = [
            ctypes.POINTER(RenderPrimitiveEmitter),
            ctypes.POINTER(RenderDrawItemRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderResolvedTransformRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderResolvedClipRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderResolvedOrderRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderPrimitiveBatchRecord),
        ]
        cls._library.render_primitive_emitter_initialize.restype = ctypes.c_int
        cls._library.render_primitive_emitter_reset.argtypes = [
            ctypes.POINTER(RenderPrimitiveEmitter)
        ]
        cls._library.render_primitive_emitter_reset.restype = None
        cls._library.render_primitive_emitter_emit_all.argtypes = [
            ctypes.POINTER(RenderPrimitiveEmitter)
        ]
        cls._library.render_primitive_emitter_emit_all.restype = ctypes.c_int
        cls._library.render_primitive_emitter_get_batch_record.argtypes = [
            ctypes.POINTER(RenderPrimitiveEmitter),
            ctypes.c_uint32,
        ]
        cls._library.render_primitive_emitter_get_batch_record.restype = (
            ctypes.POINTER(RenderPrimitiveBatchRecord)
        )

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "_library_handle"):
            ctypes.WinDLL("kernel32").FreeLibrary(ctypes.c_void_p(cls._library_handle))
        if hasattr(cls, "_temporary_directory"):
            cls._temporary_directory.cleanup()

    def _record_id(self, handle, index):
        return RenderRecordId(handle=handle, index=index)

    def _matrix(self, seed_value):
        elements = (ctypes.c_float * 16)()
        for element_index in range(16):
            elements[element_index] = float(seed_value + element_index)
        return RenderMatrix4x4(elements=elements)

    def _clip_rect(self, min_x, min_y, max_x, max_y):
        return RenderClipRect(min_x=min_x, min_y=min_y, max_x=max_x, max_y=max_y)

    def _build_valid_inputs(self):
        draw_item_array = (RenderDrawItemRecord * 3)(
            RenderDrawItemRecord(
                draw_item_id=self._record_id(100, 0),
                transform_id=self._record_id(200, 0),
                clip_id=self._record_id(300, 0),
                layer_order_id=self._record_id(400, 0),
                source_handle=500,
                draw_kind=10,
                draw_flags=11,
            ),
            RenderDrawItemRecord(
                draw_item_id=self._record_id(101, 1),
                transform_id=self._record_id(201, 1),
                clip_id=self._record_id(301, 1),
                layer_order_id=self._record_id(401, 1),
                source_handle=501,
                draw_kind=12,
                draw_flags=13,
            ),
            RenderDrawItemRecord(
                draw_item_id=self._record_id(102, 2),
                transform_id=self._record_id(202, 2),
                clip_id=self._record_id(302, 2),
                layer_order_id=self._record_id(402, 2),
                source_handle=502,
                draw_kind=14,
                draw_flags=15,
            ),
        )

        transform_array = (RenderResolvedTransformRecord * 3)(
            RenderResolvedTransformRecord(
                transform_id=self._record_id(200, 0),
                resolved_matrix=self._matrix(1),
                resolved_depth=0,
                source_transform_flags=21,
            ),
            RenderResolvedTransformRecord(
                transform_id=self._record_id(201, 1),
                resolved_matrix=self._matrix(21),
                resolved_depth=1,
                source_transform_flags=22,
            ),
            RenderResolvedTransformRecord(
                transform_id=self._record_id(202, 2),
                resolved_matrix=self._matrix(41),
                resolved_depth=2,
                source_transform_flags=23,
            ),
        )

        clip_array = (RenderResolvedClipRecord * 3)(
            RenderResolvedClipRecord(
                clip_id=self._record_id(300, 0),
                resolved_clip_rect=self._clip_rect(0.0, 0.0, 10.0, 10.0),
                resolved_depth=0,
                source_clip_flags=31,
            ),
            RenderResolvedClipRecord(
                clip_id=self._record_id(301, 1),
                resolved_clip_rect=self._clip_rect(1.0, 2.0, 8.0, 9.0),
                resolved_depth=1,
                source_clip_flags=32,
            ),
            RenderResolvedClipRecord(
                clip_id=self._record_id(302, 2),
                resolved_clip_rect=self._clip_rect(2.0, 3.0, 7.0, 8.0),
                resolved_depth=2,
                source_clip_flags=33,
            ),
        )

        order_array = (RenderResolvedOrderRecord * 3)(
            RenderResolvedOrderRecord(
                layer_order_id=self._record_id(401, 1),
                draw_item_id=self._record_id(101, 1),
                layer_index=0,
                order_key=5,
                submission_index=3,
                source_input_index=1,
                source_order_flags=41,
            ),
            RenderResolvedOrderRecord(
                layer_order_id=self._record_id(402, 2),
                draw_item_id=self._record_id(102, 2),
                layer_index=0,
                order_key=5,
                submission_index=4,
                source_input_index=2,
                source_order_flags=42,
            ),
            RenderResolvedOrderRecord(
                layer_order_id=self._record_id(400, 0),
                draw_item_id=self._record_id(100, 0),
                layer_index=1,
                order_key=2,
                submission_index=1,
                source_input_index=0,
                source_order_flags=43,
            ),
        )

        batch_array = (RenderPrimitiveBatchRecord * 3)()
        return draw_item_array, transform_array, clip_array, order_array, batch_array

    def _initialize_emitter(
        self,
        draw_item_array,
        transform_array,
        clip_array,
        order_array,
        batch_array,
    ):
        emitter = RenderPrimitiveEmitter()
        result = self._library.render_primitive_emitter_initialize(
            ctypes.byref(emitter),
            draw_item_array,
            len(draw_item_array),
            transform_array,
            len(transform_array),
            clip_array,
            len(clip_array),
            order_array,
            len(order_array),
            batch_array,
        )
        self.assertEqual(result, RenderPrimitiveEmitResult.OK)
        return emitter

    def test_primitive_emitter_runtime_behavior(self):
        (
            draw_item_array,
            transform_array,
            clip_array,
            order_array,
            batch_array,
        ) = self._build_valid_inputs()
        emitter = self._initialize_emitter(
            draw_item_array,
            transform_array,
            clip_array,
            order_array,
            batch_array,
        )

        first_result = self._library.render_primitive_emitter_emit_all(
            ctypes.byref(emitter)
        )
        self.assertEqual(first_result, RenderPrimitiveEmitResult.OK)

        expected_draw_indices = [1, 2, 0]
        first_run_indices = []
        for output_index, expected_draw_index in enumerate(expected_draw_indices):
            batch_pointer = self._library.render_primitive_emitter_get_batch_record(
                ctypes.byref(emitter),
                output_index,
            )
            self.assertTrue(bool(batch_pointer))
            batch = batch_pointer.contents
            first_run_indices.append(batch.draw_item_id.index)
            self.assertEqual(batch.draw_item_id.index, expected_draw_index)
            self.assertEqual(batch.transform_id.index, expected_draw_index)
            self.assertEqual(batch.clip_id.index, expected_draw_index)
            self.assertEqual(batch.layer_order_id.index, expected_draw_index)
            self.assertEqual(
                batch.source_handle,
                draw_item_array[expected_draw_index].source_handle,
            )
            self.assertEqual(
                batch.draw_kind,
                draw_item_array[expected_draw_index].draw_kind,
            )
            self.assertEqual(
                batch.draw_flags,
                draw_item_array[expected_draw_index].draw_flags,
            )
            self.assertEqual(
                batch.source_transform_flags,
                transform_array[expected_draw_index].source_transform_flags,
            )
            self.assertEqual(
                batch.source_clip_flags,
                clip_array[expected_draw_index].source_clip_flags,
            )
            self.assertEqual(
                batch.source_order_flags,
                order_array[output_index].source_order_flags,
            )
            self.assertEqual(
                batch.source_order_input_index,
                order_array[output_index].source_input_index,
            )
            self.assertEqual(
                list(batch.resolved_transform_matrix.elements),
                list(transform_array[expected_draw_index].resolved_matrix.elements),
            )
            self.assertEqual(
                batch.resolved_clip_rect.min_x,
                clip_array[expected_draw_index].resolved_clip_rect.min_x,
            )
            self.assertEqual(
                batch.resolved_clip_rect.max_y,
                clip_array[expected_draw_index].resolved_clip_rect.max_y,
            )

        self._library.render_primitive_emitter_reset(ctypes.byref(emitter))

        second_result = self._library.render_primitive_emitter_emit_all(
            ctypes.byref(emitter)
        )
        self.assertEqual(second_result, RenderPrimitiveEmitResult.OK)

        second_run_indices = []
        for output_index in range(len(order_array)):
            batch_pointer = self._library.render_primitive_emitter_get_batch_record(
                ctypes.byref(emitter),
                output_index,
            )
            self.assertTrue(bool(batch_pointer))
            second_run_indices.append(batch_pointer.contents.draw_item_id.index)

        self.assertEqual(first_run_indices, second_run_indices)

        out_of_range_batch_pointer = self._library.render_primitive_emitter_get_batch_record(
            ctypes.byref(emitter),
            len(order_array),
        )
        self.assertFalse(bool(out_of_range_batch_pointer))

        invalid_initialize_result = self._library.render_primitive_emitter_initialize(
            None,
            None,
            0,
            None,
            0,
            None,
            0,
            None,
            0,
            None,
        )
        self.assertEqual(
            invalid_initialize_result,
            RenderPrimitiveEmitResult.INVALID_ARGUMENT,
        )

        invalid_storage_emitter = RenderPrimitiveEmitter()
        invalid_storage_emitter.order_count = 1
        invalid_emit_result = self._library.render_primitive_emitter_emit_all(
            ctypes.byref(invalid_storage_emitter)
        )
        self.assertEqual(
            invalid_emit_result,
            RenderPrimitiveEmitResult.INVALID_ARGUMENT,
        )

        (
            invalid_draw_item_array,
            invalid_transform_array,
            invalid_clip_array,
            invalid_order_array,
            invalid_batch_array,
        ) = self._build_valid_inputs()
        invalid_order_array[0].draw_item_id = self._record_id(999, 1)
        invalid_reference_emitter = self._initialize_emitter(
            invalid_draw_item_array,
            invalid_transform_array,
            invalid_clip_array,
            invalid_order_array,
            invalid_batch_array,
        )
        invalid_reference_result = self._library.render_primitive_emitter_emit_all(
            ctypes.byref(invalid_reference_emitter)
        )
        self.assertEqual(
            invalid_reference_result,
            RenderPrimitiveEmitResult.INVALID_INPUT_REFERENCE,
        )

        (
            missing_draw_item_array,
            missing_transform_array,
            missing_clip_array,
            missing_order_array,
            missing_batch_array,
        ) = self._build_valid_inputs()
        missing_transform_array[1].transform_id = self._record_id(777, 1)
        missing_resolution_emitter = self._initialize_emitter(
            missing_draw_item_array,
            missing_transform_array,
            missing_clip_array,
            missing_order_array,
            missing_batch_array,
        )
        missing_resolution_result = self._library.render_primitive_emitter_emit_all(
            ctypes.byref(missing_resolution_emitter)
        )
        self.assertEqual(
            missing_resolution_result,
            RenderPrimitiveEmitResult.MISSING_RESOLVED_INPUT,
        )


if __name__ == "__main__":
    unittest.main()
