#ifndef RENDER_CLIP_RESOLVER_H
#define RENDER_CLIP_RESOLVER_H

#include <stdint.h>

#include "render_data_contracts.h"

typedef enum RenderClipResolveResult
{
	RENDER_CLIP_RESOLVE_RESULT_OK = 0,
	RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT = 1,
	RENDER_CLIP_RESOLVE_RESULT_OUT_OF_RANGE_PARENT = 2,
	RENDER_CLIP_RESOLVE_RESULT_INVALID_PARENT_LINK = 3,
	RENDER_CLIP_RESOLVE_RESULT_CYCLE_DETECTED = 4
} RenderClipResolveResult;

typedef enum RenderClipResolveState
{
	RENDER_CLIP_RESOLVE_STATE_UNRESOLVED = 0,
	RENDER_CLIP_RESOLVE_STATE_RESOLVING = 1,
	RENDER_CLIP_RESOLVE_STATE_RESOLVED = 2
} RenderClipResolveState;

typedef struct RenderResolvedClipRecord
{
	RenderRecordId clip_id;
	RenderClipRect resolved_clip_rect;
	uint32_t resolved_depth;
	uint32_t source_clip_flags;
} RenderResolvedClipRecord;

typedef struct RenderClipResolver
{
	const RenderClipRecord *input_records;
	RenderResolvedClipRecord *resolved_records;
	RenderClipResolveState *resolve_states;
	uint32_t count;
} RenderClipResolver;

RenderClipResolveResult render_clip_resolver_initialize(
	RenderClipResolver *resolver,
	const RenderClipRecord *input_records,
	RenderResolvedClipRecord *resolved_records,
	RenderClipResolveState *resolve_states,
	uint32_t count);

void render_clip_resolver_reset(RenderClipResolver *resolver);

RenderClipResolveResult render_clip_resolver_resolve_all(
	RenderClipResolver *resolver);

const RenderResolvedClipRecord *render_clip_resolver_get_resolved_record(
	const RenderClipResolver *resolver,
	uint32_t index);

#endif
