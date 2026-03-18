#include <stdio.h>
#include <string.h>
#include "lens_grammar_parser.h"

static void assert_test(bool condition, const char* name) {
    printf("[%s] %s\n", condition ? "PASS" : "FAIL", name);
}

int main(void) {
    printf("--- Lens Grammar Verification ---\n");

    // Test 1: Negative Parse (Range Violation - Gate 2)
    {
        LensParserContext ctx = { .active_tier = LENS_TIER_PACKAGE, .source_path = "test.lmat" };
        LensMaterialDef mat;
        const char* bad_input = "id:neon, color:#00FF00, opacity:1.5";
        LensValidationResult res = lens_parse_material(ctx, bad_input, &mat);
        
        assert_test(!res.success && res.error.rule == LENS_RULE_RANGE, "Gate 2: Rejects out-of-bounds opacity (>1.0)");
    }

    // Test 2: Tier Boundary Violation (Gate 6)
    {
        // Define a component with a PACKAGE-tier parameter (locked)
        LensComponentDef def;
        memset(&def, 0, sizeof(def));
        strcpy(def.id, "LogicGate");
        def.param_count = 1;
        strcpy(def.params[0].name, "internal_id");
        def.params[0].tier = LENS_TIER_PACKAGE;
        def.params[0].type = LENS_PARAM_FLOAT;
        def.params[0].constraints.f_range.min = 0.0f;
        def.params[0].constraints.f_range.max = 100.0f;
        def.params[0].constraints.f_range.default_val = 50.0f;
        def.params[0].is_instance_overridable = false; // LOCKED

        // Try to override as USER
        LensParserContext user_ctx = { .active_tier = LENS_TIER_USER };
        LensValidationResult res = lens_validate_parameter_override(user_ctx, &def, "internal_id", 10.0f);

        assert_test(!res.success && res.error.rule == LENS_RULE_TIER_VIOLATION, "Gate 6: Rejects User-tier edit to Package-locked property");
    }

    // Test 3: Valid USER-tier override (Gate 6 success)
    {
        LensComponentDef def;
        memset(&def, 0, sizeof(def));
        def.param_count = 1;
        strcpy(def.params[0].name, "label_width");
        def.params[0].tier = LENS_TIER_USER; // SAFE FOR USER
        def.params[0].type = LENS_PARAM_FLOAT;
        def.params[0].constraints.f_range.min = 0.0f;
        def.params[0].constraints.f_range.max = 100.0f;
        def.params[0].constraints.f_range.default_val = 10.0f;
        def.params[0].is_instance_overridable = true;

        LensParserContext user_ctx = { .active_tier = LENS_TIER_USER };
        LensValidationResult res = lens_validate_parameter_override(user_ctx, &def, "label_width", 25.0f);

        assert_test(res.success, "Gate 6: Allows User-tier edit to User-authorized property");
    }

    printf("--- Verification Complete ---\n");
    return 0;
}
