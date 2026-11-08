import argparse
import re
import xml.etree.ElementTree as ET
from pathlib import Path


def verify_results(root):
    cases = list(root.iter("testcase"))
    if not cases:
        raise ValueError("The CTest report contains no test cases")
    problems = []
    google_test_counts = []
    for suite in root.iter("testsuite"):
        if int(suite.get("skipped", "0")) or int(suite.get("disabled", "0")):
            problems.append(f"Skipped or disabled tests in {suite.get('name', 'CTest')}")
    for case in cases:
        name = case.get("name", "unnamed test")
        if any(case.find(tag) is not None for tag in ("failure", "error", "skipped")):
            problems.append(f"CTest did not pass {name}")
        output = case.findtext("system-out", "") + case.findtext("system-err", "")
        counts = [int(value) for value in re.findall(
            r"\[==========\] Running (\d+) tests? from", output,
        )]
        google_test_counts.extend(counts)
        if 0 in counts:
            problems.append(f"Zero GoogleTest cases executed in {name}")
        if re.search(r"\[\s*SKIPPED\s*\]|\bDISABLED TESTS?\b|\bDISABLED_", output):
            problems.append(f"Skipped or disabled GoogleTest cases in {name}")
        if re.search(r"^\s*[^:\r\n]+:\s*skipped\b", output, re.MULTILINE | re.IGNORECASE):
            problems.append(f"Skipped backend checks in {name}")
    if not google_test_counts:
        problems.append("No GoogleTest execution records were found")
    if problems:
        raise ValueError("\n".join(problems))
    return len(cases), len(google_test_counts), sum(google_test_counts)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("report", type=Path)
    options = parser.parse_args()
    try:
        cases, programs, google_tests = verify_results(ET.parse(options.report).getroot())
    except (OSError, ET.ParseError, ValueError) as error:
        parser.exit(1, f"CTest execution verification failed: {error}\n")
    print(f"Verified {cases} CTest cases and {google_tests} GoogleTest cases in "
          f"{programs} program executions; zero skipped, disabled or empty tests")


if __name__ == "__main__":
    main()
