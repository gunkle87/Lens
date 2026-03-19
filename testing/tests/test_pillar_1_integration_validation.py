import ctypes
import subprocess
import tempfile
import unittest
from pathlib import Path


RENDER_INVALID_HANDLE = 0xFFFFFFFF
RENDER_INVALID_INDEX = 0xFFFFFFFF


class RenderRecordId(ctypes.Structure):
    _fields_ = [("handle", ctypes.c_uint32), ("index", ctypes.c_uint32)]


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


class RenderTransformRecord(ctypes.Structure):
    _fields_ = [
        ("transform_id", RenderRecordId),
        ("parent_transform_id", RenderRecordId),
        ("local_matrix", RenderMatrix4x4),
        ("transform_flags", ctypes.c_uint32),
    ]


class RenderClipRecord(ctypes.Structure):
    _fields_ = [
        ("clip_id", RenderRecordId),
        ("parent_clip_id", RenderRecordId),
        ("clip_rect", RenderClipRect),
        ("clip_flags", ctypes.c_uint32),
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


class RenderCommandBuffer(ctypes.Structure):
    _fields_ = [
        ("draw_item_records", ctypes.POINTER(RenderDrawItemRecord)),
        ("capacity", ctypes.c_uint32),
        ("count", ctypes.c_uint32),
    ]


class RenderTransformResolver(ctypes.Structure):
    _fields_ = [
        ("input_records", ctypes.POINTER(RenderTransformRecord)),
        ("resolved_records", ctypes.POINTER(RenderResolvedTransformRecord)),
        ("resolve_states", ctypes.POINTER(ctypes.c_uint32)),
        ("count", ctypes.c_uint32),
    ]


class RenderClipResolver(ctypes.Structure):
    _fields_ = [
        ("input_records", ctypes.POINTER(RenderClipRecord)),
        ("resolved_records", ctypes.POINTER(RenderResolvedClipRecord)),
        ("resolve_states", ctypes.POINTER(ctypes.c_uint32)),
        ("count", ctypes.c_uint32),
    ]


class RenderOrderingResolver(ctypes.Structure):
    _fields_ = [
        ("input_records", ctypes.POINTER(RenderLayerOrderInputRecord)),
        ("resolved_records", ctypes.POINTER(RenderResolvedOrderRecord)),
        ("count", ctypes.c_uint32),
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


class RenderBackendBoundaryPresentRecord(ctypes.Structure):
    _fields_ = [
        ("presented_batch_count", ctypes.c_uint32),
        ("first_draw_item_id", RenderRecordId),
        ("last_draw_item_id", RenderRecordId),
        ("first_layer_order_id", RenderRecordId),
        ("last_layer_order_id", RenderRecordId),
        ("first_source_handle", ctypes.c_uint32),
        ("last_source_handle", ctypes.c_uint32),
    ]


class RenderBackendBoundary(ctypes.Structure):
    _fields_ = [
        ("max_primitive_batch_count", ctypes.c_uint32),
        ("state", ctypes.c_uint32),
        ("pending_present_record", RenderBackendBoundaryPresentRecord),
    ]


class RenderNativePipelineExecutionRecord(ctypes.Structure):
    _fields_ = [
        ("failed_stage", ctypes.c_uint32),
        ("stage_result_code", ctypes.c_int32),
        ("ingested_draw_item_count", ctypes.c_uint32),
        ("emitted_primitive_count", ctypes.c_uint32),
        ("present_record", RenderBackendBoundaryPresentRecord),
    ]


class RenderNativePipeline(ctypes.Structure):
    _fields_ = [
        ("command_buffer", RenderCommandBuffer),
        ("transform_resolver", RenderTransformResolver),
        ("clip_resolver", RenderClipResolver),
        ("ordering_resolver", RenderOrderingResolver),
        ("primitive_emitter", RenderPrimitiveEmitter),
        ("backend_boundary", RenderBackendBoundary),
        ("resolved_transform_records", ctypes.POINTER(RenderResolvedTransformRecord)),
        ("transform_resolve_states", ctypes.POINTER(ctypes.c_uint32)),
        ("max_transform_count", ctypes.c_uint32),
        ("resolved_clip_records", ctypes.POINTER(RenderResolvedClipRecord)),
        ("clip_resolve_states", ctypes.POINTER(ctypes.c_uint32)),
        ("max_clip_count", ctypes.c_uint32),
        ("resolved_order_records", ctypes.POINTER(RenderResolvedOrderRecord)),
        ("max_order_count", ctypes.c_uint32),
        ("primitive_batch_records", ctypes.POINTER(RenderPrimitiveBatchRecord)),
        ("max_primitive_batch_count", ctypes.c_uint32),
    ]


class RenderNativePipelineResult:
    OK = 0
    INVALID_ARGUMENT = 1
    OUT_OF_CAPACITY = 2
    STAGE_FAILURE = 3


class RenderNativePipelineStage:
    NONE = 0
    COMMAND_INPUT = 1
    TRANSFORM_RESOLVE = 2
    CLIP_RESOLVE = 3
    ORDERING_RESOLVE = 4
    PRIMITIVE_EMIT = 5
    BACKEND_SUBMIT = 6
    BACKEND_PRESENT = 7


class RenderPrimitiveEmitResult:
    MISSING_RESOLVED_INPUT = 3


class Pillar1IntegrationValidationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        repo_root = Path(__file__).resolve().parents[2]
        include_dir = repo_root / "include"
        source_files = [
            repo_root / "src" / "render_native_pipeline.c",
            repo_root / "src" / "render_command_buffer.c",
            repo_root / "src" / "render_transform_resolver.c",
            repo_root / "src" / "render_clip_resolver.c",
            repo_root / "src" / "render_ordering_resolver.c",
            repo_root / "src" / "render_primitive_emitter.c",
            repo_root / "src" / "render_backend_boundary.c",
        ]
        cls._temporary_directory = tempfile.TemporaryDirectory()
        library_path = Path(cls._temporary_directory.name) / "render_native_pipeline.dll"

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
                *[str(source_file) for source_file in source_files],
                "-o",
                str(library_path),
            ],
            check=True,
            capture_output=True,
            text=True,
        )

        cls._library = ctypes.CDLL(str(library_path))
        cls._library_handle = cls._library._handle
        cls._library.render_native_pipeline_initialize.argtypes = [
            ctypes.POINTER(RenderNativePipeline),
            ctypes.POINTER(RenderDrawItemRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderResolvedTransformRecord),
            ctypes.POINTER(ctypes.c_uint32),
            ctypes.c_uint32,
            ctypes.POINTER(RenderResolvedClipRecord),
            ctypes.POINTER(ctypes.c_uint32),
            ctypes.c_uint32,
            ctypes.POINTER(RenderResolvedOrderRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderPrimitiveBatchRecord),
            ctypes.c_uint32,
            ctypes.c_uint32,
        ]
        cls._library.render_native_pipeline_initialize.restype = ctypes.c_int
        cls._library.render_native_pipeline_reset.argtypes = [
            ctypes.POINTER(RenderNativePipeline)
        ]
        cls._library.render_native_pipeline_reset.restype = None
        cls._library.render_native_pipeline_execute.argtypes = [
            ctypes.POINTER(RenderNativePipeline),
            ctypes.POINTER(RenderDrawItemRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderTransformRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderClipRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderLayerOrderInputRecord),
            ctypes.c_uint32,
            ctypes.POINTER(RenderNativePipelineExecutionRecord),
        ]
        cls._library.render_native_pipeline_execute.restype = ctypes.c_int

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "_library_handle"):
            ctypes.WinDLL("kernel32").FreeLibrary(ctypes.c_void_p(cls._library_handle))
        if hasattr(cls, "_temporary_directory"):
            cls._temporary_directory.cleanup()

    def _record_id(self, handle, index):
        return RenderRecordId(handle=handle, index=index)

    def _root_parent(self):
        return self._record_id(RENDER_INVALID_HANDLE, RENDER_INVALID_INDEX)

    def _matrix(self, seed_value):
        elements = (ctypes.c_float * 16)()
        for element_index in range(16):
            elements[element_index] = float(seed_value + element_index)
        return RenderMatrix4x4(elements=elements)

    def _clip_rect(self, min_x, min_y, max_x, max_y):
        return RenderClipRect(min_x=min_x, min_y=min_y, max_x=max_x, max_y=max_y)

    def _build_pipeline_storage(self, capacity):
        return (
            (RenderDrawItemRecord * capacity)(),
            (RenderResolvedTransformRecord * capacity)(),
            (ctypes.c_uint32 * capacity)(),
            (RenderResolvedClipRecord * capacity)(),
            (ctypes.c_uint32 * capacity)(),
            (RenderResolvedOrderRecord * capacity)(),
            (RenderPrimitiveBatchRecord * capacity)(),
        )

    def _build_valid_inputs(self):
        draw_records = (RenderDrawItemRecord * 3)(
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

        transform_records = (RenderTransformRecord * 3)(
            RenderTransformRecord(
                transform_id=self._record_id(200, 0),
                parent_transform_id=self._root_parent(),
                local_matrix=self._matrix(1),
                transform_flags=21,
            ),
            RenderTransformRecord(
                transform_id=self._record_id(201, 1),
                parent_transform_id=self._root_parent(),
                local_matrix=self._matrix(21),
                transform_flags=22,
            ),
            RenderTransformRecord(
                transform_id=self._record_id(202, 2),
                parent_transform_id=self._root_parent(),
                local_matrix=self._matrix(41),
                transform_flags=23,
            ),
        )

        clip_records = (RenderClipRecord * 3)(
            RenderClipRecord(
                clip_id=self._record_id(300, 0),
                parent_clip_id=self._root_parent(),
                clip_rect=self._clip_rect(0.0, 0.0, 10.0, 10.0),
                clip_flags=31,
            ),
            RenderClipRecord(
                clip_id=self._record_id(301, 1),
                parent_clip_id=self._root_parent(),
                clip_rect=self._clip_rect(1.0, 1.0, 8.0, 8.0),
                clip_flags=32,
            ),
            RenderClipRecord(
                clip_id=self._record_id(302, 2),
                parent_clip_id=self._root_parent(),
                clip_rect=self._clip_rect(2.0, 2.0, 7.0, 7.0),
                clip_flags=33,
            ),
        )

        layer_order_records = (RenderLayerOrderInputRecord * 3)(
            RenderLayerOrderInputRecord(
                layer_order_id=self._record_id(400, 0),
                draw_item_id=self._record_id(100, 0),
                layer_index=1,
                order_key=2,
                submission_index=1,
                order_flags=41,
            ),
            RenderLayerOrderInputRecord(
                layer_order_id=self._record_id(401, 1),
                draw_item_id=self._record_id(101, 1),
                layer_index=0,
                order_key=5,
                submission_index=3,
                order_flags=42,
            ),
            RenderLayerOrderInputRecord(
                layer_order_id=self._record_id(402, 2),
                draw_item_id=self._record_id(102, 2),
                layer_index=0,
                order_key=5,
                submission_index=4,
                order_flags=43,
            ),
        )

        return draw_records, transform_records, clip_records, layer_order_records

    def _initialize_pipeline(self, capacity):
        (
            draw_storage,
            resolved_transform_storage,
            transform_state_storage,
            resolved_clip_storage,
            clip_state_storage,
            resolved_order_storage,
            primitive_batch_storage,
        ) = self._build_pipeline_storage(capacity)
        pipeline = RenderNativePipeline()
        initialize_result = self._library.render_native_pipeline_initialize(
            ctypes.byref(pipeline),
            draw_storage,
            capacity,
            resolved_transform_storage,
            transform_state_storage,
            capacity,
            resolved_clip_storage,
            clip_state_storage,
            capacity,
            resolved_order_storage,
            capacity,
            primitive_batch_storage,
            capacity,
            capacity,
        )
        self.assertEqual(initialize_result, RenderNativePipelineResult.OK)
        return pipeline, draw_storage, resolved_transform_storage, transform_state_storage, resolved_clip_storage, clip_state_storage, resolved_order_storage, primitive_batch_storage

    def test_native_pipeline_runtime_behavior(self):
        pipeline, draw_storage, resolved_transform_storage, transform_state_storage, resolved_clip_storage, clip_state_storage, resolved_order_storage, primitive_batch_storage = self._initialize_pipeline(3)
        draw_records, transform_records, clip_records, layer_order_records = self._build_valid_inputs()

        first_execution_record = RenderNativePipelineExecutionRecord()
        first_result = self._library.render_native_pipeline_execute(
            ctypes.byref(pipeline),
            draw_records,
            len(draw_records),
            transform_records,
            len(transform_records),
            clip_records,
            len(clip_records),
            layer_order_records,
            len(layer_order_records),
            ctypes.byref(first_execution_record),
        )
        self.assertEqual(first_result, RenderNativePipelineResult.OK)
        self.assertEqual(first_execution_record.failed_stage, RenderNativePipelineStage.NONE)
        self.assertEqual(first_execution_record.ingested_draw_item_count, 3)
        self.assertEqual(first_execution_record.emitted_primitive_count, 3)
        self.assertEqual(first_execution_record.present_record.presented_batch_count, 3)
        self.assertEqual(first_execution_record.present_record.first_draw_item_id.index, 1)
        self.assertEqual(first_execution_record.present_record.last_draw_item_id.index, 0)
        self.assertEqual(first_execution_record.present_record.first_source_handle, 501)
        self.assertEqual(first_execution_record.present_record.last_source_handle, 500)

        second_execution_record = RenderNativePipelineExecutionRecord()
        second_result = self._library.render_native_pipeline_execute(
            ctypes.byref(pipeline),
            draw_records,
            len(draw_records),
            transform_records,
            len(transform_records),
            clip_records,
            len(clip_records),
            layer_order_records,
            len(layer_order_records),
            ctypes.byref(second_execution_record),
        )
        self.assertEqual(second_result, RenderNativePipelineResult.OK)
        self.assertEqual(second_execution_record.present_record.presented_batch_count, first_execution_record.present_record.presented_batch_count)
        self.assertEqual(second_execution_record.present_record.first_draw_item_id.index, first_execution_record.present_record.first_draw_item_id.index)
        self.assertEqual(second_execution_record.present_record.last_draw_item_id.index, first_execution_record.present_record.last_draw_item_id.index)

        invalid_argument_result = self._library.render_native_pipeline_execute(None, None, 0, None, 0, None, 0, None, 0, None)
        self.assertEqual(invalid_argument_result, RenderNativePipelineResult.INVALID_ARGUMENT)

        small_pipeline, small_draw_storage, small_resolved_transform_storage, small_transform_state_storage, small_resolved_clip_storage, small_clip_state_storage, small_resolved_order_storage, small_primitive_batch_storage = self._initialize_pipeline(2)
        capacity_execution_record = RenderNativePipelineExecutionRecord()
        capacity_result = self._library.render_native_pipeline_execute(
            ctypes.byref(small_pipeline),
            draw_records,
            len(draw_records),
            transform_records,
            len(transform_records),
            clip_records,
            len(clip_records),
            layer_order_records,
            len(layer_order_records),
            ctypes.byref(capacity_execution_record),
        )
        self.assertEqual(capacity_result, RenderNativePipelineResult.OUT_OF_CAPACITY)
        self.assertEqual(capacity_execution_record.failed_stage, RenderNativePipelineStage.COMMAND_INPUT)

        missing_transform_records = (RenderTransformRecord * 2)(transform_records[0], transform_records[1])
        missing_execution_record = RenderNativePipelineExecutionRecord()
        missing_result = self._library.render_native_pipeline_execute(
            ctypes.byref(pipeline),
            draw_records,
            len(draw_records),
            missing_transform_records,
            len(missing_transform_records),
            clip_records,
            len(clip_records),
            layer_order_records,
            len(layer_order_records),
            ctypes.byref(missing_execution_record),
        )
        self.assertEqual(missing_result, RenderNativePipelineResult.STAGE_FAILURE)
        self.assertEqual(missing_execution_record.failed_stage, RenderNativePipelineStage.PRIMITIVE_EMIT)
        self.assertEqual(missing_execution_record.stage_result_code, RenderPrimitiveEmitResult.MISSING_RESOLVED_INPUT)

        self.assertIsNotNone(draw_storage)
        self.assertIsNotNone(resolved_transform_storage)
        self.assertIsNotNone(transform_state_storage)
        self.assertIsNotNone(resolved_clip_storage)
        self.assertIsNotNone(clip_state_storage)
        self.assertIsNotNone(resolved_order_storage)
        self.assertIsNotNone(primitive_batch_storage)
        self.assertIsNotNone(small_draw_storage)
        self.assertIsNotNone(small_resolved_transform_storage)
        self.assertIsNotNone(small_transform_state_storage)
        self.assertIsNotNone(small_resolved_clip_storage)
        self.assertIsNotNone(small_clip_state_storage)
        self.assertIsNotNone(small_resolved_order_storage)
        self.assertIsNotNone(small_primitive_batch_storage)


if __name__ == "__main__":
    unittest.main()
