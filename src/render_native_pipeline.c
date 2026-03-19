#include "render_native_pipeline.h"

#include <stddef.h>

_Static_assert(offsetof(RenderNativePipelineExecutionRecord, failed_stage) == 0U,
	"RenderNativePipelineExecutionRecord status must lead the record.");
_Static_assert(offsetof(RenderNativePipeline, command_buffer) == 0U,
	"RenderNativePipeline command buffer must lead the pipeline state.");

static void render_native_pipeline_write_invalid_record_id(
	RenderRecordId *record_id)
{
	record_id->handle = RENDER_INVALID_HANDLE;
	record_id->index = RENDER_INVALID_INDEX;
}

static void render_native_pipeline_reset_execution_record(
	RenderNativePipelineExecutionRecord *execution_record)
{
	execution_record->failed_stage = RENDER_NATIVE_PIPELINE_STAGE_NONE;
	execution_record->stage_result_code = 0;
	execution_record->ingested_draw_item_count = 0U;
	execution_record->emitted_primitive_count = 0U;
	execution_record->present_record.presented_batch_count = 0U;
	render_native_pipeline_write_invalid_record_id(
		&execution_record->present_record.first_draw_item_id);
	render_native_pipeline_write_invalid_record_id(
		&execution_record->present_record.last_draw_item_id);
	render_native_pipeline_write_invalid_record_id(
		&execution_record->present_record.first_layer_order_id);
	render_native_pipeline_write_invalid_record_id(
		&execution_record->present_record.last_layer_order_id);
	execution_record->present_record.first_source_handle = 0U;
	execution_record->present_record.last_source_handle = 0U;
}

static void render_native_pipeline_initialize_safe_state(
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
	uint32_t max_backend_batch_count)
{
	pipeline->command_buffer.draw_item_records = draw_item_records;
	pipeline->command_buffer.capacity = max_draw_item_count;
	pipeline->command_buffer.count = 0U;

	pipeline->transform_resolver.input_records = NULL;
	pipeline->transform_resolver.resolved_records = resolved_transform_records;
	pipeline->transform_resolver.resolve_states = transform_resolve_states;
	pipeline->transform_resolver.count = 0U;

	pipeline->clip_resolver.input_records = NULL;
	pipeline->clip_resolver.resolved_records = resolved_clip_records;
	pipeline->clip_resolver.resolve_states = clip_resolve_states;
	pipeline->clip_resolver.count = 0U;

	pipeline->ordering_resolver.input_records = NULL;
	pipeline->ordering_resolver.resolved_records = resolved_order_records;
	pipeline->ordering_resolver.count = 0U;

	pipeline->primitive_emitter.draw_item_records = NULL;
	pipeline->primitive_emitter.resolved_transform_records =
		resolved_transform_records;
	pipeline->primitive_emitter.resolved_clip_records = resolved_clip_records;
	pipeline->primitive_emitter.resolved_order_records = resolved_order_records;
	pipeline->primitive_emitter.primitive_batch_records =
		primitive_batch_records;
	pipeline->primitive_emitter.draw_item_count = 0U;
	pipeline->primitive_emitter.transform_count = 0U;
	pipeline->primitive_emitter.clip_count = 0U;
	pipeline->primitive_emitter.order_count = 0U;

	pipeline->backend_boundary.max_primitive_batch_count =
		max_backend_batch_count;
	pipeline->backend_boundary.state =
		RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_SUBMIT;
	pipeline->backend_boundary.pending_present_record.presented_batch_count = 0U;
	render_native_pipeline_write_invalid_record_id(
		&pipeline->backend_boundary.pending_present_record.first_draw_item_id);
	render_native_pipeline_write_invalid_record_id(
		&pipeline->backend_boundary.pending_present_record.last_draw_item_id);
	render_native_pipeline_write_invalid_record_id(
		&pipeline->backend_boundary.pending_present_record.first_layer_order_id);
	render_native_pipeline_write_invalid_record_id(
		&pipeline->backend_boundary.pending_present_record.last_layer_order_id);
	pipeline->backend_boundary.pending_present_record.first_source_handle = 0U;
	pipeline->backend_boundary.pending_present_record.last_source_handle = 0U;

	pipeline->resolved_transform_records = resolved_transform_records;
	pipeline->transform_resolve_states = transform_resolve_states;
	pipeline->max_transform_count = max_transform_count;
	pipeline->resolved_clip_records = resolved_clip_records;
	pipeline->clip_resolve_states = clip_resolve_states;
	pipeline->max_clip_count = max_clip_count;
	pipeline->resolved_order_records = resolved_order_records;
	pipeline->max_order_count = max_order_count;
	pipeline->primitive_batch_records = primitive_batch_records;
	pipeline->max_primitive_batch_count = max_primitive_batch_count;
}

static RenderNativePipelineResult render_native_pipeline_fail_capacity(
	RenderNativePipelineExecutionRecord *execution_record,
	RenderNativePipelineStage failed_stage)
{
	execution_record->failed_stage = failed_stage;
	execution_record->stage_result_code = 0;
	return RENDER_NATIVE_PIPELINE_RESULT_OUT_OF_CAPACITY;
}

static RenderNativePipelineResult render_native_pipeline_fail_stage(
	RenderNativePipelineExecutionRecord *execution_record,
	RenderNativePipelineStage failed_stage,
	int32_t stage_result_code)
{
	execution_record->failed_stage = failed_stage;
	execution_record->stage_result_code = stage_result_code;
	return RENDER_NATIVE_PIPELINE_RESULT_STAGE_FAILURE;
}

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
	uint32_t max_backend_batch_count)
{
	RenderCommandBufferResult command_buffer_result;
	RenderBackendBoundaryResult backend_boundary_result;

	if (pipeline == NULL)
	{
		return RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT;
	}

	if (((draw_item_records == NULL) && (max_draw_item_count > 0U)) ||
		((resolved_transform_records == NULL) && (max_transform_count > 0U)) ||
		((transform_resolve_states == NULL) && (max_transform_count > 0U)) ||
		((resolved_clip_records == NULL) && (max_clip_count > 0U)) ||
		((clip_resolve_states == NULL) && (max_clip_count > 0U)) ||
		((resolved_order_records == NULL) && (max_order_count > 0U)) ||
		((primitive_batch_records == NULL) && (max_primitive_batch_count > 0U)))
	{
		return RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT;
	}

	render_native_pipeline_initialize_safe_state(
		pipeline,
		draw_item_records,
		max_draw_item_count,
		resolved_transform_records,
		transform_resolve_states,
		max_transform_count,
		resolved_clip_records,
		clip_resolve_states,
		max_clip_count,
		resolved_order_records,
		max_order_count,
		primitive_batch_records,
		max_primitive_batch_count,
		max_backend_batch_count);

	command_buffer_result = render_command_buffer_initialize(
		&pipeline->command_buffer,
		draw_item_records,
		max_draw_item_count);
	if (command_buffer_result != RENDER_COMMAND_BUFFER_RESULT_OK)
	{
		return RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT;
	}

	backend_boundary_result = render_backend_boundary_initialize(
		&pipeline->backend_boundary,
		max_backend_batch_count);
	if (backend_boundary_result != RENDER_BACKEND_BOUNDARY_RESULT_OK)
	{
		return RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT;
	}

	render_native_pipeline_reset(pipeline);
	return RENDER_NATIVE_PIPELINE_RESULT_OK;
}

void render_native_pipeline_reset(RenderNativePipeline *pipeline)
{
	if (pipeline == NULL)
	{
		return;
	}

	render_command_buffer_reset(&pipeline->command_buffer);
	render_transform_resolver_reset(&pipeline->transform_resolver);
	render_clip_resolver_reset(&pipeline->clip_resolver);
	render_ordering_resolver_reset(&pipeline->ordering_resolver);
	render_primitive_emitter_reset(&pipeline->primitive_emitter);
	render_backend_boundary_reset(&pipeline->backend_boundary);
}

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
	RenderNativePipelineExecutionRecord *execution_record)
{
	uint32_t draw_item_index;
	RenderCommandBufferResult command_buffer_result;
	RenderTransformResolveResult transform_result;
	RenderClipResolveResult clip_result;
	RenderOrderingResolveResult ordering_result;
	RenderPrimitiveEmitResult primitive_emit_result;
	RenderBackendBoundaryResult backend_boundary_result;

	if ((pipeline == NULL) || (execution_record == NULL))
	{
		return RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT;
	}

	if (((draw_item_records == NULL) && (draw_item_count > 0U)) ||
		((transform_records == NULL) && (transform_count > 0U)) ||
		((clip_records == NULL) && (clip_count > 0U)) ||
		((layer_order_records == NULL) && (layer_order_count > 0U)))
	{
		return RENDER_NATIVE_PIPELINE_RESULT_INVALID_ARGUMENT;
	}

	render_native_pipeline_reset_execution_record(execution_record);

	if (draw_item_count > pipeline->command_buffer.capacity)
	{
		return render_native_pipeline_fail_capacity(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_COMMAND_INPUT);
	}

	if (transform_count > pipeline->max_transform_count)
	{
		return render_native_pipeline_fail_capacity(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_TRANSFORM_RESOLVE);
	}

	if (clip_count > pipeline->max_clip_count)
	{
		return render_native_pipeline_fail_capacity(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_CLIP_RESOLVE);
	}

	if (layer_order_count > pipeline->max_order_count)
	{
		return render_native_pipeline_fail_capacity(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_ORDERING_RESOLVE);
	}

	if (layer_order_count > pipeline->max_primitive_batch_count)
	{
		return render_native_pipeline_fail_capacity(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_PRIMITIVE_EMIT);
	}

	if (layer_order_count > pipeline->backend_boundary.max_primitive_batch_count)
	{
		return render_native_pipeline_fail_capacity(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_BACKEND_SUBMIT);
	}

	render_native_pipeline_reset(pipeline);

	for (draw_item_index = 0U; draw_item_index < draw_item_count; ++draw_item_index)
	{
		command_buffer_result = render_command_buffer_append(
			&pipeline->command_buffer,
			&draw_item_records[draw_item_index],
			NULL);
		if (command_buffer_result != RENDER_COMMAND_BUFFER_RESULT_OK)
		{
			return render_native_pipeline_fail_stage(
				execution_record,
				RENDER_NATIVE_PIPELINE_STAGE_COMMAND_INPUT,
				(int32_t)command_buffer_result);
		}
	}

	transform_result = render_transform_resolver_initialize(
		&pipeline->transform_resolver,
		transform_records,
		pipeline->resolved_transform_records,
		pipeline->transform_resolve_states,
		transform_count);
	if (transform_result != RENDER_TRANSFORM_RESOLVE_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_TRANSFORM_RESOLVE,
			(int32_t)transform_result);
	}

	transform_result = render_transform_resolver_resolve_all(
		&pipeline->transform_resolver);
	if (transform_result != RENDER_TRANSFORM_RESOLVE_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_TRANSFORM_RESOLVE,
			(int32_t)transform_result);
	}

	clip_result = render_clip_resolver_initialize(
		&pipeline->clip_resolver,
		clip_records,
		pipeline->resolved_clip_records,
		pipeline->clip_resolve_states,
		clip_count);
	if (clip_result != RENDER_CLIP_RESOLVE_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_CLIP_RESOLVE,
			(int32_t)clip_result);
	}

	clip_result = render_clip_resolver_resolve_all(&pipeline->clip_resolver);
	if (clip_result != RENDER_CLIP_RESOLVE_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_CLIP_RESOLVE,
			(int32_t)clip_result);
	}

	ordering_result = render_ordering_resolver_initialize(
		&pipeline->ordering_resolver,
		layer_order_records,
		pipeline->resolved_order_records,
		layer_order_count);
	if (ordering_result != RENDER_ORDERING_RESOLVE_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_ORDERING_RESOLVE,
			(int32_t)ordering_result);
	}

	ordering_result = render_ordering_resolver_resolve_all(
		&pipeline->ordering_resolver);
	if (ordering_result != RENDER_ORDERING_RESOLVE_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_ORDERING_RESOLVE,
			(int32_t)ordering_result);
	}

	primitive_emit_result = render_primitive_emitter_initialize(
		&pipeline->primitive_emitter,
		pipeline->command_buffer.draw_item_records,
		pipeline->command_buffer.count,
		pipeline->resolved_transform_records,
		transform_count,
		pipeline->resolved_clip_records,
		clip_count,
		pipeline->resolved_order_records,
		layer_order_count,
		pipeline->primitive_batch_records);
	if (primitive_emit_result != RENDER_PRIMITIVE_EMIT_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_PRIMITIVE_EMIT,
			(int32_t)primitive_emit_result);
	}

	primitive_emit_result = render_primitive_emitter_emit_all(
		&pipeline->primitive_emitter);
	if (primitive_emit_result != RENDER_PRIMITIVE_EMIT_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_PRIMITIVE_EMIT,
			(int32_t)primitive_emit_result);
	}

	render_backend_boundary_reset(&pipeline->backend_boundary);
	backend_boundary_result = render_backend_boundary_submit(
		&pipeline->backend_boundary,
		pipeline->primitive_batch_records,
		layer_order_count);
	if (backend_boundary_result != RENDER_BACKEND_BOUNDARY_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_BACKEND_SUBMIT,
			(int32_t)backend_boundary_result);
	}

	backend_boundary_result = render_backend_boundary_present(
		&pipeline->backend_boundary,
		&execution_record->present_record);
	if (backend_boundary_result != RENDER_BACKEND_BOUNDARY_RESULT_OK)
	{
		return render_native_pipeline_fail_stage(
			execution_record,
			RENDER_NATIVE_PIPELINE_STAGE_BACKEND_PRESENT,
			(int32_t)backend_boundary_result);
	}

	execution_record->ingested_draw_item_count = pipeline->command_buffer.count;
	execution_record->emitted_primitive_count = layer_order_count;
	return RENDER_NATIVE_PIPELINE_RESULT_OK;
}
