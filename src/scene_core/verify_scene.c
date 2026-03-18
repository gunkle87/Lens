#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "lens_scene_manager.h"

static bool float_eq(float a, float b) {
    return fabs(a - b) < 0.0001f;
}

static void assert_test(bool condition, const char* name) {
    printf("[%s] %s\n", condition ? "PASS" : "FAIL", name);
}

int main(void) {
    printf("--- Lens Scene Core Verification ---\n");

    LensSceneDocument* doc = (LensSceneDocument*)malloc(sizeof(LensSceneDocument));
    lens_scene_document_init(doc);

    // 1. Identity Stability Test
    LensSceneNode* root = lens_scene_create_node(doc, LENS_NODE_GROUP, lens_uuid_null());
    LensUUID root_id = root->id;
    
    LensSceneNode* child = lens_scene_create_node(doc, LENS_NODE_COMPONENT, root_id);
    LensUUID child_id = child->id;
    
    // Simulate moving the child to a different parent container
    LensSceneNode* new_parent = lens_scene_create_node(doc, LENS_NODE_GROUP, lens_uuid_null());
    LensUUID new_parent_id = new_parent->id;
    
    LensSceneNode* retrieved_child = lens_scene_get_node(doc, child_id);
    retrieved_child->parent_id = new_parent_id;
    
    assert_test(lens_uuid_is_equal(retrieved_child->id, child_id), "Identity Stability: Moving a node across parents does NOT change its core UUID");
    
    // 2. Transform Chain Test
    root->local_transform = lens_matrix_translation(10.0f, 20.0f);
    new_parent->local_transform = lens_matrix_translation(100.0f, 100.0f);
    retrieved_child->local_transform = lens_matrix_translation(5.0f, 5.0f);
    
    LensMatrix3x3 world_tx = lens_scene_get_world_transform(doc, child_id);
    // child is presently under new_parent, World = new_parent * child
    // new_parent is (100, 100), child is (5, 5). World coordinates should resolve to (105, 105)
    assert_test(float_eq(world_tx.m[2], 105.0f) && float_eq(world_tx.m[5], 105.0f), "Transform Chain: Parent transformations correctly propagate structurally to children on demand");

    // 3. Instance Override Test
    lens_scene_set_override_float(retrieved_child, "opacity", 0.75f);
    lens_scene_set_override_string(retrieved_child, "button_label", "Submit Data");
    
    float retrieved_opacity = 0.0f;
    bool found_opacity = lens_scene_get_override_float(retrieved_child, "opacity", &retrieved_opacity);
    
    char retrieved_label[128] = {0};
    bool found_label = lens_scene_get_override_string(retrieved_child, "button_label", retrieved_label, sizeof(retrieved_label));
    
    assert_test(found_opacity && float_eq(retrieved_opacity, 0.75f), "Instance Override: Correctly isolates runtime float overrides from the definition");
    assert_test(found_label && strcmp(retrieved_label, "Submit Data") == 0, "Instance Override: Correctly isolates specific string labels for generic components");
    
    // Test negative case (unknown parameter)
    float dummy;
    assert_test(!lens_scene_get_override_float(retrieved_child, "unknown_prop", &dummy), "Instance Override: Safely handles requests for unbound variant parameters");

    printf("--- Verification Complete ---\n");
    free(doc);
    return 0;
}
