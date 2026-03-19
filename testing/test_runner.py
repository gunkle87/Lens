#!/usr/bin/env python3

import argparse
import datetime
import io
import json
from pathlib import Path
from typing import List, Optional
import unittest


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
        if test_filter in test_file.name:
            filtered.append(test_file)
    return filtered


def build_suite(tests_root: Path, mode: str, pillar: Optional[str], single_test: Optional[str]) -> unittest.TestSuite:
    loader = unittest.TestLoader()

    if mode == "all":
        return loader.discover(
            start_dir=str(tests_root),
            pattern="test_*.py",
        )

    if mode == "pillar":
        return loader.discover(
            start_dir=str(tests_root),
            pattern=f"test_pillar_{pillar}_*.py",
        )

    single_test_pattern = Path(single_test).name if single_test else ""
    return loader.discover(
        start_dir=str(tests_root),
        pattern=single_test_pattern,
    )


def run_suite(test_suite: unittest.TestSuite) -> unittest.TestResult:
    output_stream = io.StringIO()
    runner = unittest.TextTestRunner(stream=output_stream, verbosity=0)
    return runner.run(test_suite)


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
        tests = discover_tests(tests_root, Path(args.test).name)
        execution_scope = f"single:{args.test}"

    suite = build_suite(tests_root, args.mode, args.pillar, args.test)
    total = suite.countTestCases()

    if total == 0:
        failed = 1
        passed = 0
        failed_ids = ["NO_TESTS_DISCOVERED"]
    else:
        result = run_suite(suite)
        failed_cases = result.failures + result.errors
        failed = len(failed_cases)
        passed = result.testsRun - failed
        failed_ids = sorted(test_case.id() for test_case, _ in failed_cases)

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
