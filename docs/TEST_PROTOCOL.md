# Lens Test Protocol

This document defines the technical execution suite for the LENS framework. 
It is the **subordinate implementation** of the policies defined in the [Validation Protocol](framework/cross_cutting/VALIDATION_PROTOCOL.md).

## 1. Test Suite Purpose
The **Lens Test Suite** verifies framework correctness and adherence to architectural boundaries.
- Tests must detect regressions in rendering, parsing, and scene management.
- Tests must validate behavior against the **Seven Structural Pillars**.
- Tests must ensure that the **Customization Tier Policy** is strictly enforced.

## 2. Framework Independence Rule
Testing must exist **outside the framework implementation**.
- Tests may read framework state through authorized probes or APIs.
- Tests must not introduce logic or "test modes" inside the core engine.
- Tests must not modify the internal state of the renderer directly; they must interact via the Scene or Grammar layers.

## 3. Determinism & Stability Rule
All tests must be **deterministic**.
- Given the same input (declarative file or signal), the output (Render Graph or Scene State) must be identical every time.
- No "flaky" tests related to timing or non-deterministic OS behavior are permitted.
- Temporal testing for Pillar 5 (Regimes) must use the authoritative **Simulation Time**, not Wall-Clock Time.

## 4. Pillar-Specific Testing Requirements

### 4.1 Pillar 1: Render Core
- Verify primitive Draw Resolution for RECT, CIRCLE, PATH, and TEXT.
- Validate hierarchical Transform math (Matrix multiplication) against a reference.

### 4.2 Pillar 2: Visual Grammar
- **The "Negative Parse" Rule**: Tests must include a suite of malformed `.lcomp` and `.lmat` files to verify that the parser rejects them with readable errors.
- Verify that every supported capability in the **Capability Set** can be correctly parsed.

### 4.3 Pillar 3: Scene & Identity
- Verify **UUID Stability**: IDs must persist across save/load and node mutations.
- Validate the **Undo/Redo Stack**: Every scene operation must be reversible without identity loss.

### 4.4 Pillar 4: Semantic Topology
- Verify that Traces correctly bind to Port IDs, not raw coordinates.
- Validate that Trace paths update correctly when the parent Component moves.

### 4.5 Pillar 5: Data-Binding
- Verify Signal Normalization (Adapter logic) from raw input to normalized signal.
- Validate **Regime Transitions**: Ensure signals switch correctly between PULSE, STEADY, and FLICKER.

### 4.6 Pillar 6: Interaction
- Verify Hit-Testing accuracy for nested and transformed nodes.
- Validate Selection Logic (single/multi) and spatial snapping.

### 4.7 Pillar 7: Shell & Customization
- **Tier Enforcement**: Tests must attempt to edit Tier-1 (Engine) properties from a Tier-3 (User) context and verify they are rejected.
- Validate Tool Gating based on package permissions.

## 5. Test Creation Rule
When any new functionality or architectural capability is introduced to a Pillar:
- At least one test must be created to verify that capability.
- At least one "boundary test" must be created to ensure the new capability does not leak into restricted layers.

## 6. Failure & Reporting Policy
When a test fails:
- The failure must report the specific Pillar and Rule violated.
- The system must provide the expected state vs. the actual state.
- Automated fixes are **forbidden**. All fixes require explicit developer review and alignment with the **AGENT_ALIGNMENT_MANIFEST**.

## 7. Clean-Room Verification
Tests must ensure that no third-party patterns or unapproved external dependencies are introduced during the implementation of a pillar, maintaining the **Clean-Room Rule** defined in `RULES.md`.
