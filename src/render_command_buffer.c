#include "render_command_buffer.h"

#include <stddef.h>

_Static_assert(offsetof(RenderCommandBuffer, draw_item_records) == 0U,
	"RenderCommandBuffer storage must lead the record.");
_Static_assert(sizeof(RenderCommandBufferResult) == sizeof(int),
	"RenderCommandBufferResult must stay enum-sized for C call boundaries.");

RenderCommandBufferResult render_command_buffer_initialize(
	RenderCommandBuffer *command_buffer,
	RenderDrawItemRecord *draw_item_records,
	uint32_t capacity)
{
	if (command_buffer == NULL)
	{
		return RENDER_COMMAND_BUFFER_RESULT_INVALID_ARGUMENT;
	}

	if ((draw_item_records == NULL) && (capacity > 0U))
	{
		return RENDER_COMMAND_BUFFER_RESULT_INVALID_ARGUMENT;
	}

	command_buffer->draw_item_records = draw_item_records;
	command_buffer->capacity = capacity;
	command_buffer->count = 0U;
	return RENDER_COMMAND_BUFFER_RESULT_OK;
}

void render_command_buffer_reset(RenderCommandBuffer *command_buffer)
{
	if (command_buffer == NULL)
	{
		return;
	}

	command_buffer->count = 0U;
}

RenderCommandBufferResult render_command_buffer_append(
	RenderCommandBuffer *command_buffer,
	const RenderDrawItemRecord *draw_item_record,
	RenderRecordId *assigned_draw_item_id)
{
	RenderDrawItemRecord stored_record;
	uint32_t assigned_index;

	if ((command_buffer == NULL) || (draw_item_record == NULL))
	{
		return RENDER_COMMAND_BUFFER_RESULT_INVALID_ARGUMENT;
	}

	if (command_buffer->draw_item_records == NULL)
	{
		return RENDER_COMMAND_BUFFER_RESULT_INVALID_ARGUMENT;
	}

	if (command_buffer->count >= command_buffer->capacity)
	{
		return RENDER_COMMAND_BUFFER_RESULT_OUT_OF_CAPACITY;
	}

	assigned_index = command_buffer->count;
	stored_record = *draw_item_record;
	stored_record.draw_item_id.index = assigned_index;
	command_buffer->draw_item_records[assigned_index] = stored_record;
	command_buffer->count = assigned_index + 1U;

	if (assigned_draw_item_id != NULL)
	{
		*assigned_draw_item_id = stored_record.draw_item_id;
	}

	return RENDER_COMMAND_BUFFER_RESULT_OK;
}

const RenderDrawItemRecord *render_command_buffer_get_record(
	const RenderCommandBuffer *command_buffer,
	uint32_t index)
{
	if (command_buffer == NULL)
	{
		return NULL;
	}

	if (index >= command_buffer->count)
	{
		return NULL;
	}

	return &command_buffer->draw_item_records[index];
}
