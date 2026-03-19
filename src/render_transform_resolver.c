#include "render_transform_resolver.h"

#include <stddef.h>

_Static_assert(offsetof(RenderResolvedTransformRecord, transform_id) == 0U,
	"RenderResolvedTransformRecord identity must lead the record.");
_Static_assert(offsetof(RenderTransformResolver, input_records) == 0U,
	"RenderTransformResolver inputs must lead the resolver state.");

static uint32_t render_transform_resolver_parent_is_root(
	const RenderRecordId *parent_transform_id)
{
	return (parent_transform_id->handle == RENDER_INVALID_HANDLE) &&
		(parent_transform_id->index == RENDER_INVALID_INDEX);
}

static void render_transform_resolver_write_identity_matrix(
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

static void render_transform_resolver_multiply_matrices(
	const RenderMatrix4x4 *left_matrix,
	const RenderMatrix4x4 *right_matrix,
	RenderMatrix4x4 *output_matrix)
{
	uint32_t row_index;
	uint32_t column_index;
	uint32_t element_index;

	for (row_index = 0U; row_index < 4U; ++row_index)
	{
		for (column_index = 0U; column_index < 4U; ++column_index)
		{
			float element_value = 0.0f;

			for (element_index = 0U; element_index < 4U; ++element_index)
			{
				element_value +=
					left_matrix->elements[(row_index * 4U) + element_index] *
					right_matrix->elements[(element_index * 4U) + column_index];
			}

			output_matrix->elements[(row_index * 4U) + column_index] =
				element_value;
		}
	}
}

static void render_transform_resolver_reset_slot(
	RenderTransformResolver *resolver,
	uint32_t index)
{
	resolver->resolve_states[index] =
		RENDER_TRANSFORM_RESOLVE_STATE_UNRESOLVED;
	resolver->resolved_records[index].transform_id.handle =
		RENDER_INVALID_HANDLE;
	resolver->resolved_records[index].transform_id.index =
		RENDER_INVALID_INDEX;
	render_transform_resolver_write_identity_matrix(
		&resolver->resolved_records[index].resolved_matrix);
	resolver->resolved_records[index].resolved_depth = 0U;
	resolver->resolved_records[index].source_transform_flags = 0U;
}

static RenderTransformResolveResult render_transform_resolver_resolve_index(
	RenderTransformResolver *resolver,
	uint32_t index)
{
	const RenderTransformRecord *input_record;
	const RenderTransformRecord *parent_record;
	const RenderResolvedTransformRecord *parent_resolved_record;
	RenderResolvedTransformRecord *resolved_record;
	RenderTransformResolveResult result;
	uint32_t parent_index;

	if (resolver->resolve_states[index] ==
		RENDER_TRANSFORM_RESOLVE_STATE_RESOLVED)
	{
		return RENDER_TRANSFORM_RESOLVE_RESULT_OK;
	}

	if (resolver->resolve_states[index] ==
		RENDER_TRANSFORM_RESOLVE_STATE_RESOLVING)
	{
		return RENDER_TRANSFORM_RESOLVE_RESULT_CYCLE_DETECTED;
	}

	resolver->resolve_states[index] = RENDER_TRANSFORM_RESOLVE_STATE_RESOLVING;
	input_record = &resolver->input_records[index];
	resolved_record = &resolver->resolved_records[index];
	resolved_record->transform_id = input_record->transform_id;
	resolved_record->transform_id.index = index;
	resolved_record->resolved_depth = 0U;
	resolved_record->source_transform_flags = input_record->transform_flags;

	if (render_transform_resolver_parent_is_root(
		&input_record->parent_transform_id))
	{
		resolved_record->resolved_matrix = input_record->local_matrix;
		resolver->resolve_states[index] =
			RENDER_TRANSFORM_RESOLVE_STATE_RESOLVED;
		return RENDER_TRANSFORM_RESOLVE_RESULT_OK;
	}

	if ((input_record->parent_transform_id.handle == RENDER_INVALID_HANDLE) ||
		(input_record->parent_transform_id.index == RENDER_INVALID_INDEX))
	{
		resolver->resolve_states[index] =
			RENDER_TRANSFORM_RESOLVE_STATE_UNRESOLVED;
		return RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_PARENT_LINK;
	}

	parent_index = input_record->parent_transform_id.index;
	if (parent_index >= resolver->count)
	{
		resolver->resolve_states[index] =
			RENDER_TRANSFORM_RESOLVE_STATE_UNRESOLVED;
		return RENDER_TRANSFORM_RESOLVE_RESULT_OUT_OF_RANGE_PARENT;
	}

	parent_record = &resolver->input_records[parent_index];
	if ((parent_record->transform_id.handle !=
		input_record->parent_transform_id.handle) ||
		(parent_record->transform_id.index !=
		input_record->parent_transform_id.index))
	{
		resolver->resolve_states[index] =
			RENDER_TRANSFORM_RESOLVE_STATE_UNRESOLVED;
		return RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_PARENT_LINK;
	}

	result = render_transform_resolver_resolve_index(resolver, parent_index);
	if (result != RENDER_TRANSFORM_RESOLVE_RESULT_OK)
	{
		resolver->resolve_states[index] =
			RENDER_TRANSFORM_RESOLVE_STATE_UNRESOLVED;
		return result;
	}

	parent_resolved_record = &resolver->resolved_records[parent_index];
	render_transform_resolver_multiply_matrices(
		&parent_resolved_record->resolved_matrix,
		&input_record->local_matrix,
		&resolved_record->resolved_matrix);
	resolved_record->resolved_depth =
		parent_resolved_record->resolved_depth + 1U;
	resolver->resolve_states[index] = RENDER_TRANSFORM_RESOLVE_STATE_RESOLVED;
	return RENDER_TRANSFORM_RESOLVE_RESULT_OK;
}

RenderTransformResolveResult render_transform_resolver_initialize(
	RenderTransformResolver *resolver,
	const RenderTransformRecord *input_records,
	RenderResolvedTransformRecord *resolved_records,
	RenderTransformResolveState *resolve_states,
	uint32_t count)
{
	if (resolver == NULL)
	{
		return RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	if (((input_records == NULL) || (resolved_records == NULL) ||
		(resolve_states == NULL)) && (count > 0U))
	{
		return RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	resolver->input_records = input_records;
	resolver->resolved_records = resolved_records;
	resolver->resolve_states = resolve_states;
	resolver->count = count;
	render_transform_resolver_reset(resolver);
	return RENDER_TRANSFORM_RESOLVE_RESULT_OK;
}

void render_transform_resolver_reset(RenderTransformResolver *resolver)
{
	uint32_t input_index;

	if (resolver == NULL)
	{
		return;
	}

	for (input_index = 0U; input_index < resolver->count; ++input_index)
	{
		render_transform_resolver_reset_slot(resolver, input_index);
	}
}

RenderTransformResolveResult render_transform_resolver_resolve_all(
	RenderTransformResolver *resolver)
{
	RenderTransformResolveResult result;
	uint32_t input_index;

	if (resolver == NULL)
	{
		return RENDER_TRANSFORM_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	for (input_index = 0U; input_index < resolver->count; ++input_index)
	{
		result = render_transform_resolver_resolve_index(resolver, input_index);
		if (result != RENDER_TRANSFORM_RESOLVE_RESULT_OK)
		{
			return result;
		}
	}

	return RENDER_TRANSFORM_RESOLVE_RESULT_OK;
}

const RenderResolvedTransformRecord *render_transform_resolver_get_resolved_record(
	const RenderTransformResolver *resolver,
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

	if (resolver->resolve_states[index] !=
		RENDER_TRANSFORM_RESOLVE_STATE_RESOLVED)
	{
		return NULL;
	}

	return &resolver->resolved_records[index];
}
