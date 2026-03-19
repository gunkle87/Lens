#ifndef RENDER_BACKEND_BOUNDARY_H
#define RENDER_BACKEND_BOUNDARY_H

#include <stdint.h>

#include "render_primitive_emitter.h"

typedef enum RenderBackendBoundaryResult
{
	RENDER_BACKEND_BOUNDARY_RESULT_OK = 0,
	RENDER_BACKEND_BOUNDARY_RESULT_INVALID_ARGUMENT = 1,
	RENDER_BACKEND_BOUNDARY_RESULT_OUT_OF_BOUNDS = 2,
	RENDER_BACKEND_BOUNDARY_RESULT_INVALID_STATE = 3,
	RENDER_BACKEND_BOUNDARY_RESULT_INVALID_BATCH_REFERENCE = 4
} RenderBackendBoundaryResult;

typedef enum RenderBackendBoundaryState
{
	RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_SUBMIT = 0,
	RENDER_BACKEND_BOUNDARY_STATE_READY_FOR_PRESENT = 1
} RenderBackendBoundaryState;

typedef struct RenderBackendBoundaryPresentRecord
{
	uint32_t presented_batch_count;
	RenderRecordId first_draw_item_id;
	RenderRecordId last_draw_item_id;
	RenderRecordId first_layer_order_id;
	RenderRecordId last_layer_order_id;
	uint32_t first_source_handle;
	uint32_t last_source_handle;
} RenderBackendBoundaryPresentRecord;

typedef struct RenderBackendBoundary
{
	uint32_t max_primitive_batch_count;
	RenderBackendBoundaryState state;
	RenderBackendBoundaryPresentRecord pending_present_record;
} RenderBackendBoundary;

RenderBackendBoundaryResult render_backend_boundary_initialize(
	RenderBackendBoundary *boundary,
	uint32_t max_primitive_batch_count);

void render_backend_boundary_reset(RenderBackendBoundary *boundary);

RenderBackendBoundaryResult render_backend_boundary_submit(
	RenderBackendBoundary *boundary,
	const RenderPrimitiveBatchRecord *primitive_batch_records,
	uint32_t primitive_batch_count);

RenderBackendBoundaryResult render_backend_boundary_present(
	RenderBackendBoundary *boundary,
	RenderBackendBoundaryPresentRecord *present_record);

#endif
