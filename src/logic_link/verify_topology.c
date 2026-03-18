#include <stdio.h>
#include <stdlib.h>
#include "lens_topology_manager.h"
#include "../scene_core/lens_scene_manager.h"

// Basic asserts
static void assert_test(bool condition, const char* name) {
    printf("[%s] %s\n", condition ? "PASS" : "FAIL", name);
}

int main(void) {
    printf("--- Lens Topology Verification ---\n");
    
    LensSceneDocument* doc = (LensSceneDocument*)calloc(1, sizeof(LensSceneDocument));
    lens_scene_document_init(doc);

    // 1. Create two components
    LensSceneNode* comp_a = lens_scene_create_node(doc, LENS_NODE_COMPONENT, lens_uuid_null());
    LensSceneNode* comp_b = lens_scene_create_node(doc, LENS_NODE_COMPONENT, lens_uuid_null());

    // 2. Route a trace
    LensSceneNode* trace = lens_topology_route_trace(doc, comp_a->id, "Out1", comp_b->id, "In1", "trace_default");
    
    // 3. Add vertices
    bool v1 = lens_topology_add_vertex(trace, 10.0f, 10.0f);
    bool v2 = lens_topology_add_vertex(trace, 20.0f, 10.0f);
    
    // Test Path Routing
    assert_test(v1 && v2 && trace->data.trace.vertex_count == 2, "Path Routing: Trace successfully stores multiple vertices");
    
    // Test Port Binding
    LensSceneNode* results[10];
    uint32_t count_a = lens_topology_get_connected_traces(doc, comp_a->id, "Out1", results, 10);
    uint32_t count_b = lens_topology_get_connected_traces(doc, comp_b->id, NULL, results, 10);
    
    assert_test(count_a == 1 && count_b == 1, "Port Binding: Traces correctly identify their sibling components via UUID");
    
    // Test Dangling Wire Detection
    LensUUID dangling[10];
    uint32_t dang_count = lens_topology_find_dangling_traces(doc, dangling, 10);
    assert_test(dang_count == 0, "Dangling Wire: No dangling wires detected when both endpoints exist");
    
    // Delete comp_b implicitly (nullify ID to mimic node deletion where references are not yet pruned)
    comp_b->id = lens_uuid_null(); 
    
    dang_count = lens_topology_find_dangling_traces(doc, dangling, 10);
    assert_test(dang_count == 1 && lens_uuid_is_equal(dangling[0], trace->id), "Dangling Wire: Successfully detects trace when destination component disappears");

    printf("--- Verification Complete ---\n");
    free(doc);
    return 0;
}
