#include "lens_grammar_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void set_error(LensValidationResult* res, LensErrorSeverity sev, LensValidationRule rule, const char* msg, const char* obj) {
    res->success = false;
    res->error.severity = sev;
    res->error.rule = rule;
    strncpy(res->error.message, msg, 255);
    if (obj) strncpy(res->error.object_id, obj, 63);
}

// Simple internal helper to find and parse values in "key:value, key:value" strings
static const char* find_value(const char* input, const char* key) {
    char search[64];
    snprintf(search, 64, "%s:", key);
    const char* p = strstr(input, search);
    if (!p) return NULL;
    return p + strlen(search);
}

LensValidationResult lens_parse_material(LensParserContext ctx, const char* input, LensMaterialDef* out_mat) {
    LensValidationResult res = { .success = true };
    memset(out_mat, 0, sizeof(LensMaterialDef));
    out_mat->source_tier = ctx.active_tier;

    // 1. GATE 1 - Format/Opcodes
    const char* id_val = find_value(input, "id");
    if (!id_val) {
        set_error(&res, LENS_SEVERITY_ERROR, LENS_RULE_MISSING_FIELD, "Missing 'id' field in material", NULL);
        return res;
    }
    sscanf(id_val, "%63[^,]", out_mat->id);

    const char* col_val = find_value(input, "color");
    if (col_val) {
        if (col_val[0] == '#') {
            unsigned int hex;
            sscanf(col_val + 1, "%x", &hex);
            out_mat->base_color = (uint32_t)hex;
        }
    }

    const char* op_val = find_value(input, "opacity");
    if (op_val) {
        float op = (float)atof(op_val);
        // 2. GATE 2 - Schema/Limits
        if (op < 0.0f || op > 1.0f) {
            set_error(&res, LENS_SEVERITY_ERROR, LENS_RULE_RANGE, "Opacity must be between 0.0 and 1.0", out_mat->id);
            return res;
        }
        out_mat->opacity = op;
    } else {
        out_mat->opacity = 1.0f; // Default
    }

    // 3. GATE 6 - Tier Policy
    // In this simplified parser, we just mark the def with the active tier.
    // Real logic would check if an existing def is being overwritten by a lower tier.

    return res;
}

LensValidationResult lens_validate_parameter_override(LensParserContext ctx, const LensComponentDef* def, const char* param_name, float value) {
    LensValidationResult res = { .success = true };

    // Find the definition
    const LensParameterDef* p_def = NULL;
    for (uint32_t i = 0; i < def->param_count; i++) {
        if (strcmp(def->params[i].name, param_name) == 0) {
            p_def = &def->params[i];
            break;
        }
    }

    if (!p_def) {
        set_error(&res, LENS_SEVERITY_ERROR, LENS_RULE_MISSING_FIELD, "Unknown parameter name", param_name);
        return res;
    }

    // 1. GATE 6 - Tier Policy Violation
    // Context Tier (e.g. USER) cannot modify property of higher Tier (e.g. ENGINE/PACKAGE)
    // unless specifically marked as instance-overridable.
    if (ctx.active_tier > p_def->tier && !p_def->is_instance_overridable) {
        set_error(&res, LENS_SEVERITY_ERROR, LENS_RULE_TIER_VIOLATION, "Forbidden: User tier cannot override package-locked parameter", param_name);
        return res;
    }

    // 2. GATE 2 - Range Check
    if (p_def->type == LENS_PARAM_FLOAT) {
        if (value < p_def->constraints.f_range.min || value > p_def->constraints.f_range.max) {
            set_error(&res, LENS_SEVERITY_ERROR, LENS_RULE_RANGE, "Value out of defined range", param_name);
            return res;
        }
    }

    return res;
}
