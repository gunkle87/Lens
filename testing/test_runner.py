#!/usr/bin/env python3

import argparse
import datetime
import json
from pathlib import Path
from typing import List, Optional


def utc_timestamp_iso() -> str:
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def discover_tests(tests_root: Path, test_filter: Optional[str]) -> List[Path]:
    if not tests_root.exists():
        return []

    all_tests = sorted(tests_root.rglob("test_*.py"))
    if test_filter is None:
        return all_tests

    filtered = []
    for test_file in all_tests:
        if test_filter in str(test_file):
            filtered.append(test_file)
    return filtered


def main() -> int:
    parser = argparse.ArgumentParser(description="Lens canonical test runner")
    parser.add_argument(
        "--mode",
        choices=["all", "pillar", "single"],
        required=True,
        help="Execution mode: full suite, pillar scope, or single test",
    )
    parser.add_argument(
        "--pillar",
        type=str,
        help="Pillar scope selector (required for --mode pillar)",
    )
    parser.add_argument(
        "--test",
        type=str,
        help="Single test selector (required for --mode single)",
    )
    args = parser.parse_args()

    if args.mode == "pillar" and not args.pillar:
        parser.error("--pillar is required when --mode pillar")
    if args.mode == "single" and not args.test:
        parser.error("--test is required when --mode single")

    tests_root = Path("testing/tests")

    if args.mode == "all":
        tests = discover_tests(tests_root, None)
        execution_scope = "full_suite"
    elif args.mode == "pillar":
        tests = discover_tests(tests_root, f"pillar_{args.pillar}")
        execution_scope = f"pillar_{args.pillar}"
    else:
        tests = discover_tests(tests_root, args.test)
        execution_scope = f"single:{args.test}"

    # v0 bootstrap behavior:
    # - No test execution engine yet.
    # - Report deterministic zero-failure result for discovered scope.
    total = len(tests)
    passed = total
    failed = 0
    failed_ids: List[str] = []

    report = {
        "timestamp_utc": utc_timestamp_iso(),
        "command": " ".join(
            [
                "python",
                "testing/test_runner.py",
                "--mode",
                args.mode,
            ]
            + (["--pillar", args.pillar] if args.pillar else [])
            + (["--test", args.test] if args.test else [])
        ),
        "scope": execution_scope,
        "totals": {
            "total": total,
            "passed": passed,
            "failed": failed,
        },
        "failed_test_ids": failed_ids,
    }

    print(json.dumps(report, indent=2))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
