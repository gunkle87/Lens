#ifndef LENS_INTERACTION_MANAGER_H
#define LENS_INTERACTION_MANAGER_H

#include "lens_interaction_types.h"
#include "../scene_core/lens_scene_manager.h"

// Viewport / Coordinate Systems
void lens_interaction_init_viewport(LensViewport* viewport, float x, float y, float w, float h);
void lens_interaction_set_viewport_transform(LensViewport* viewport, float pan_x, float pan_y, float zoom);
bool lens_interaction_screen_to_world(const LensViewport* viewport, float screen_x, float screen_y, float* out_world_x, float* out_world_y);

// Hit Testing Substrate
LensHitResult lens_interaction_hit_test(LensSceneDocument* doc, const LensViewport* viewport, float screen_x, float screen_y);

// Selection Manager
void lens_interaction_selection_init(LensSelectionState* state);
void lens_interaction_selection_clear(LensSceneDocument* doc, LensSelectionState* state);
void lens_interaction_selection_add(LensSceneDocument* doc, LensSelectionState* state, LensUUID node_id);
void lens_interaction_selection_remove(LensSceneDocument* doc, LensSelectionState* state, LensUUID node_id);
void lens_interaction_selection_toggle(LensSceneDocument* doc, LensSelectionState* state, LensUUID node_id);
bool lens_interaction_selection_contains(const LensSelectionState* state, LensUUID node_id);

// Interaction Properties
void lens_interaction_set_hover(LensSceneDocument* doc, LensUUID node_id, bool hovered);

#endif // LENS_INTERACTION_MANAGER_H
