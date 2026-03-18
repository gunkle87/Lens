# Lens Validation Approach

This document defines the current validation posture for Lens.

It is not a test runner specification.
It is the architectural statement of what Lens must be able to validate as the
framework grows.

## Core Principle

Validation must exist at the same architectural boundaries that Lens itself
claims to preserve.

If a boundary matters in the docs, it must be checkable in some form.

## Validation Categories

### Format Validation
- malformed authored data is rejected explicitly
- unknown keys and invalid values are handled deterministically
- schema-level constraints are checked before runtime use

### Asset Validation
- duplicate IDs are rejected
- missing references are surfaced clearly
- invalid variant, material, or primitive references are rejected

### Scene Validation
- invalid object references are rejected
- hierarchy and transform relationships remain coherent
- visibility, layering, and identity rules remain consistent

### Layout Validation
- container structure remains coherent
- placement and composition rules remain valid
- shell/layout declarations do not bypass framework boundaries

### Interaction Validation
- approved editing flows stay bounded
- hit testing and selection behavior remain coherent
- interaction rules do not silently bypass policy tiers

### Engine Communication Validation
- engine-facing identifiers remain stable at the boundary
- normalized UI-facing state is coherent and inspectable
- command submission remains inside the defined bridge

### Policy Validation
- customization tiers are enforced
- runtime-user edits remain limited to whitelisted changes
- package-level authoring powers do not leak to runtime users

## Proof Strategy

Lens validation should favor:
- structural checks before behavior checks
- deterministic comparisons before manual interpretation
- explicit rejection paths over silent fallback
- boundary-focused verification over ad hoc spot checks

## Quality Bar

A Lens validation story is acceptable only when it is:
- explicit
- layered
- deterministic
- aligned with the documented architecture

Validation should reduce ambiguity, not merely add ceremony.
