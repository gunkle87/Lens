#include "render_backend_boundary.h"

#include <stddef.h>

_Static_assert(offsetof(RenderBackendBoundary, max_primitive_batch_count) == 0U,
	"RenderBackendBoundary capacity must lead the boundary state.");
_Static_assert(offsetof(RenderBackendBoundaryPresentRecord, presented_batch_count) == 0U,
	"RenderBackendBoundaryPresentRecord count must lead the record.");

static void render_backend_boundary_write_invalid_record_id(
	RenderRecordId *record_id)
{
	record_id->handle = RENDER_INVALID_HANDLE;
	record_id->index = RENDER_INVALID_INDEX;
}

static void render_backend_boundary_reset_present_record(
	RenderBackendBoundaryPresentRecord *present_record)
{
	present_record->presented_batch_count = 0U;
	render_backend_boundary_write_invalid_record_id(
		&present_record->first_draw_item_id);
	render_backend_boundary_write_invalid_record_id(
		&present_record->last_draw_item_id);
	render_backend_boundary_write_invalid_record_id(
		&present_record->first_layer_order_id);
	render_backend_boundary_write_invalid_record_id(
		&present_record->last_layer_order_id);
	present_record->first_source_handle = 0U;
	present_record->last_source_handle = 0U;
}

static int render_backend_boundary_record_id_is_invalid(
	RenderRecordId record_id)
{
	return record_id.handle == RENDER_INVALID_HANDLE ||
		record_id.index == RENDER_INVALID_INDEX;
}

static RenderBackendBoundaryResult render_backend_boundary_validate_batch_record(
	const RenderPrimitiveBatchRecord *primitive_batch_record)
{
	if (render_backend_boundary_record_id_is_invalid(
			primitive_batch_record->draw_item_id) != 0)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_BATCH_REFERENCE;
	}

	if (render_backend_boundary_record_id_is_invalid(
			primitive_batch_record->transform_id) != 0)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_BATCH_REFERENCE;
	}

	if (render_backend_boundary_record_id_is_invalid(
			primitive_batch_record->clip_id) != 0)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_BATCH_REFERENCE;
	}

	if (render_backend_boundary_record_id_is_invalid(
			primitive_batch_record->layer_order_id) != 0)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_BATCH_REFERENCE;
	}

	return RENDER_BACKEND_BOUNDARY_RESULT_OK;
}

RenderBackendBoundaryResult render_backend_boundary_initialize(
	RenderBackendBoundary *boundary,
	uint32_t max_primitive_batch_count)
{
	if (boundary == NULL)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_ARGUMENT;
	}

	boundary->max_primitive_batch_count = max_primitive_batch_count;
	render_backend_boundary_reset(boundary);
	return RENDER_BACKEND_BOUNDARY_RESULT_OK;
}

void render_backend_boundary_reset(RenderBackendBoundary *boundary)
{
	if (boundary == NULL)
	{
		return;
	}

	boundary->state = RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_SUBMIT;
	render_backend_boundary_reset_present_record(
		&boundary->pending_present_record);
}

RenderBackendBoundaryResult render_backend_boundary_submit(
	RenderBackendBoundary *boundary,
	const RenderPrimitiveBatchRecord *primitive_batch_records,
	uint32_t primitive_batch_count)
{
	uint32_t batch_index;
	const RenderPrimitiveBatchRecord *first_primitive_batch_record;
	const RenderPrimitiveBatchRecord *last_primitive_batch_record;
	RenderBackendBoundaryResult result;

	if (boundary == NULL)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_ARGUMENT;
	}

	if ((primitive_batch_records == NULL) && (primitive_batch_count > 0U))
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_ARGUMENT;
	}

	if (boundary->state != RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_SUBMIT)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_STATE;
	}

	if (primitive_batch_count > boundary->max_primitive_batch_count)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_OUT_OF_BOUNDS;
	}

	render_backend_boundary_reset_present_record(
		&boundary->pending_present_record);

	for (batch_index = 0U; batch_index < primitive_batch_count; ++batch_index)
	{
		result = render_backend_boundary_validate_batch_record(
			&primitive_batch_records[batch_index]);
		if (result != RENDER_BACKEND_BOUNDARY_RESULT_OK)
		{
			return result;
		}
	}

	boundary->pending_present_record.presented_batch_count =
		primitive_batch_count;

	if (primitive_batch_count > 0U)
	{
		first_primitive_batch_record = &primitive_batch_records[0U];
		last_primitive_batch_record =
			&primitive_batch_records[primitive_batch_count - 1U];
		boundary->pending_present_record.first_draw_item_id =
			first_primitive_batch_record->draw_item_id;
		boundary->pending_present_record.last_draw_item_id =
			last_primitive_batch_record->draw_item_id;
		boundary->pending_present_record.first_layer_order_id =
			first_primitive_batch_record->layer_order_id;
		boundary->pending_present_record.last_layer_order_id =
			last_primitive_batch_record->layer_order_id;
		boundary->pending_present_record.first_source_handle =
			first_primitive_batch_record->source_handle;
		boundary->pending_present_record.last_source_handle =
			last_primitive_batch_record->source_handle;
	}

	boundary->state = RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_PRESENT;
	return RENDER_BACKEND_BOUNDARY_RESULT_OK;
}

RenderBackendBoundaryResult render_backend_boundary_present(
	RenderBackendBoundary *boundary,
	RenderBackendBoundaryPresentRecord *present_record)
{
	if (boundary == NULL || present_record == NULL)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_ARGUMENT;
	}

	if (boundary->state != RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_PRESENT)
	{
		return RENDER_BACKEND_BOUNDARY_RESULT_INVALID_STATE;
	}

	*present_record = boundary->pending_present_record;
	boundary->state = RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_SUBMIT;
	render_backend_boundary_reset_present_record(
		&boundary->pending_present_record);
	return RENDER_BACKEND_BOUNDARY_RESULT_OK;
}
