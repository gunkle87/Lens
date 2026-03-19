#include "render_clip_resolver.h"

#include <float.h>
#include <stddef.h>

_Static_assert(offsetof(RenderResolvedClipRecord, clip_id) == 0U,
	"RenderResolvedClipRecord identity must lead the record.");
_Static_assert(offsetof(RenderClipResolver, input_records) == 0U,
	"RenderClipResolver inputs must lead the resolver state.");

static uint32_t render_clip_resolver_parent_is_root(
	const RenderRecordId *parent_clip_id)
{
	return (parent_clip_id->handle == RENDER_INVALID_HANDLE) &&
		(parent_clip_id->index == RENDER_INVALID_INDEX);
}

static void render_clip_resolver_write_default_clip_rect(
	RenderClipRect *clip_rect)
{
	clip_rect->min_x = -FLT_MAX;
	clip_rect->min_y = -FLT_MAX;
	clip_rect->max_x = FLT_MAX;
	clip_rect->max_y = FLT_MAX;
}

static void render_clip_resolver_intersect_clip_rects(
	const RenderClipRect *parent_clip_rect,
	const RenderClipRect *local_clip_rect,
	RenderClipRect *resolved_clip_rect)
{
	resolved_clip_rect->min_x =
		(parent_clip_rect->min_x > local_clip_rect->min_x) ?
			parent_clip_rect->min_x : local_clip_rect->min_x;
	resolved_clip_rect->min_y =
		(parent_clip_rect->min_y > local_clip_rect->min_y) ?
			parent_clip_rect->min_y : local_clip_rect->min_y;
	resolved_clip_rect->max_x =
		(parent_clip_rect->max_x < local_clip_rect->max_x) ?
			parent_clip_rect->max_x : local_clip_rect->max_x;
	resolved_clip_rect->max_y =
		(parent_clip_rect->max_y < local_clip_rect->max_y) ?
			parent_clip_rect->max_y : local_clip_rect->max_y;
}

static void render_clip_resolver_reset_slot(
	RenderClipResolver *resolver,
	uint32_t index)
{
	resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_UNRESOLVED;
	resolver->resolved_records[index].clip_id.handle = RENDER_INVALID_HANDLE;
	resolver->resolved_records[index].clip_id.index = RENDER_INVALID_INDEX;
	render_clip_resolver_write_default_clip_rect(
		&resolver->resolved_records[index].resolved_clip_rect);
	resolver->resolved_records[index].resolved_depth = 0U;
	resolver->resolved_records[index].source_clip_flags = 0U;
}

static RenderClipResolveResult render_clip_resolver_resolve_index(
	RenderClipResolver *resolver,
	uint32_t index)
{
	const RenderClipRecord *input_record;
	const RenderClipRecord *parent_record;
	const RenderResolvedClipRecord *parent_resolved_record;
	RenderResolvedClipRecord *resolved_record;
	RenderClipResolveResult result;
	uint32_t parent_index;

	if (resolver->resolve_states[index] == RENDER_CLIP_RESOLVE_STATE_RESOLVED)
	{
		return RENDER_CLIP_RESOLVE_RESULT_OK;
	}

	if (resolver->resolve_states[index] == RENDER_CLIP_RESOLVE_STATE_RESOLVING)
	{
		return RENDER_CLIP_RESOLVE_RESULT_CYCLE_DETECTED;
	}

	resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_RESOLVING;
	input_record = &resolver->input_records[index];
	resolved_record = &resolver->resolved_records[index];
	resolved_record->clip_id = input_record->clip_id;
	resolved_record->clip_id.index = index;
	resolved_record->resolved_depth = 0U;
	resolved_record->source_clip_flags = input_record->clip_flags;

	if (render_clip_resolver_parent_is_root(&input_record->parent_clip_id))
	{
		resolved_record->resolved_clip_rect = input_record->clip_rect;
		resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_RESOLVED;
		return RENDER_CLIP_RESOLVE_RESULT_OK;
	}

	if ((input_record->parent_clip_id.handle == RENDER_INVALID_HANDLE) ||
		(input_record->parent_clip_id.index == RENDER_INVALID_INDEX))
	{
		resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_UNRESOLVED;
		return RENDER_CLIP_RESOLVE_RESULT_INVALID_PARENT_LINK;
	}

	parent_index = input_record->parent_clip_id.index;
	if (parent_index >= resolver->count)
	{
		resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_UNRESOLVED;
		return RENDER_CLIP_RESOLVE_RESULT_OUT_OF_RANGE_PARENT;
	}

	parent_record = &resolver->input_records[parent_index];
	if ((parent_record->clip_id.handle != input_record->parent_clip_id.handle) ||
		(parent_record->clip_id.index != input_record->parent_clip_id.index))
	{
		resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_UNRESOLVED;
		return RENDER_CLIP_RESOLVE_RESULT_INVALID_PARENT_LINK;
	}

	result = render_clip_resolver_resolve_index(resolver, parent_index);
	if (result != RENDER_CLIP_RESOLVE_RESULT_OK)
	{
		resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_UNRESOLVED;
		return result;
	}

	parent_resolved_record = &resolver->resolved_records[parent_index];
	render_clip_resolver_intersect_clip_rects(
		&parent_resolved_record->resolved_clip_rect,
		&input_record->clip_rect,
		&resolved_record->resolved_clip_rect);
	resolved_record->resolved_depth =
		parent_resolved_record->resolved_depth + 1U;
	resolver->resolve_states[index] = RENDER_CLIP_RESOLVE_STATE_RESOLVED;
	return RENDER_CLIP_RESOLVE_RESULT_OK;
}

RenderClipResolveResult render_clip_resolver_initialize(
	RenderClipResolver *resolver,
	const RenderClipRecord *input_records,
	RenderResolvedClipRecord *resolved_records,
	RenderClipResolveState *resolve_states,
	uint32_t count)
{
	if (resolver == NULL)
	{
		return RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	if (((input_records == NULL) || (resolved_records == NULL) ||
		(resolve_states == NULL)) && (count > 0U))
	{
		return RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	resolver->input_records = input_records;
	resolver->resolved_records = resolved_records;
	resolver->resolve_states = resolve_states;
	resolver->count = count;
	render_clip_resolver_reset(resolver);
	return RENDER_CLIP_RESOLVE_RESULT_OK;
}

void render_clip_resolver_reset(RenderClipResolver *resolver)
{
	uint32_t input_index;

	if (resolver == NULL)
	{
		return;
	}

	for (input_index = 0U; input_index < resolver->count; ++input_index)
	{
		render_clip_resolver_reset_slot(resolver, input_index);
	}
}

RenderClipResolveResult render_clip_resolver_resolve_all(
	RenderClipResolver *resolver)
{
	RenderClipResolveResult result;
	uint32_t input_index;

	if (resolver == NULL)
	{
		return RENDER_CLIP_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	for (input_index = 0U; input_index < resolver->count; ++input_index)
	{
		result = render_clip_resolver_resolve_index(resolver, input_index);
		if (result != RENDER_CLIP_RESOLVE_RESULT_OK)
		{
			return result;
		}
	}

	return RENDER_CLIP_RESOLVE_RESULT_OK;
}

const RenderResolvedClipRecord *render_clip_resolver_get_resolved_record(
	const RenderClipResolver *resolver,
	uint32_t index)
{
	if (resolver == NULL)
	{
		return NULL;
	}

	if (index >= resolver->count)
	{
		return NULL;
	}

	if (resolver->resolve_states[index] != RENDER_CLIP_RESOLVE_STATE_RESOLVED)
	{
		return NULL;
	}

	return &resolver->resolved_records[index];
}
