#include "render_ordering_resolver.h"

static void render_ordering_resolver_set_default_record(
	RenderResolvedOrderRecord *resolved_record)
{
	resolved_record->layer_order_id.handle = RENDER_INVALID_HANDLE;
	resolved_record->layer_order_id.index = RENDER_INVALID_INDEX;
	resolved_record->draw_item_id.handle = RENDER_INVALID_HANDLE;
	resolved_record->draw_item_id.index = RENDER_INVALID_INDEX;
	resolved_record->layer_index = 0;
	resolved_record->order_key = 0;
	resolved_record->submission_index = 0;
	resolved_record->source_input_index = RENDER_INVALID_INDEX;
	resolved_record->source_order_flags = 0;
}

static int render_ordering_resolver_is_invalid_record_id(
	RenderRecordId record_id)
{
	return record_id.handle == RENDER_INVALID_HANDLE ||
		record_id.index == RENDER_INVALID_INDEX;
}

static RenderOrderingResolveResult render_ordering_resolver_validate_record(
	const RenderLayerOrderInputRecord *input_record,
	uint32_t input_index,
	uint32_t record_count)
{
	if (render_ordering_resolver_is_invalid_record_id(
			input_record->layer_order_id) != 0)
	{
		return RENDER_ORDERING_RESOLVE_RESULT_INVALID_RECORD_REFERENCE;
	}

	if (render_ordering_resolver_is_invalid_record_id(
			input_record->draw_item_id) != 0)
	{
		return RENDER_ORDERING_RESOLVE_RESULT_INVALID_RECORD_REFERENCE;
	}

	if (input_record->layer_order_id.index >= record_count)
	{
		return RENDER_ORDERING_RESOLVE_RESULT_INVALID_RECORD_REFERENCE;
	}

	if (input_record->layer_order_id.index != input_index)
	{
		return RENDER_ORDERING_RESOLVE_RESULT_INVALID_RECORD_REFERENCE;
	}

	return RENDER_ORDERING_RESOLVE_RESULT_OK;
}

static int render_ordering_resolver_compare_records(
	const RenderResolvedOrderRecord *left_record,
	const RenderResolvedOrderRecord *right_record)
{
	if (left_record->layer_index < right_record->layer_index)
	{
		return -1;
	}

	if (left_record->layer_index > right_record->layer_index)
	{
		return 1;
	}

	if (left_record->order_key < right_record->order_key)
	{
		return -1;
	}

	if (left_record->order_key > right_record->order_key)
	{
		return 1;
	}

	if (left_record->submission_index < right_record->submission_index)
	{
		return -1;
	}

	if (left_record->submission_index > right_record->submission_index)
	{
		return 1;
	}

	if (left_record->source_input_index < right_record->source_input_index)
	{
		return -1;
	}

	if (left_record->source_input_index > right_record->source_input_index)
	{
		return 1;
	}

	return 0;
}

RenderOrderingResolveResult render_ordering_resolver_initialize(
	RenderOrderingResolver *resolver,
	const RenderLayerOrderInputRecord *input_records,
	RenderResolvedOrderRecord *resolved_records,
	uint32_t count)
{
	if (resolver == 0)
	{
		return RENDER_ORDERING_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	if (count > 0U)
	{
		if (input_records == 0 || resolved_records == 0)
		{
			return RENDER_ORDERING_RESOLVE_RESULT_INVALID_ARGUMENT;
		}
	}

	resolver->input_records = input_records;
	resolver->resolved_records = resolved_records;
	resolver->count = count;

	render_ordering_resolver_reset(resolver);
	return RENDER_ORDERING_RESOLVE_RESULT_OK;
}

void render_ordering_resolver_reset(RenderOrderingResolver *resolver)
{
	uint32_t record_index;

	if (resolver == 0 || resolver->resolved_records == 0)
	{
		return;
	}

	for (record_index = 0; record_index < resolver->count; ++record_index)
	{
		render_ordering_resolver_set_default_record(
			&resolver->resolved_records[record_index]);
	}
}

RenderOrderingResolveResult render_ordering_resolver_resolve_all(
	RenderOrderingResolver *resolver)
{
	uint32_t input_index;
	uint32_t sorted_index;
	RenderResolvedOrderRecord current_record;

	if (resolver == 0)
	{
		return RENDER_ORDERING_RESOLVE_RESULT_INVALID_ARGUMENT;
	}

	if (resolver->count > 0U)
	{
		if (resolver->input_records == 0 || resolver->resolved_records == 0)
		{
			return RENDER_ORDERING_RESOLVE_RESULT_INVALID_ARGUMENT;
		}
	}

	render_ordering_resolver_reset(resolver);

	for (input_index = 0; input_index < resolver->count; ++input_index)
	{
		RenderOrderingResolveResult validation_result;
		const RenderLayerOrderInputRecord *input_record;

		input_record = &resolver->input_records[input_index];
		validation_result = render_ordering_resolver_validate_record(
			input_record,
			input_index,
			resolver->count);
		if (validation_result != RENDER_ORDERING_RESOLVE_RESULT_OK)
		{
			return validation_result;
		}

		resolver->resolved_records[input_index].layer_order_id =
			input_record->layer_order_id;
		resolver->resolved_records[input_index].draw_item_id =
			input_record->draw_item_id;
		resolver->resolved_records[input_index].layer_index =
			input_record->layer_index;
		resolver->resolved_records[input_index].order_key =
			input_record->order_key;
		resolver->resolved_records[input_index].submission_index =
			input_record->submission_index;
		resolver->resolved_records[input_index].source_input_index = input_index;
		resolver->resolved_records[input_index].source_order_flags =
			input_record->order_flags;
	}

	for (input_index = 1; input_index < resolver->count; ++input_index)
	{
		current_record = resolver->resolved_records[input_index];
		sorted_index = input_index;

		while (sorted_index > 0U)
		{
			if (render_ordering_resolver_compare_records(
					&current_record,
					&resolver->resolved_records[sorted_index - 1U]) >= 0)
			{
				break;
			}

			resolver->resolved_records[sorted_index] =
				resolver->resolved_records[sorted_index - 1U];
			--sorted_index;
		}

		resolver->resolved_records[sorted_index] = current_record;
	}

	return RENDER_ORDERING_RESOLVE_RESULT_OK;
}

const RenderResolvedOrderRecord *render_ordering_resolver_get_resolved_record(
	const RenderOrderingResolver *resolver,
	uint32_t index)
{
	if (resolver == 0 || resolver->resolved_records == 0)
	{
		return 0;
	}

	if (index >= resolver->count)
	{
		return 0;
	}

	return &resolver->resolved_records[index];
}
