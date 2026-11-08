import unittest
import xml.etree.ElementTree as ET

from verify_ctest_results import verify_results


def report(output="[==========] Running 3 tests from 1 test suite."):
    root = ET.Element("testsuite", name="CTest", skipped="0", disabled="0")
    case = ET.SubElement(root, "testcase", name="GeometryTest")
    ET.SubElement(case, "system-out").text = output
    return root, case


class CTestExecutionVerificationTest(unittest.TestCase):
    def test_counts_real_program_executions_and_cases(self):
        root, _ = report()
        _, second = report("[==========] Running 7 tests from 2 test suites.")
        root.append(second)
        self.assertEqual(verify_results(root), (2, 2, 10))

    def test_rejects_an_empty_report(self):
        with self.assertRaisesRegex(ValueError, "no test cases"):
            verify_results(ET.Element("testsuite"))

    def test_rejects_missing_google_test_execution_records(self):
        root, _ = report("No tests were executed")
        with self.assertRaisesRegex(ValueError, "No GoogleTest execution records"):
            verify_results(root)

    def test_rejects_zero_case_google_test_programs(self):
        root, _ = report("[==========] Running 0 tests from 0 test suites.")
        with self.assertRaisesRegex(ValueError, "Zero GoogleTest cases"):
            verify_results(root)

    def test_rejects_ctest_failures_errors_and_skipped_cases(self):
        for tag in ("failure", "error", "skipped"):
            with self.subTest(tag=tag):
                root, case = report()
                ET.SubElement(case, tag)
                with self.assertRaisesRegex(ValueError, "did not pass"):
                    verify_results(root)

    def test_rejects_suite_skip_and_disable_counts(self):
        for attribute in ("skipped", "disabled"):
            with self.subTest(attribute=attribute):
                root, _ = report()
                root.set(attribute, "1")
                with self.assertRaisesRegex(ValueError, "Skipped or disabled tests"):
                    verify_results(root)

    def test_rejects_google_test_skip_and_disable_output(self):
        for message in ("[  SKIPPED ] Geometry.Optional", "YOU HAVE 2 DISABLED TESTS",
                        "[ RUN ] Geometry.DISABLED_Optional"):
            with self.subTest(message=message):
                root, _ = report("[==========] Running 3 tests from 1 test suite.\n" + message)
                with self.assertRaisesRegex(ValueError, "Skipped or disabled GoogleTest"):
                    verify_results(root)

    def test_rejects_device_specific_backend_skips(self):
        root, _ = report("[==========] Running 3 tests from 1 test suite.\n"
                         "  five cube stages: skipped on the Apple Paravirtual device\n")
        with self.assertRaisesRegex(ValueError, "Skipped backend checks"):
            verify_results(root)


if __name__ == "__main__":
    unittest.main()
