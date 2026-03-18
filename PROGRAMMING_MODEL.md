# Lens Programming Model

This document defines the mandatory implementation style for Lens. Every
subsystem, from rendering through shell integration, must follow these rules
to preserve determinism, auditability, and architectural clarity.

## Core Mandate

Lens follows a data-oriented systems style by default.

Procedural systems and explicit data structures are preferred over classical
object-oriented ownership models.

## Architectural Style

- DOD first.
  - Design data layouts for iteration clarity and predictable ownership.
- Procedural over OOP.
  - Core logic should live in systems and passes, not inside deep object
    hierarchies.
- Composition over inheritance.
  - Assemble capability through data and bounded systems rather than base-class
    trees.
- Explicit data flow.
  - Data should move through visible pipeline stages such as ingest, validate,
    normalize, resolve, and present.

## Data Layout

- Prefer plain structs and explicit records.
- Prefer stable handles, dense tables, and explicit indices over pointer-heavy
  ownership webs.
- Prefer structure-of-arrays layouts for hot paths when they improve iteration
  or memory behavior.
- Keep immutable authored definitions separate from mutable runtime state.

## Separation Of Concerns

- Definitions are data.
  - Authored state should remain static, inspectable, and serializable.
- Runtime state is data.
  - Current session state should live in runtime tables or equivalent
    structures.
- Systems are logic.
  - Subsystems operate on data through explicit passes.
- Avoid smart objects.
  - Data containers should not quietly own complex logic when a clear system
    pass is better.

## Control Flow And Determinism

- Avoid hidden side effects from setters, observers, or implicit callbacks.
- Prefer explicit phases and bounded transitions.
- All passes must be deterministic and auditable.
- Batch processing is preferred over per-object virtual dispatch in hot paths.

## Language Guardrails

Any deviation from this policy requires explicit authorization.

### Core System

Core substrate logic must be written in C. This includes:
- renderer core
- visual grammar parsing and validation
- scene/runtime state management
- topology and connectivity logic
- binding and adaptation logic
- validation core logic

### Application And UI Packages

Higher-level application shells and package-specific UI structures may use
Java when they sit above the C core through a clear boundary.

### Tooling And Automation

Python is the preferred language for tooling, automation, validation runners,
and authoring support.

### Forbidden Languages

TypeScript and JavaScript are not accepted implementation languages for core,
runtime, renderer, scene, topology, or engine-facing layers.

## Preferred Patterns

- plain records for data shapes
- discriminated variants for explicit kind handling
- dense arrays and handle maps for identity
- explicit system functions over state tables
- immutable definitions paired with mutable runtime state
- explicit validation and normalization stages

## Discouraged Patterns

- inheritance-heavy scene graphs
- base-class-driven ownership models
- per-node `render()` or `update()` methods as the core architecture
- implicit observer webs
- business logic hidden inside UI objects
- object-first designs that obscure data flow

## Exception Rule

A small amount of narrow object-style encapsulation is acceptable in a cold
path or wrapper boundary when it materially improves clarity.

The core architecture must still remain data-oriented and system-driven.
