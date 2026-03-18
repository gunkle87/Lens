# Lens Build Protocol

This document defines the execution protocol for planning and implementation
work in this repository.

It exists to keep work small, auditable, and deterministic.

## Scope

- This protocol applies to all build and architecture work.
- Canonical and governance documentation lives in the repo root.
- Testing governance is defined in `testing/TEST_PROTOCOL.md`.

## Terminology (Strict)

- **Pillar**: one of the 8 architecture execution stages listed below.
  - The word "pillar" is reserved for these stages only.
- **Task**: either:
  - one user directive, or
  - one pre-approved scope the agent can complete end-to-end without user
    intervention.

## Pre-Pillar Governance Stage (Overall)

This stage is required before pillar execution starts.

Required outputs:
- scope statement
- success criteria
- dependency order across pillars
- audit rubric (blockers/non-blockers)
- small-task sizing guidance

This governance stage is not a pillar.

## Pillar Model (8 Pillars)

Execution pillars are:
1. Native Render
2. Visual Description
3. Asset Definition
4. Scene And Workspace Construction
5. Layout
6. Interaction
7. Engine Communication
8. Application Shell Integration

## Hard Gate: Pre-Task Verification

Before starting any new task, verify all prior tasks are complete.

Completion means all of the following exist for each prior task:
- implementation marked complete
- test pass evidence recorded (current + previous tests at 100%)
- audit marked complete and passed at 100%
- local commit recorded with commit ID

If any prior task is incomplete, execution must stop and report the gap.

## Mandatory Task Lifecycle

Each task must follow this exact order:

1. Pre-task verification
2. Implementation
3. Runner-enforced test pass
4. Task audit
5. Local commit
6. Task record update

### Mandatory Test Gate Requirements

- The canonical runner must be used for all test execution.
- No tests may be run outside the canonical runner.
- No task can pass audit unless all current and previous tests pass at 100%.
- See `testing/TEST_PROTOCOL.md` for full test policy.

### Task Audit Requirements

- Audit must clear all blockers and non-blockers.
- Pass threshold is 100%.
- No next-task progression is allowed before audit pass.

### Local Commit Requirements

- Task must be committed locally after audit pass.
- Record commit hash in the task record.

## Mandatory Pillar Closure Lifecycle

At pillar completion:
1. Confirm every pillar task is complete.
2. Run required pillar test sweep through the canonical runner.
3. Confirm pillar tests pass at 100%.
4. Run full pillar audit.
5. Pillar audit must pass at 100%.
6. Record pillar completion.
7. Ensure local commit state is correct.
8. Push pillar-complete state to GitHub.

## Required Task Checklist Fields

Each task record must include all four checklist items:

- [ ] Implementation complete — `YYYY-MM-DD HH:MM TZ`
- [ ] Tests passed (current + previous = 100%) —
      `YYYY-MM-DD HH:MM TZ` — `runner: <COMMAND>`
- [ ] Audit passed (100%, blockers=0, non-blockers=0) —
      `YYYY-MM-DD HH:MM TZ`
- [ ] Local commit created — `YYYY-MM-DD HH:MM TZ` —
      `commit: <HASH>`

## Recording Standard

- Use local time with timezone in all timestamps.
- `PILLAR_TRACKER.md` is a template/guiderail, not the live run log.
- For active execution, create a run-specific tracker file copied from the
  template and update that run file.
