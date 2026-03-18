# Directive Templates (Implementation + Audit)

Use these templates when handing work to a new model with zero context.

Keep each directive tight.
Do not include extra scope.

## Delivery Policy

- By default, directive drafts are provided inline in chat inside a copy/paste
  code block.
- Do not create task-specific directive files unless explicitly requested by
  the user.

---

## Template A: Implementation Directive

Copy and fill this template for each implementation task.

```md
# Implementation Directive

## Directive Identity
- Directive ID: `<id>`
- Pillar: `<pillar>`
- Task ID: `<task id>`
- Tracker File: `<path>`
- Plan File: `<path>`

## Required Reading (Must Read In Order)
1. `RULES.md`
2. `BUILD_PROTOCOL.md`
3. `testing/TEST_PROTOCOL.md`
4. `AGENT_ALIGNMENT_MANIFEST.md`
5. `PROGRAMMING_MODEL.md`
6. `<pillar-specific plan file>`
7. `<pillar-specific tracker file>`

## Pre-Execution Gate (Required)
1. Verify all previous tasks are marked complete in the tracker.
2. If any previous task is incomplete, stop and report. Do not implement.
3. Confirm this directive scope matches the current task ID.

## Task Scope
- Objective: `<exact expected outcome>`
- In scope:
  - `<item>`
  - `<item>`
- Out of scope:
  - `<item>`
  - `<item>`

## File Authority (Strict)
- Allowed to read:
  - `<path>`
  - `<path>`
- Allowed to edit:
  - `<path>`
  - `<path>`
- Allowed to create:
  - `<path>`
- Allowed to delete:
  - `<path or NONE>`
- Forbidden to touch:
  - all files not explicitly listed above

## Execution Rules
1. Follow project intent, not internal model preferences.
2. Stay inside this directive scope only.
3. Use naming policy from `RULES.md`.
4. Do not make speculative refactors.
5. Do not change governance docs unless explicitly authorized.

## Required Reporting Policy
At completion, report only:
1. Summary of changes
2. Files touched
3. Tests executed via canonical runner (command + result)
4. Open blockers/non-blockers
5. Next handoff recommendation

## Tracker Update Policy
- Update tracker only for this task.
- Mark only the **Implementation complete** checkbox as `[x]` when implementation is done.
- Do not mark tests/audit/commit checkboxes in this directive.

## Completion Condition
- Implementation is complete and reported.
- Tracker implementation checkbox for this task is marked `[x]`.
```

---

## Template B: Audit Directive

Copy and fill this template for each task audit.

```md
# Audit Directive

## Directive Identity
- Directive ID: `<id>`
- Pillar: `<pillar>`
- Task ID: `<task id>`
- Tracker File: `<path>`
- Prior Implementation Report: `<path or message reference>`

## Required Reading (Must Read In Order)
1. `RULES.md`
2. `BUILD_PROTOCOL.md`
3. `testing/TEST_PROTOCOL.md`
4. `<pillar-specific tracker file>`
5. `<prior implementation report>`

## Pre-Execution Gate (Required)
1. Verify target task implementation checkbox is `[x]`.
2. Verify audit scope file list is present.
3. If prerequisites are missing, stop and report.

## Audit Scope (Strict)
- Audit only files touched by prior implementation:
  - `<path>`
  - `<path>`
- Do not expand scope beyond this file list.

## File Authority (Strict)
- Allowed to read:
  - listed audit-scope files
  - referenced policy docs
- Allowed to edit:
  - `NONE` unless explicitly authorized in this directive
- Allowed to create:
  - `NONE` unless explicitly authorized in this directive
- Allowed to delete:
  - `NONE`

## Audit Checks
1. Scope compliance
2. Functional correctness for task objective
3. Naming/style compliance (`RULES.md`)
4. Boundary compliance (layer intent + no forbidden coupling)
5. Test evidence validity (runner usage, results, current+previous 100%)
6. Regression risk in touched files

## Required Reporting Policy
Report using this exact structure:
1. Verdict: `PASS` or `FAIL`
2. Blockers (must be empty for PASS)
3. Non-blockers
4. File-by-file findings
5. Required fixes (if FAIL)
6. Recommended next step

## Tracker Update Policy
- Do not alter implementation checkbox state.
- If verdict is `PASS`, mark only the **Audit passed** checkbox for this task as `[x]`.
- If verdict is `FAIL`, do not mark audit checkbox.

## Completion Condition
- Audit report delivered.
- Tracker updated per verdict policy.
```
