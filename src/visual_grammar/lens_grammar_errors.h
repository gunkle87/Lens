#ifndef LENS_GRAMMAR_ERRORS_H
#define LENS_GRAMMAR_ERRORS_H

#include <stdint.h>

typedef enum {
    LENS_SEVERITY_INFO,
    LENS_SEVERITY_WARNING,
    LENS_SEVERITY_ERROR,
    LENS_SEVERITY_FATAL
} LensErrorSeverity;

typedef enum {
    LENS_RULE_SYNTAX,             // Gate 1: Lexical/Format
    LENS_RULE_RANGE,              // Gate 2: Schema/Limits
    LENS_RULE_TIER_VIOLATION,     // Gate 6: Policy/Tier
    LENS_RULE_UNKNOWN_OPCODE,     // Unknown primitive
    LENS_RULE_MISSING_FIELD       // Required data missing
} LensValidationRule;

typedef struct {
    LensErrorSeverity severity;
    LensValidationRule rule;
    char message[256];
    char source_path[128];
    int line_number;
    char object_id[64];
} LensValidationError;

#endif // LENS_GRAMMAR_ERRORS_H
