# Lens Codex Suggestions

This file is non-authoritative guidance for future Codex work in `C:\DEV\Lens`.

The binding rules live in [RULES.md](RULES.md).
The canonical identity and architecture docs live in:
- [AGENT_ALIGNMENT_MANIFEST.md](AGENT_ALIGNMENT_MANIFEST.md)
- [ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)
- [PROGRAMMING_MODEL.md](PROGRAMMING_MODEL.md)
- [VALIDATION_APPROACH.md](VALIDATION_APPROACH.md)

Use this file for helpful implementation and planning habits, not as a source
of product truth.

## Carry Forward From Strata

Use Strata as a process-quality reference, not as a domain template.

The useful habits to keep are:
- separate policy from implementation
- separate layers before code grows
- keep one source of truth for contracts
- document boundaries before building across them
- prefer small coherent slices over sprawling mixed work
- keep audits report-only
- keep capability and customization boundaries explicit
- avoid hidden parallel truths

Import the discipline, not the domain model.

## Good Working Defaults

When planning or documenting Lens work:
- preserve the current architectural boundaries
- prefer clarifying ambiguity over adding speculative detail
- keep documents implementation-facing
- reduce duplicated semantics across files
- write docs that help later code stay bounded

When implementing:
- keep renderer, interaction, scene, layout, and engine communication concerns
  separated
- keep reusable definitions separate from placed instances
- keep scene graph and derived render graph distinct
- keep runtime user editing inside explicit policy boundaries

## Recommended Review Questions

Before accepting a Lens change, ask:
- does this collapse two layers that should stay separate?
- does this hardcode one package or workflow into the shared core?
- does this give end users authoring power that should remain restricted?
- does this create a second truth source for the same contract?
- does this add implementation detail before policy and boundary are clear?

## Useful Future Documentation

If the user asks for more documentation later, the highest-value additions are:
- a runtime struct model
- a binding object model
- a scene/reference integrity contract
- a package-shell model
- a grammar capability registry
- a serialization and persistence boundary

Each new document should reduce ambiguity for later implementation.

## What To Avoid

Do not:
- turn Lens into a browser or SVG pipeline by assumption
- collapse scene graph and render graph together
- treat authored traces or connections as decorative-only paths
- treat the framework as one finished application
- add speculative implementation detail just to make the docs feel larger
