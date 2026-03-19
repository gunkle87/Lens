#ifndef RENDER_TRANSFORM_RESOLVER_H
#define RENDER_TRANSFORM_RESOLVER_H

#include <stdint.h>

#include "render_data_contracts.h"

typedef enum RenderTransformResolveResult
{
	RENDER_TRANSFORM_RESOLVE_RESULT_OK = 0,
	RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT = 1,
	RENDER_TRANSFORM_RESOLVE_RESULT_OUT_OF_RANGE_PARENT = 2,
	RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_PARENT_LINK = 3,
	RENDER_TRANSFORM_RESOLVE_RESULT_CYCLE_DETECTED = 4
} RenderTransformResolveResult;

typedef enum RenderTransformResolveState
{
	RENDER_TRANSFORM_RESOLVE_STATE_UNRESOLVED = 0,
	RENDER_TRANSFORM_RESOLVE_STATE_RESOLVING = 1,
	RENDER_TRANSFORM_RESOLVE_STATE_RESOLVED = 2
} RenderTransformResolveState;

typedef struct RenderResolvedTransformRecord
{
	RenderRecordId transform_id;
	RenderMatrix4x4 resolved_matrix;
	uint32_t resolved_depth;
	uint32_t source_transform_flags;
} RenderResolvedTransformRecord;

typedef struct RenderTransformResolver
{
	const RenderTransformRecord *input_records;
	RenderResolvedTransformRecord *resolved_records;
	RenderTransformResolveState *resolve_states;
	uint32_t count;
} RenderTransformResolver;

RenderTransformResolveResult render_transform_resolver_initialize(
	RenderTransformResolver *resolver,
	const RenderTransformRecord *input_records,
	RenderResolvedTransformRecord *resolved_records,
	RenderTransformResolveState *resolve_states,
	uint32_t count);

void render_transform_resolver_reset(RenderTransformResolver *resolver);

RenderTransformResolveResult render_transform_resolver_resolve_all(
	RenderTransformResolver *resolver);

const RenderResolvedTransformRecord *render_transform_resolver_get_resolved_record(
	const RenderTransformResolver *resolver,
	uint32_t index);

#endif
