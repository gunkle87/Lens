import ctypes
import subprocess
import tempfile
import unittest
from pathlib import Path


RENDER_INVALID_HANDLE = 0xFFFFFFFF
RENDER_INVALID_INDEX = 0xFFFFFFFF


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


class RenderBackendBoundaryResult:
    OK = 0
    INVALID_ARGUMENT = 1
    OUT_OF_BOUNDS = 2
    INVALID_STATE = 3
    INVALID_BATCH_REFERENCE = 4


class RenderBackendBoundaryState:
    READY_FOR_SUBMIT = 0
    READY_FOR_PRESENT = 1


class Pillar1BackendBoundaryInterfaceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        repo_root = Path(__file__).resolve().parents[2]
        source_file = repo_root / "src" / "render_backend_boundary.c"
        include_dir = repo_root / "include"
        cls._temporary_directory = tempfile.TemporaryDirectory()
        library_path = Path(cls._temporary_directory.name) / "render_backend_boundary.dll"

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
        cls._library.render_backend_boundary_initialize.argtypes = [
            ctypes.POINTER(RenderBackendBoundary),
            ctypes.c_uint32,
        ]
        cls._library.render_backend_boundary_initialize.restype = ctypes.c_int
        cls._library.render_backend_boundary_reset.argtypes = [
            ctypes.POINTER(RenderBackendBoundary)
        ]
        cls._library.render_backend_boundary_reset.restype = None
        cls._library.render_backend_boundary_submit.argtypes = [
            ctypes.POINTER(RenderBackendBoundary),
            ctypes.POINTER(RenderPrimitiveBatchRecord),
            ctypes.c_uint32,
        ]
        cls._library.render_backend_boundary_submit.restype = ctypes.c_int
        cls._library.render_backend_boundary_present.argtypes = [
            ctypes.POINTER(RenderBackendBoundary),
            ctypes.POINTER(RenderBackendBoundaryPresentRecord),
        ]
        cls._library.render_backend_boundary_present.restype = ctypes.c_int

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

    def _build_batch_array(self):
        return (RenderPrimitiveBatchRecord * 3)(
            RenderPrimitiveBatchRecord(
                draw_item_id=self._record_id(101, 1),
                transform_id=self._record_id(201, 1),
                clip_id=self._record_id(301, 1),
                layer_order_id=self._record_id(401, 1),
                resolved_transform_matrix=self._matrix(1),
                resolved_clip_rect=self._clip_rect(0.0, 0.0, 10.0, 10.0),
                layer_index=0,
                order_key=2,
                submission_index=3,
                source_order_input_index=1,
                source_handle=501,
                draw_kind=10,
                draw_flags=11,
                source_transform_flags=21,
                source_clip_flags=31,
                source_order_flags=41,
            ),
            RenderPrimitiveBatchRecord(
                draw_item_id=self._record_id(102, 2),
                transform_id=self._record_id(202, 2),
                clip_id=self._record_id(302, 2),
                layer_order_id=self._record_id(402, 2),
                resolved_transform_matrix=self._matrix(21),
                resolved_clip_rect=self._clip_rect(1.0, 1.0, 8.0, 8.0),
                layer_index=0,
                order_key=5,
                submission_index=4,
                source_order_input_index=2,
                source_handle=502,
                draw_kind=12,
                draw_flags=13,
                source_transform_flags=22,
                source_clip_flags=32,
                source_order_flags=42,
            ),
            RenderPrimitiveBatchRecord(
                draw_item_id=self._record_id(100, 0),
                transform_id=self._record_id(200, 0),
                clip_id=self._record_id(300, 0),
                layer_order_id=self._record_id(400, 0),
                resolved_transform_matrix=self._matrix(41),
                resolved_clip_rect=self._clip_rect(2.0, 2.0, 7.0, 7.0),
                layer_index=1,
                order_key=1,
                submission_index=1,
                source_order_input_index=0,
                source_handle=500,
                draw_kind=14,
                draw_flags=15,
                source_transform_flags=23,
                source_clip_flags=33,
                source_order_flags=43,
            ),
        )

    def test_backend_boundary_runtime_behavior(self):
        boundary = RenderBackendBoundary()
        initialize_result = self._library.render_backend_boundary_initialize(
            ctypes.byref(boundary),
            3,
        )
        self.assertEqual(initialize_result, RenderBackendBoundaryResult.OK)
        self.assertEqual(
            boundary.state,
            RenderBackendBoundaryState.READY_FOR_SUBMIT,
        )

        batch_array = self._build_batch_array()
        submit_result = self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            batch_array,
            len(batch_array),
        )
        self.assertEqual(submit_result, RenderBackendBoundaryResult.OK)
        self.assertEqual(
            boundary.state,
            RenderBackendBoundaryState.READY_FOR_PRESENT,
        )

        present_record = RenderBackendBoundaryPresentRecord()
        present_result = self._library.render_backend_boundary_present(
            ctypes.byref(boundary),
            ctypes.byref(present_record),
        )
        self.assertEqual(present_result, RenderBackendBoundaryResult.OK)
        self.assertEqual(present_record.presented_batch_count, 3)
        self.assertEqual(present_record.first_draw_item_id.index, 1)
        self.assertEqual(present_record.last_draw_item_id.index, 0)
        self.assertEqual(present_record.first_layer_order_id.index, 1)
        self.assertEqual(present_record.last_layer_order_id.index, 0)
        self.assertEqual(present_record.first_source_handle, 501)
        self.assertEqual(present_record.last_source_handle, 500)
        self.assertEqual(
            boundary.state,
            RenderBackendBoundaryState.READY_FOR_SUBMIT,
        )

        second_submit_result = self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            batch_array,
            len(batch_array),
        )
        self.assertEqual(second_submit_result, RenderBackendBoundaryResult.OK)

        second_present_record = RenderBackendBoundaryPresentRecord()
        second_present_result = self._library.render_backend_boundary_present(
            ctypes.byref(boundary),
            ctypes.byref(second_present_record),
        )
        self.assertEqual(second_present_result, RenderBackendBoundaryResult.OK)
        self.assertEqual(
            second_present_record.presented_batch_count,
            present_record.presented_batch_count,
        )
        self.assertEqual(
            second_present_record.first_draw_item_id.index,
            present_record.first_draw_item_id.index,
        )
        self.assertEqual(
            second_present_record.last_draw_item_id.index,
            present_record.last_draw_item_id.index,
        )
        self.assertEqual(
            second_present_record.first_source_handle,
            present_record.first_source_handle,
        )
        self.assertEqual(
            second_present_record.last_source_handle,
            present_record.last_source_handle,
        )

        zero_submit_result = self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            None,
            0,
        )
        self.assertEqual(zero_submit_result, RenderBackendBoundaryResult.OK)

        zero_present_record = RenderBackendBoundaryPresentRecord()
        zero_present_result = self._library.render_backend_boundary_present(
            ctypes.byref(boundary),
            ctypes.byref(zero_present_record),
        )
        self.assertEqual(zero_present_result, RenderBackendBoundaryResult.OK)
        self.assertEqual(zero_present_record.presented_batch_count, 0)
        self.assertEqual(
            zero_present_record.first_draw_item_id.handle,
            RENDER_INVALID_HANDLE,
        )
        self.assertEqual(
            zero_present_record.last_draw_item_id.index,
            RENDER_INVALID_INDEX,
        )

        out_of_bounds_result = self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            batch_array,
            4,
        )
        self.assertEqual(
            out_of_bounds_result,
            RenderBackendBoundaryResult.OUT_OF_BOUNDS,
        )

        invalid_initialize_result = self._library.render_backend_boundary_initialize(
            None,
            1,
        )
        self.assertEqual(
            invalid_initialize_result,
            RenderBackendBoundaryResult.INVALID_ARGUMENT,
        )

        invalid_submit_result = self._library.render_backend_boundary_submit(
            None,
            None,
            0,
        )
        self.assertEqual(
            invalid_submit_result,
            RenderBackendBoundaryResult.INVALID_ARGUMENT,
        )

        invalid_present_result = self._library.render_backend_boundary_present(
            ctypes.byref(boundary),
            None,
        )
        self.assertEqual(
            invalid_present_result,
            RenderBackendBoundaryResult.INVALID_ARGUMENT,
        )

        present_without_submit_result = self._library.render_backend_boundary_present(
            ctypes.byref(boundary),
            ctypes.byref(RenderBackendBoundaryPresentRecord()),
        )
        self.assertEqual(
            present_without_submit_result,
            RenderBackendBoundaryResult.INVALID_STATE,
        )

        self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            batch_array,
            len(batch_array),
        )
        submit_without_present_result = self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            batch_array,
            len(batch_array),
        )
        self.assertEqual(
            submit_without_present_result,
            RenderBackendBoundaryResult.INVALID_STATE,
        )

        self._library.render_backend_boundary_reset(ctypes.byref(boundary))
        invalid_batch_array = self._build_batch_array()
        invalid_batch_array[0].draw_item_id.handle = RENDER_INVALID_HANDLE
        invalid_batch_result = self._library.render_backend_boundary_submit(
            ctypes.byref(boundary),
            invalid_batch_array,
            len(invalid_batch_array),
        )
        self.assertEqual(
            invalid_batch_result,
            RenderBackendBoundaryResult.INVALID_BATCH_REFERENCE,
        )


if __name__ == "__main__":
    unittest.main()
