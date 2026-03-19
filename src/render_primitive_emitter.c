#include "render_primitive_emitter.h"

#include <stddef.h>

_Static_assert(offsetof(RenderPrimitiveBatchRecord, draw_item_id) == 0U,
	"RenderPrimitiveBatchRecord identity must lead the record.");
_Static_assert(offsetof(RenderPrimitiveEmitter, draw_item_records) == 0U,
	"RenderPrimitiveEmitter inputs must lead the emitter state.");

static void render_primitive_emitter_write_identity_matrix(
	RenderMatrix4x4 *matrix)
{
	uint32_t element_index;

	for (element_index = 0U;
		element_index < RENDER_MATRIX_4X4_ELEMENT_COUNT;
		++element_index)
	{
		matrix->elements[element_index] = 0.0f;
	}

	matrix->elements[0U] = 1.0f;
	matrix->elements[5U] = 1.0f;
	matrix->elements[10U] = 1.0f;
	matrix->elements[15U] = 1.0f;
}

static void render_primitive_emitter_reset_slot(
	RenderPrimitiveBatchRecord *primitive_batch_record)
{
	primitive_batch_record->draw_item_id.handle = RENDER_INVALID_HANDLE;
	primitive_batch_record->draw_item_id.index = RENDER_INVALID_INDEX;
	primitive_batch_record->transform_id.handle = RENDER_INVALID_HANDLE;
	primitive_batch_record->transform_id.index = RENDER_INVALID_INDEX;
	primitive_batch_record->clip_id.handle = RENDER_INVALID_HANDLE;
	primitive_batch_record->clip_id.index = RENDER_INVALID_INDEX;
	primitive_batch_record->layer_order_id.handle = RENDER_INVALID_HANDLE;
	primitive_batch_record->layer_order_id.index = RENDER_INVALID_INDEX;
	render_primitive_emitter_write_identity_matrix(
		&primitive_batch_record->resolved_transform_matrix);
	primitive_batch_record->resolved_clip_rect.min_x = 0.0f;
	primitive_batch_record->resolved_clip_rect.min_y = 0.0f;
	primitive_batch_record->resolved_clip_rect.max_x = 0.0f;
	primitive_batch_record->resolved_clip_rect.max_y = 0.0f;
	primitive_batch_record->layer_index = 0;
	primitive_batch_record->order_key = 0;
	primitive_batch_record->submission_index = 0U;
	primitive_batch_record->source_order_input_index = RENDER_INVALID_INDEX;
	primitive_batch_record->source_handle = 0U;
	primitive_batch_record->draw_kind = 0U;
	primitive_batch_record->draw_flags = 0U;
	primitive_batch_record->source_transform_flags = 0U;
	primitive_batch_record->source_clip_flags = 0U;
	primitive_batch_record->source_order_flags = 0U;
}

static int render_primitive_emitter_record_id_is_invalid(
	RenderRecordId record_id)
{
	return record_id.handle == RENDER_INVALID_HANDLE ||
		record_id.index == RENDER_INVALID_INDEX;
}

static int render_primitive_emitter_record_id_matches(
	RenderRecordId left_record_id,
	RenderRecordId right_record_id)
{
	return left_record_id.handle == right_record_id.handle &&
		left_record_id.index == right_record_id.index;
}

static RenderPrimitiveEmitResult render_primitive_emitter_validate_order_record(
	const RenderPrimitiveEmitter *emitter,
	const RenderResolvedOrderRecord *resolved_order_record,
	const RenderDrawItemRecord **draw_item_record_out)
{
	uint32_t draw_item_index;
	const RenderDrawItemRecord *draw_item_record;

	if (render_primitive_emitter_record_id_is_invalid(
			resolved_order_record->layer_order_id) != 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	if (render_primitive_emitter_record_id_is_invalid(
			resolved_order_record->draw_item_id) != 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	draw_item_index = resolved_order_record->draw_item_id.index;
	if (draw_item_index >= emitter->draw_item_count)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	draw_item_record = &emitter->draw_item_records[draw_item_index];
	if (render_primitive_emitter_record_id_matches(
			draw_item_record->draw_item_id,
			resolved_order_record->draw_item_id) == 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	if (render_primitive_emitter_record_id_matches(
			draw_item_record->layer_order_id,
			resolved_order_record->layer_order_id) == 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	*draw_item_record_out = draw_item_record;
	return RENDER_PRIMITIVE_EMIT_RESULT_OK;
}

static RenderPrimitiveEmitResult render_primitive_emitter_get_transform_record(
	const RenderPrimitiveEmitter *emitter,
	RenderRecordId transform_id,
	const RenderResolvedTransformRecord **resolved_transform_record_out)
{
	const RenderResolvedTransformRecord *resolved_transform_record;

	if (render_primitive_emitter_record_id_is_invalid(transform_id) != 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	if (transform_id.index >= emitter->transform_count)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_MISSING_RESOLVED_INPUT;
	}

	resolved_transform_record =
		&emitter->resolved_transform_records[transform_id.index];
	if (render_primitive_emitter_record_id_matches(
			resolved_transform_record->transform_id,
			transform_id) == 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_MISSING_RESOLVED_INPUT;
	}

	*resolved_transform_record_out = resolved_transform_record;
	return RENDER_PRIMITIVE_EMIT_RESULT_OK;
}

static RenderPrimitiveEmitResult render_primitive_emitter_get_clip_record(
	const RenderPrimitiveEmitter *emitter,
	RenderRecordId clip_id,
	const RenderResolvedClipRecord **resolved_clip_record_out)
{
	const RenderResolvedClipRecord *resolved_clip_record;

	if (render_primitive_emitter_record_id_is_invalid(clip_id) != 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_INPUT_REFERENCE;
	}

	if (clip_id.index >= emitter->clip_count)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_MISSING_RESOLVED_INPUT;
	}

	resolved_clip_record = &emitter->resolved_clip_records[clip_id.index];
	if (render_primitive_emitter_record_id_matches(
			resolved_clip_record->clip_id,
			clip_id) == 0)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_MISSING_RESOLVED_INPUT;
	}

	*resolved_clip_record_out = resolved_clip_record;
	return RENDER_PRIMITIVE_EMIT_RESULT_OK;
}

static void render_primitive_emitter_write_batch_record(
	RenderPrimitiveBatchRecord *primitive_batch_record,
	const RenderDrawItemRecord *draw_item_record,
	const RenderResolvedTransformRecord *resolved_transform_record,
	const RenderResolvedClipRecord *resolved_clip_record,
	const RenderResolvedOrderRecord *resolved_order_record)
{
	primitive_batch_record->draw_item_id = draw_item_record->draw_item_id;
	primitive_batch_record->transform_id = draw_item_record->transform_id;
	primitive_batch_record->clip_id = draw_item_record->clip_id;
	primitive_batch_record->layer_order_id = draw_item_record->layer_order_id;
	primitive_batch_record->resolved_transform_matrix =
		resolved_transform_record->resolved_matrix;
	primitive_batch_record->resolved_clip_rect =
		resolved_clip_record->resolved_clip_rect;
	primitive_batch_record->layer_index = resolved_order_record->layer_index;
	primitive_batch_record->order_key = resolved_order_record->order_key;
	primitive_batch_record->submission_index =
		resolved_order_record->submission_index;
	primitive_batch_record->source_order_input_index =
		resolved_order_record->source_input_index;
	primitive_batch_record->source_handle = draw_item_record->source_handle;
	primitive_batch_record->draw_kind = draw_item_record->draw_kind;
	primitive_batch_record->draw_flags = draw_item_record->draw_flags;
	primitive_batch_record->source_transform_flags =
		resolved_transform_record->source_transform_flags;
	primitive_batch_record->source_clip_flags =
		resolved_clip_record->source_clip_flags;
	primitive_batch_record->source_order_flags =
		resolved_order_record->source_order_flags;
}

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
	RenderPrimitiveBatchRecord *primitive_batch_records)
{
	if (emitter == NULL)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_ARGUMENT;
	}

	if (((draw_item_records == NULL) && (draw_item_count > 0U)) ||
		((resolved_transform_records == NULL) && (transform_count > 0U)) ||
		((resolved_clip_records == NULL) && (clip_count > 0U)) ||
		((resolved_order_records == NULL) && (order_count > 0U)) ||
		((primitive_batch_records == NULL) && (order_count > 0U)))
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_ARGUMENT;
	}

	emitter->draw_item_records = draw_item_records;
	emitter->resolved_transform_records = resolved_transform_records;
	emitter->resolved_clip_records = resolved_clip_records;
	emitter->resolved_order_records = resolved_order_records;
	emitter->primitive_batch_records = primitive_batch_records;
	emitter->draw_item_count = draw_item_count;
	emitter->transform_count = transform_count;
	emitter->clip_count = clip_count;
	emitter->order_count = order_count;
	render_primitive_emitter_reset(emitter);
	return RENDER_PRIMITIVE_EMIT_RESULT_OK;
}

void render_primitive_emitter_reset(RenderPrimitiveEmitter *emitter)
{
	uint32_t order_index;

	if (emitter == NULL || emitter->primitive_batch_records == NULL)
	{
		return;
	}

	for (order_index = 0U; order_index < emitter->order_count; ++order_index)
	{
		render_primitive_emitter_reset_slot(
			&emitter->primitive_batch_records[order_index]);
	}
}

RenderPrimitiveEmitResult render_primitive_emitter_emit_all(
	RenderPrimitiveEmitter *emitter)
{
	uint32_t order_index;

	if (emitter == NULL)
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_ARGUMENT;
	}

	if ((emitter->order_count > 0U) &&
		((emitter->resolved_order_records == NULL) ||
		(emitter->primitive_batch_records == NULL)))
	{
		return RENDER_PRIMITIVE_EMIT_RESULT_INVALID_ARGUMENT;
	}

	render_primitive_emitter_reset(emitter);

	for (order_index = 0U; order_index < emitter->order_count; ++order_index)
	{
		const RenderResolvedOrderRecord *resolved_order_record;
		const RenderDrawItemRecord *draw_item_record;
		const RenderResolvedTransformRecord *resolved_transform_record;
		const RenderResolvedClipRecord *resolved_clip_record;
		RenderPrimitiveEmitResult result;

		resolved_order_record = &emitter->resolved_order_records[order_index];
		result = render_primitive_emitter_validate_order_record(
			emitter,
			resolved_order_record,
			&draw_item_record);
		if (result != RENDER_PRIMITIVE_EMIT_RESULT_OK)
		{
			return result;
		}

		result = render_primitive_emitter_get_transform_record(
			emitter,
			draw_item_record->transform_id,
			&resolved_transform_record);
		if (result != RENDER_PRIMITIVE_EMIT_RESULT_OK)
		{
			return result;
		}

		result = render_primitive_emitter_get_clip_record(
			emitter,
			draw_item_record->clip_id,
			&resolved_clip_record);
		if (result != RENDER_PRIMITIVE_EMIT_RESULT_OK)
		{
			return result;
		}

		render_primitive_emitter_write_batch_record(
			&emitter->primitive_batch_records[order_index],
			draw_item_record,
			resolved_transform_record,
			resolved_clip_record,
			resolved_order_record);
	}

	return RENDER_PRIMITIVE_EMIT_RESULT_OK;
}

const RenderPrimitiveBatchRecord *render_primitive_emitter_get_batch_record(
	const RenderPrimitiveEmitter *emitter,
	uint32_t index)
{
	if (emitter == NULL || emitter->primitive_batch_records == NULL)
	{
		return NULL;
	}

	if (index >= emitter->order_count)
	{
		return NULL;
	}

	return &emitter->primitive_batch_records[index];
}
