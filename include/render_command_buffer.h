#ifndef RENDER_COMMAND_BUFFER_H
#define RENDER_COMMAND_BUFFER_H

#include <stdint.h>

#include "render_data_contracts.h"

typedef enum RenderCommandBufferResult
{
	RENDER_COMMAND_BUFFER_RESULT_OK = 0,
	RENDER_COMMAND_BUFFER_RESULT_INVALID_ARGUMENT = 1,
	RENDER_COMMAND_BUFFER_RESULT_OUT_OF_CAPACITY = 2
} RenderCommandBufferResult;

typedef struct RenderCommandBuffer
{
	RenderDrawItemRecord *draw_item_records;
	uint32_t capacity;
	uint32_t count;
} RenderCommandBuffer;

RenderCommandBufferResult render_command_buffer_initialize(
	RenderCommandBuffer *command_buffer,
	RenderDrawItemRecord *draw_item_records,
	uint32_t capacity);

void render_command_buffer_reset(RenderCommandBuffer *command_buffer);

RenderCommandBufferResult render_command_buffer_append(
	RenderCommandBuffer *command_buffer,
	const RenderDrawItemRecord *draw_item_record,
	RenderRecordId *assigned_draw_item_id);

const RenderDrawItemRecord *render_command_buffer_get_record(
	const RenderCommandBuffer *command_buffer,
	uint32_t index);

#endif
