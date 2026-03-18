#ifndef LENS_GRAMMAR_PARSER_H
#define LENS_GRAMMAR_PARSER_H

#include "lens_grammar_types.h"
#include "lens_grammar_errors.h"

// Context for the parser, specifying the active authority
typedef struct {
    LensCustomizationTier active_tier;
    const char* source_path;
} LensParserContext;

// Parser Results
typedef struct {
    bool success;
    LensValidationError error;
} LensValidationResult;

// Ingest a material definition string (simplified KV format for DOD efficiency)
// Format: "id:mat_gold, color:#FFD700, opacity:1.0"
LensValidationResult lens_parse_material(LensParserContext ctx, const char* input, LensMaterialDef* out_mat);

// Ingest a component parameter override
// Format: "param:width, value:50.0"
LensValidationResult lens_validate_parameter_override(LensParserContext ctx, const LensComponentDef* def, const char* param_name, float value);

#endif // LENS_GRAMMAR_PARSER_H
