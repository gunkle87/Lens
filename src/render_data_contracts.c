#include "render_data_contracts.h"

#include <stddef.h>

_Static_assert(sizeof(RenderRecordId) == (sizeof(uint32_t) * 2U),
	"RenderRecordId must store a handle/index pair.");
_Static_assert(sizeof(((RenderMatrix4x4 *)0)->elements) ==
	(sizeof(float) * RENDER_MATRIX_4X4_ELEMENT_COUNT),
	"RenderMatrix4x4 must remain a fixed-size matrix record.");
_Static_assert(offsetof(RenderDrawItemRecord, draw_item_id) == 0U,
	"RenderDrawItemRecord identity must lead the record.");
_Static_assert(offsetof(RenderTransformRecord, transform_id) == 0U,
	"RenderTransformRecord identity must lead the record.");
_Static_assert(offsetof(RenderClipRecord, clip_id) == 0U,
	"RenderClipRecord identity must lead the record.");
_Static_assert(offsetof(RenderLayerOrderInputRecord, layer_order_id) == 0U,
	"RenderLayerOrderInputRecord identity must lead the record.");
