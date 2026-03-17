# Lens: A Universal UI Framework for Logic Simulation

Lens is a layered, engine-agnostic framework designed for building deterministic and visually premium logic simulation interfaces.

## 🏛️ The Structural Pillars

The project is organized around seven core architectural pillars, each representing a stable capability target:

1.  **Rendering Engine (Foundation)**: Native execution environment for visual primitives (`src/render_core`).
2.  **Visual Grammar (Language)**: Declarative specification for describing visual intent (`docs/framework/pillar_2_grammar`).
3.  **Scene & Identity Core (Workspace)**: Stable hierarchy of identifiable objects (`src/scene_core`).
4.  **Semantic Connectivity (Topology)**: Abstraction for logical and relational connections (`src/logic_link`).
5.  **Data-Binding & Adaptation (The Bridge)**: Deterministic normalization of external runtime truth (`src/comm_link`).
6.  **Generic Interaction (Toolset)**: Package-agnostic logic for human manipulation (`src/interaction_core`).
7.  **Shell & Layout Substrate (Execution Frame)**: Declarative environment for application construction (`src/shell_core`).

## 📁 Project Structure

- `docs/`: Comprehensive architectural specifications and rules.
  - `framework/`: Pillar-aligned specification documents.
- `src/`: Source code organized by architectural responsibility.

> [!IMPORTANT]
> **[AGENT_ALIGNMENT_MANIFEST.md](AGENT_ALIGNMENT_MANIFEST.md)** is the canonical onboarding and alignment source for this repository.

## 🏁 Goal State

Lens provides the "Substrate" (Engine) while individual **Packages** (like *180-in-1*) provide the "Fuel" (Assets, Themes, Tools). This separation allows multiple products to share a high-performance, consistent core while maintaining unique visual identities.

Together, these layers form a flexible authoring framework that allows a complete UI to be constructed dynamically and deterministically, without coupling to a specific engine implementation.

## 🗺️ Documentation Index

- [Project Documentation Hub](docs/INDEX.md)
- [Project Phases & Progression](docs/planning/PROJECT_PHASES.md)
- [Agent Rules & Constraints](docs/RULES.md)
