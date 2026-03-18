# Lens Test Protocol

This document defines the mandatory testing policy for Lens build work.

Testing is critical-path work.
Testing is never optional.

## Scope

- This protocol applies to all implementation tasks in this repository.
- Tests and the test runner are build tools for this repo only.
- Tests and test runner components must not ship with final product artifacts.

## Hard Test Gates

1. No implementation task is complete without test execution evidence.
2. **No task can pass audit unless all current and previous tests pass at 100%.**
3. No test failure may be carried forward as a blocker or non-blocker.
4. No task may proceed to commit while test pass status is below 100%.
5. No pillar may close unless its full required test set passes at 100%.

## Canonical Test Runner Policy

1. A single canonical test runner is required.
2. The runner is exclusive.
   - No test may be run outside the canonical runner.
3. The runner must support all of the following:
   - full suite sweep
   - pillar-scoped sweep
   - single-test execution
4. The runner must produce standardized output including:
   - execution timestamp
   - executed scope
   - pass/fail totals
   - failed test identifiers
   - command used

## Task-Level Test Flow

For each implementation task:
1. Run targeted tests while implementing as needed.
2. Before audit, run the canonical runner against the required scope to prove
   that all current and previous tests pass at 100%.
3. Record evidence in the task tracker.

## Required Test Evidence

Each task record must include:
- timestamp (local time + timezone)
- runner command
- execution scope (single test, pillar sweep, or full sweep)
- pass/fail totals
- failed test IDs (must be none at completion)
- rerun evidence if failures were fixed

## Completion Report Requirement

Final implementation completion reports must include a compiled test section
containing:
- tests executed
- pass/fail totals
- unresolved failures (must be zero)
- explicit statement that current and previous tests are 100% green

## Core Attachment Prohibition

Tests and test runner components must not attach directly to core runtime code
paths.

If any direct attachment is unavoidable:
1. It must be documented in `testing/TEST_CORE_ATTACHMENT_EXCEPTIONS.md`.
2. It must be heavily and clearly commented in code.
3. Comments must include explicit removal steps.
4. The attachment must be treated as temporary and removable before final
   compile/release build.
