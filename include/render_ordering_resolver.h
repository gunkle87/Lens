#ifndef RENDER_ORDERING_RESOLVER_H
#define RENDER_ORDERING_RESOLVER_H

#include <stdint.h>

#include "render_data_contracts.h"

typedef enum RenderOrderingResolveResult
{
	RENDER_ORDERING_RESOLVE_RESULT_OK = 0,
	RENDER_ORDERING_RESOLVE_RESULT_INVALID_ARGUMENT = 1,
	RENDER_ORDERING_RESOLVE_RESULT_INVALID_RECORD_REFERENCE = 2
} RenderOrderingResolveResult;

typedef struct RenderResolvedOrderRecord
{
	RenderRecordId layer_order_id;
	RenderRecordId draw_item_id;
	int32_t layer_index;
	int32_t order_key;
	uint32_t submission_index;
	uint32_t source_input_index;
	uint32_t source_order_flags;
} RenderResolvedOrderRecord;

typedef struct RenderOrderingResolver
{
	const RenderLayerOrderInputRecord *input_records;
	RenderResolvedOrderRecord *resolved_records;
	uint32_t count;
} RenderOrderingResolver;

RenderOrderingResolveResult render_ordering_resolver_initialize(
	RenderOrderingResolver *resolver,
	const RenderLayerOrderInputRecord *input_records,
	RenderResolvedOrderRecord *resolved_records,
	uint32_t count);

void render_ordering_resolver_reset(RenderOrderingResolver *resolver);

RenderOrderingResolveResult render_ordering_resolver_resolve_all(
	RenderOrderingResolver *resolver);

const RenderResolvedOrderRecord *render_ordering_resolver_get_resolved_record(
	const RenderOrderingResolver *resolver,
	uint32_t index);

#endif
