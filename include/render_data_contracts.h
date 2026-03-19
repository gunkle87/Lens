#ifndef RENDER_DATA_CONTRACTS_H
#define RENDER_DATA_CONTRACTS_H

#include <stdint.h>

#define RENDER_INVALID_HANDLE UINT32_MAX
#define RENDER_INVALID_INDEX UINT32_MAX
#define RENDER_MATRIX_4X4_ELEMENT_COUNT 16U

typedef struct RenderRecordId
{
	uint32_t handle;
	uint32_t index;
} RenderRecordId;

typedef struct RenderMatrix4x4
{
	float elements[RENDER_MATRIX_4X4_ELEMENT_COUNT];
} RenderMatrix4x4;

typedef struct RenderClipRect
{
	float min_x;
	float min_y;
	float max_x;
	float max_y;
} RenderClipRect;

typedef struct RenderDrawItemRecord
{
	RenderRecordId draw_item_id;
	RenderRecordId transform_id;
	RenderRecordId clip_id;
	RenderRecordId layer_order_id;
	uint32_t source_handle;
	uint32_t draw_kind;
	uint32_t draw_flags;
} RenderDrawItemRecord;

typedef struct RenderTransformRecord
{
	RenderRecordId transform_id;
	RenderRecordId parent_transform_id;
	RenderMatrix4x4 local_matrix;
	uint32_t transform_flags;
} RenderTransformRecord;

typedef struct RenderClipRecord
{
	RenderRecordId clip_id;
	RenderRecordId parent_clip_id;
	RenderClipRect clip_rect;
	uint32_t clip_flags;
} RenderClipRecord;

typedef struct RenderLayerOrderInputRecord
{
	RenderRecordId layer_order_id;
	RenderRecordId draw_item_id;
	int32_t layer_index;
	int32_t order_key;
	uint32_t submission_index;
	uint32_t order_flags;
} RenderLayerOrderInputRecord;

#endif
