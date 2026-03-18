#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lens_interaction_manager.h"

void fail(const char* msg) {
    printf("[FAIL] %s\n", msg);
    exit(1);
}

void test_aabb_hit() {
    printf("Running AABB Hit Test Proof...\n");
    static LensSceneDocument doc;
    lens_scene_document_init(&doc);
    
    LensSceneNode* node1 = lens_scene_create_node(&doc, LENS_NODE_COMPONENT, lens_uuid_null());
    node1->is_visible = true;
    node1->local_transform = lens_matrix_translation(100.0f, 100.0f);
    lens_scene_set_override_float(node1, "Width", 50.0f);
    lens_scene_set_override_float(node1, "Height", 50.0f);
    
    LensViewport vp;
    lens_interaction_init_viewport(&vp, 0.0f, 0.0f, 800.0f, 600.0f);
    lens_interaction_set_viewport_transform(&vp, 0.0f, 0.0f, 1.0f);
    
    LensHitResult hit1 = lens_interaction_hit_test(&doc, &vp, 125.0f, 125.0f);
    if (hit1.hit != true) fail("hit1.hit != true");
    if (!lens_uuid_is_equal(hit1.node_id, node1->id)) fail("hit_id != node_id");
    
    LensHitResult hit2 = lens_interaction_hit_test(&doc, &vp, 80.0f, 80.0f);
    if (hit2.hit != false) fail("hit2.hit != false");
    if (!lens_uuid_is_null(hit2.node_id)) fail("hit2_id not null");
    
    printf("AABB Hit Test Proof passed.\n");
}

void test_selection_toggle() {
    printf("Running Selection Toggle Test...\n");
    static LensSceneDocument doc;
    lens_scene_document_init(&doc);
    
    LensSceneNode* node1 = lens_scene_create_node(&doc, LENS_NODE_COMPONENT, lens_uuid_null());
    
    LensSelectionState state;
    lens_interaction_selection_init(&state);
    
    lens_interaction_selection_toggle(&doc, &state, node1->id);
    if (!lens_interaction_selection_contains(&state, node1->id)) fail("not contained after toggle");
    
    float selected_val = 0.0f;
    bool has_param = lens_scene_get_override_float(node1, "IsSelected", &selected_val);
    if (!has_param || selected_val != 1.0f) fail("override IsSelected != 1.0f");
    
    lens_interaction_selection_toggle(&doc, &state, node1->id);
    if (lens_interaction_selection_contains(&state, node1->id)) fail("contained after second toggle");
    
    has_param = lens_scene_get_override_float(node1, "IsSelected", &selected_val);
    if (!has_param || selected_val != 0.0f) fail("override IsSelected != 0.0f after toggle off");
    printf("Selection Toggle Test passed.\n");
}

void test_viewport_clip() {
    printf("Running Viewport Clip Test...\n");
    static LensSceneDocument doc;
    lens_scene_document_init(&doc);
    
    LensSceneNode* node1 = lens_scene_create_node(&doc, LENS_NODE_COMPONENT, lens_uuid_null());
    node1->is_visible = true;
    node1->local_transform = lens_matrix_translation(100.0f, 100.0f);
    
    LensViewport vp;
    lens_interaction_init_viewport(&vp, 100.0f, 100.0f, 400.0f, 400.0f);
    lens_interaction_set_viewport_transform(&vp, 0.0f, 0.0f, 1.0f);
    
    LensHitResult hit1 = lens_interaction_hit_test(&doc, &vp, 205.0f, 205.0f);
    if (hit1.hit != true) fail("hit1.hit != true in viewport clip test");
    
    LensHitResult hit2 = lens_interaction_hit_test(&doc, &vp, 50.0f, 50.0f);
    if (hit2.hit != false) fail("hit2.hit != false in viewport clip test");
    printf("Viewport Clip Test passed.\n");
}

int main() {
    setbuf(stdout, NULL);
    printf("========== INTERACTION VALIDATION ==========\n");
    test_aabb_hit();
    test_selection_toggle();
    test_viewport_clip();
    printf("All Interaction checks passed!\n");
    return 0;
}
