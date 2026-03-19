import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
HEADER_PATH = ROOT / "include" / "render_data_contracts.h"
SOURCE_PATH = ROOT / "src" / "render_data_contracts.c"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def get_struct_body(text: str, struct_name: str) -> str:
    pattern = re.compile(
        rf"typedef struct {struct_name}\s*\{{(?P<body>.*?)\}}\s*{struct_name};",
        re.DOTALL,
    )
    match = pattern.search(text)
    if match is None:
        raise AssertionError(f"Could not find struct body for {struct_name}.")
    return match.group("body")


class RenderDataContractsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.header_text = read_text(HEADER_PATH)
        cls.source_text = read_text(SOURCE_PATH)

    def test_contract_files_exist(self) -> None:
        self.assertTrue(HEADER_PATH.is_file())
        self.assertTrue(SOURCE_PATH.is_file())

    def test_header_declares_core_identity_macros_and_records(self) -> None:
        self.assertIn("#define RENDER_INVALID_HANDLE UINT32_MAX", self.header_text)
        self.assertIn("#define RENDER_INVALID_INDEX UINT32_MAX", self.header_text)
        self.assertIn("#define RENDER_MATRIX_4X4_ELEMENT_COUNT 16U", self.header_text)
        self.assertIn("typedef struct RenderRecordId", self.header_text)
        self.assertIn("uint32_t handle;", self.header_text)
        self.assertIn("uint32_t index;", self.header_text)
        self.assertIn("typedef struct RenderMatrix4x4", self.header_text)
        self.assertIn("typedef struct RenderClipRect", self.header_text)

    def test_draw_item_record_references_transform_clip_and_order_inputs(self) -> None:
        body = get_struct_body(self.header_text, "RenderDrawItemRecord")
        self.assertIn("RenderRecordId draw_item_id;", body)
        self.assertIn("RenderRecordId transform_id;", body)
        self.assertIn("RenderRecordId clip_id;", body)
        self.assertIn("RenderRecordId layer_order_id;", body)
        self.assertIn("uint32_t source_handle;", body)
        self.assertIn("uint32_t draw_kind;", body)
        self.assertIn("uint32_t draw_flags;", body)

    def test_transform_and_clip_records_keep_identity_and_parent_links(self) -> None:
        transform_body = get_struct_body(self.header_text, "RenderTransformRecord")
        self.assertIn("RenderRecordId transform_id;", transform_body)
        self.assertIn("RenderRecordId parent_transform_id;", transform_body)
        self.assertIn("RenderMatrix4x4 local_matrix;", transform_body)
        self.assertIn("uint32_t transform_flags;", transform_body)

        clip_body = get_struct_body(self.header_text, "RenderClipRecord")
        self.assertIn("RenderRecordId clip_id;", clip_body)
        self.assertIn("RenderRecordId parent_clip_id;", clip_body)
        self.assertIn("RenderClipRect clip_rect;", clip_body)
        self.assertIn("uint32_t clip_flags;", clip_body)

    def test_layer_order_record_tracks_draw_item_identity_and_sort_inputs(self) -> None:
        body = get_struct_body(self.header_text, "RenderLayerOrderInputRecord")
        self.assertIn("RenderRecordId layer_order_id;", body)
        self.assertIn("RenderRecordId draw_item_id;", body)
        self.assertIn("int32_t layer_index;", body)
        self.assertIn("int32_t order_key;", body)
        self.assertIn("uint32_t submission_index;", body)
        self.assertIn("uint32_t order_flags;", body)

    def test_source_stays_data_only_and_uses_compile_time_layout_checks(self) -> None:
        self.assertIn('_Static_assert(sizeof(RenderRecordId)', self.source_text)
        self.assertIn('offsetof(RenderDrawItemRecord, draw_item_id)', self.source_text)
        self.assertIn('offsetof(RenderTransformRecord, transform_id)', self.source_text)
        self.assertIn('offsetof(RenderClipRecord, clip_id)', self.source_text)
        self.assertIn(
            'offsetof(RenderLayerOrderInputRecord, layer_order_id)',
            self.source_text,
        )
        self.assertNotRegex(self.source_text, re.compile(r"\)\s*\n\{"))


if __name__ == "__main__":
    unittest.main()
