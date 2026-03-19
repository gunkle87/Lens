#ifndef RENDER_NATIVE_PIPELINE_H
#define RENDER_NATIVE_PIPELINE_H

#include <stdint.h>

#include "render_command_buffer.h"
#include "render_transform_resolver.h"
#include "render_clip_resolver.h"
#include "render_ordering_resolver.h"
#include "render_primitive_emitter.h"
#include "render_backend_boundary.h"

typedef enum RenderNativePipelineResult
{
	RENDER_NATIVE_PIPELINE_RESULT_OK = 0,
	RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT = 1,
	RENDER_NATIVE_PIPELINE_RESULT_OUT_OF_CAPACITY = 2,
	RENDER_NATIVE_PIPELINE_RESULT_STAGE_FAILURE = 3
} RenderNativePipelineResult;

typedef enum RenderNativePipelineStage
{
	RENDER_NATIVE_PIPELINE_STAGE_NONE = 0,
	RENDER_NATIVE_PIPELINE_STAGE_COMMAND_INPUT = 1,
	RENDER_NATIVE_PIPELINE_STAGE_TRANSFORM_RESOLVE = 2,
	RENDER_NATIVE_PIPELINE_STAGE_CLIP_RESOLVE = 3,
	RENDER_NATIVE_PIPELINE_STAGE_ORDERING_RESOLVE = 4,
	RENDER_NATIVE_PIPELINE_STAGE_PRIMITIVE_EMIT = 5,
	RENDER_NATIVE_PIPELINE_STAGE_BACKEND_SUBMIT = 6,
	RENDER_NATIVE_PIPELINE_STAGE_BACKEND_PRESENT = 7
} RenderNativePipelineStage;

typedef struct RenderNativePipelineExecutionRecord
{
	RenderNativePipelineStage failed_stage;
	int32_t stage_result_code;
	uint32_t ingested_draw_item_count;
	uint32_t emitted_primitive_count;
	RenderBackendBoundaryPresentRecord present_record;
} RenderNativePipelineExecutionRecord;

typedef struct RenderNativePipeline
{
	RenderCommandBuffer command_buffer;
	RenderTransformResolver transform_resolver;
	RenderClipResolver clip_resolver;
	RenderOrderingResolver ordering_resolver;
	RenderPrimitiveEmitter primitive_emitter;
	RenderBackendBoundary backend_boundary;
	RenderResolvedTransformRecord *resolved_transform_records;
	RenderTransformResolveState *transform_resolve_states;
	uint32_t max_transform_count;
	RenderResolvedClipRecord *resolved_clip_records;
	RenderClipResolveState *clip_resolve_states;
	uint32_t max_clip_count;
	RenderResolvedOrderRecord *resolved_order_records;
	uint32_t max_order_count;
	RenderPrimitiveBatchRecord *primitive_batch_records;
	uint32_t max_primitive_batch_count;
} RenderNativePipeline;

RenderNativePipelineResult render_native_pipeline_initialize(
	RenderNativePipeline *pipeline,
	RenderDrawItemRecord *draw_item_records,
	uint32_t max_draw_item_count,
	RenderResolvedTransformRecord *resolved_transform_records,
	RenderTransformResolveState *transform_resolve_states,
	uint32_t max_transform_count,
	RenderResolvedClipRecord *resolved_clip_records,
	RenderClipResolveState *clip_resolve_states,
	uint32_t max_clip_count,
	RenderResolvedOrderRecord *resolved_order_records,
	uint32_t max_order_count,
	RenderPrimitiveBatchRecord *primitive_batch_records,
	uint32_t max_primitive_batch_count,
	uint32_t max_backend_batch_count);

void render_native_pipeline_reset(RenderNativePipeline *pipeline);

RenderNativePipelineResult render_native_pipeline_execute(
	RenderNativePipeline *pipeline,
	const RenderDrawItemRecord *draw_item_records,
	uint32_t draw_item_count,
	const RenderTransformRecord *transform_records,
	uint32_t transform_count,
	const RenderClipRecord *clip_records,
	uint32_t clip_count,
	const RenderLayerOrderInputRecord *layer_order_records,
	uint32_t layer_order_count,
	RenderNativePipelineExecutionRecord *execution_record);

#endif
