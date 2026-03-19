import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
HEADER_PATH = ROOT / "include" / "render_command_buffer.h"
SOURCE_PATH = ROOT / "src" / "render_command_buffer.c"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


class CommandInputBufferTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.header_text = read_text(HEADER_PATH)
        cls.source_text = read_text(SOURCE_PATH)

    def test_command_buffer_files_exist(self) -> None:
        self.assertTrue(HEADER_PATH.is_file())
        self.assertTrue(SOURCE_PATH.is_file())

    def test_header_declares_result_codes_and_buffer_layout(self) -> None:
        self.assertIn("typedef enum RenderCommandBufferResult", self.header_text)
        self.assertIn("RENDER_COMMAND_BUFFER_RESULT_OK = 0", self.header_text)
        self.assertIn(
            "RENDER_COMMAND_BUFFER_RESULT_INVALID_ARGUMENT = 1",
            self.header_text,
        )
        self.assertIn(
            "RENDER_COMMAND_BUFFER_RESULT_OUT_OF_CAPACITY = 2",
            self.header_text,
        )
        self.assertIn("typedef struct RenderCommandBuffer", self.header_text)
        self.assertIn("RenderDrawItemRecord *draw_item_records;", self.header_text)
        self.assertIn("uint32_t capacity;", self.header_text)
        self.assertIn("uint32_t count;", self.header_text)

    def test_header_declares_deterministic_access_pattern(self) -> None:
        self.assertIn("render_command_buffer_initialize(", self.header_text)
        self.assertIn("render_command_buffer_reset(", self.header_text)
        self.assertIn("render_command_buffer_append(", self.header_text)
        self.assertIn("RenderRecordId *assigned_draw_item_id", self.header_text)
        self.assertIn("render_command_buffer_get_record(", self.header_text)

    def test_initialize_and_reset_clear_runtime_state(self) -> None:
        self.assertIn(
            "RenderCommandBufferResult render_command_buffer_initialize(",
            self.source_text,
        )
        self.assertIn("if (command_buffer == NULL)", self.source_text)
        self.assertIn(
            "if ((draw_item_records == NULL) && (capacity > 0U))",
            self.source_text,
        )
        self.assertIn(
            "command_buffer->draw_item_records = draw_item_records;",
            self.source_text,
        )
        self.assertIn("command_buffer->capacity = capacity;", self.source_text)
        self.assertIn("command_buffer->count = 0U;", self.source_text)
        self.assertIn("void render_command_buffer_reset(", self.source_text)

    def test_append_assigns_stable_indices_and_rejects_overflow(self) -> None:
        self.assertIn(
            "RenderCommandBufferResult render_command_buffer_append(",
            self.source_text,
        )
        self.assertIn(
            "if ((command_buffer == NULL) || (draw_item_record == NULL))",
            self.source_text,
        )
        self.assertIn(
            "if (command_buffer->draw_item_records == NULL)",
            self.source_text,
        )
        self.assertIn(
            "if (command_buffer->count >= command_buffer->capacity)",
            self.source_text,
        )
        self.assertIn("assigned_index = command_buffer->count;", self.source_text)
        self.assertIn("stored_record = *draw_item_record;", self.source_text)
        self.assertIn(
            "stored_record.draw_item_id.index = assigned_index;",
            self.source_text,
        )
        self.assertIn(
            "command_buffer->draw_item_records[assigned_index] = stored_record;",
            self.source_text,
        )
        self.assertIn(
            "command_buffer->count = assigned_index + 1U;",
            self.source_text,
        )
        self.assertIn(
            "return RENDER_COMMAND_BUFFER_RESULT_OUT_OF_CAPACITY;",
            self.source_text,
        )

    def test_get_record_exposes_only_appended_range(self) -> None:
        self.assertIn(
            "const RenderDrawItemRecord *render_command_buffer_get_record(",
            self.source_text,
        )
        self.assertIn("if (index >= command_buffer->count)", self.source_text)
        self.assertIn(
            "return &command_buffer->draw_item_records[index];",
            self.source_text,
        )


if __name__ == "__main__":
    unittest.main()
