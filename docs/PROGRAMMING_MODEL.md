# Lens Programming Model: Data-Oriented Design (DOD)

This document defines the mandatory programming style and architectural patterns for the Lens project. Every subsystem, from the renderer to the shell integration, must adhere to these rules to ensure determinism, performance, and auditability.

## Core Mandate

This project follows a **Data-Oriented Systems (DBS/DOD)** style by default. Procedural systems and explicit data structures are preferred over classical Object-Oriented Programming (OOP).

### 1. Architectural Style
- **DOD First**: Design data layouts for efficiency and clarity.
- **Procedural over OOP**: Core logic should live in systems/functions, not hidden in objects.
- **Composition over Inheritance**: Assemble capabilities from data fields, not from a class hierarchy.
- **Explicit Data Flow**: Data should move through visible pipeline stages (e.g., Ingest -> Validate -> Normalize -> Render).

### 2. Data Layout (SoA over AoS)
- Wherever a system processes many items (transforms, simulation, render-graph flattening, validation), prefer **Structure of Arrays (SoA)** over **Array of Structures (AoS)** if it improves iteration or memory layout.
- Use plain structs and data records. Avoid deep, stateful class trees.

### 3. Separation of Concerns
- **Definitions are Data**: Authored state should be static and inspectable.
- **Runtime State is Data**: Current session state should be stored in tables/handles.
- **Systems are Logic**: Subsystems operate on that data through explicit passes.
- No "Smart Objects": Avoid objects that own their own logic when a system pass is clearer.

### 4. Control Flow & Determinism
- **Explicit Phases**: Avoid hidden side effects from property setters or observers.
- **Stable IDs & Handles**: Use stable identifiers and table/index-based relationships instead of pointer-style ownership webs.
- **Batch Processing**: Prefer operating on batches of data over per-object virtual behavior.
- **Determinism**: All passes must be deterministic and auditable.

### 5. Language & Stack Guardrails

This project has a **strict implementation-language policy**. Any deviation from this policy requires explicit justification and authorization.

#### Core System (C Only)
The core substrate logic must be written in **C**. This includes:
- **Renderer Core** (primitive draw execution, transforms, clipping, graph flattening)
- **Visual Grammar Execution** (parsing, Astra, and validation passes)
- **Scene Core** (identity, hierarchy, and runtime state tables)
- **Topology/Connectivity Core** (port binding, trace routing)
- **Binding/Adaptation Core** (normalized signaling adapters)
- **Validation Core** (Gate 1-4 integrity checks)

#### Application & UI Packages (Java)
For higher-level application shells, UI layouts, and package-specific behaviors that sit on top of the C core, the preferred language is **Java**. These components must communicate with the C core through a defined, data-oriented boundary.

#### Tooling, Automation & Support (Python)
The preferred language for conversion tools, automation scripts, validation runners, and authoring support is **Python**.

#### Forbidden Languages
- **TypeScript/JavaScript**: This project is **not** a web-stack project. TypeScript is **not** an accepted implementation language for core, runtime, renderer, scene, topology, or engine-facing layers.

### 6. Architecture First, Language Second
No language choice may weaken the architectural model. The project remains data-oriented, system-driven, and layer-separated. Managed or scripting languages must not become the architectural center of ownership.

## Preferred Patterns
- **Records/Interfaces** for data shapes.
- **Discriminated Unions** for variant kinds.
- **Dense Arrays** plus handle maps for identity.
- **System Functions** operating over state tables.
- **Explicit Pipeline Stages**.
- **Immutable Definitions** paired with mutable runtime state tables.

## Discouraged Patterns
- **Inheritance-heavy** scene graphs.
- **Base-class-driven** architectures.
- **Per-node `render()` or `update()` methods**.
- **Implicit Observer Webs** (events/signals should be explicit and directed).
- **Business Logic** hidden inside UI objects.
- **AoS-heavy layouts** for performance-critical hot paths.
- **"Everything is a Class"** design patterns.

## Exception Rule
If a small amount of object-style encapsulation materially improves clarity in a cold path or narrow boundary wrapper, it is acceptable. However, the core architecture must remains data-oriented and system-driven.
