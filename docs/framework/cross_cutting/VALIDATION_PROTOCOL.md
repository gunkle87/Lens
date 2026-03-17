# VALIDATION PROTOCOL

============================================================
PURPOSE
============================================================

This document defines the multi-gate validation process for the LENS
system. It is the **primary architectural authority** for correctness.
The technical execution of these rules is documented in the [Test Protocol](../../TEST_PROTOCOL.md).

============================================================
VALIDATION GATES
============================================================

1. FORMAT VALIDATION (Lex/Parse)
   - Ensure .lcomp, .lmat, .lscn files follow the declarative grammar.
   - Reject malformed syntax or unknown primitive opcodes.

2. SCHEMA & LIMITS VALIDATION
   - Reject out-of-range parameters (e.g., opacity < 0.0).
   - Verify required fields (e.g., every ComponentDef must have a Symbol).

3. ASSET INTEGRITY VALIDATION
   - Ensure all ComponentNodes reference a valid ComponentDef in the registry.
   - Ensure all Styles reference a valid MaterialDef.
   - Detect and reject cyclic references in groups or subassemblies.

4. SCENE & TOPOLOGY VALIDATION
   - Check if IDs are unique.
   - Ensure Traces have valid endpoints (Ports/Nodes).
   - Detect and flag disconnected ports if "Enforce Connectivity" is enabled.

5. RENDERER VALIDATION
   - Verify the Render Graph generation is deterministic.
   - Check for stable Z-sorting within LayerClasses.
   - Ensure bounds calculations are accurate for culling.

6. POLICY & TIER VALIDATION (Crucial)
   - Ensure Runtime Users cannot edit Package-Author properties.
   - Ensure Scripts cannot touch renderer internals.
   - Reject non-whitelisted property overrides.

============================================================
VALIDATION REPORTING
============================================================

Validation should be:
- LOUD during authoring/startup (throw error, stop load).
- CONTROLLED during runtime (log warning, revert to default).

All errors must report:
- Severity (Error, Warning, Info)
- Source (File path, Line number)
- Object (ID, Name)
- Rule violated

============================================================
AUTOMATED TESTING EXPECTATIONS
============================================================

The system should include "Negative Files" in its test suite:
- A file with a missing ID should fail Gate 4.
- A file with a custom bloom kernel in tier-3 should fail Gate 6.

============================================================
BOTTOM LINE
============================================================

Validation is not an afterthought. It is the enforcement mechanism for the
architectural boundaries. If the system cannot validate a file, it
should not render it.
