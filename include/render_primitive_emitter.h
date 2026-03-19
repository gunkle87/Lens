#ifndef RENDER_PRIMITIVE_EMITTER_H
#define RENDER_PRIMITIVE_EMITTER_H

#include <stdint.h>

#include "render_data_contracts.h"
#include "render_transform_resolver.h"
#include "render_clip_resolver.h"
#include "render_ordering_resolver.h"

typedef enum RenderPrimitiveEmitResult
{
	RENDER_PRIMITIVE_EMIT_RESULT_OK = 0,
	RENDER_PRIMITIVE_EMIT_RESULT_INVALID_ARGUMENT = 1,
	RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE = 2,
	RENDER_PRIMITIVE_EMIT_RESULT_MISSING_RESOLVED_INPUT = 3
} RenderPrimitiveEmitResult;

typedef struct RenderPrimitiveBatchRecord
{
	RenderRecordId draw_item_id;
	RenderRecordId transform_id;
	RenderRecordId clip_id;
	RenderRecordId layer_order_id;
	RenderMatrix4x4 resolved_transform_matrix;
	RenderClipRect resolved_clip_rect;
	int32_t layer_index;
	int32_t order_key;
	uint32_t submission_index;
	uint32_t source_order_input_index;
	uint32_t source_handle;
	uint32_t draw_kind;
	uint32_t draw_flags;
	uint32_t source_transform_flags;
	uint32_t source_clip_flags;
	uint32_t source_order_flags;
} RenderPrimitiveBatchRecord;

typedef struct RenderPrimitiveEmitter
{
	const RenderDrawItemRecord *draw_item_records;
	const RenderResolvedTransformRecord *resolved_transform_records;
	const RenderResolvedClipRecord *resolved_clip_records;
	const RenderResolvedOrderRecord *resolved_order_records;
	RenderPrimitiveBatchRecord *primitive_batch_records;
	uint32_t draw_item_count;
	uint32_t transform_count;
	uint32_t clip_count;
	uint32_t order_count;
} RenderPrimitiveEmitter;

RenderPrimitiveEmitResult render_primitive_emitter_initialize(
	RenderPrimitiveEmitter *emitter,
	const RenderDrawItemRecord *draw_item_records,
	uint32_t draw_item_count,
	const RenderResolvedTransformRecord *resolved_transform_records,
	uint32_t transform_count,
	const RenderResolvedClipRecord *resolved_clip_records,
	uint32_t clip_count,
	const RenderResolvedOrderRecord *resolved_order_records,
	uint32_t order_count,
	RenderPrimitiveBatchRecord *primitive_batch_records);

void render_primitive_emitter_reset(RenderPrimitiveEmitter *emitter);

RenderPrimitiveEmitResult render_primitive_emitter_emit_all(
	RenderPrimitiveEmitter *emitter);

const RenderPrimitiveBatchRecord *render_primitive_emitter_get_batch_record(
	const RenderPrimitiveEmitter *emitter,
	uint32_t index);

#endif
