"""Render CTest JUnit results and Cobertura coverage into GitHub job summaries."""
import argparse
import html
import os
from pathlib import Path
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser()
parser.add_argument("kind", choices=["tests", "coverage"])
parser.add_argument("path", type=Path)
args = parser.parse_args()
lines = []
if not args.path.exists():
    lines = ["## Report unavailable", "Build or test execution did not produce the expected report."]
else:
    root = ET.parse(args.path).getroot()
    if args.kind == "tests":
        cases = list(root.iter("testcase"))
        failed = sum(c.find("failure") is not None or c.find("error") is not None for c in cases)
        known = sum(any(p.get("name") == "cmake_labels" and "known_bug" in p.get("value", "").split(";") for p in c.findall("./properties/property")) and c.find("failure") is None and c.find("skipped") is None for c in cases)
        skipped = sum(c.find("skipped") is not None for c in cases)
        lines = ["## Core test results", "Tests labelled `known_bug` are expected failures confirming documented upstream defects; see tests/KNOWN_BUGS.md.", f"**{len(cases) - failed - skipped - known} passed · {known} known defects reproduced · {failed} failed · {skipped} skipped**", "", "| Test | Result | Seconds |", "|---|---|---:|"]
        for case in cases:
            status = "❌ Failed" if case.find("failure") is not None or case.find("error") is not None else "⏭ Skipped" if case.find("skipped") is not None else "✅ Passed"
            if any(p.get("name") == "cmake_labels" and "known_bug" in p.get("value", "").split(";") for p in case.findall("./properties/property")) and case.find("failure") is None and case.find("skipped") is None:
                status = "⚠️ Known defect reproduced"
            name = html.escape(case.get("name", "unknown")).replace("|", "&#124;")
            lines.append(f"| {name} | {status} | {case.get('time', '0')} |")
            for tag in ("failure", "error"):
                detail = case.find(tag)
                if detail is not None:
                    lines.extend(["", f"<details><summary>{name} failure</summary><pre>{html.escape(''.join(detail.itertext()))}</pre></details>", ""])
    else:
        lines = ["## Core coverage", "Coverage measures the compiled modules below; untested core modules remain outside this report.", "", "| Module | Lines | Branches |", "|---|---:|---:|"]
        for entry in root.iter("class"):
            pct = lambda key: f"{float(entry.get(key, '0')) * 100:.1f}%"
            lines.append(f"| {entry.get('filename')} | {pct('line-rate')} | {pct('branch-rate')} |")
        lines.extend(["", "Download the coverage artifact for annotated source and branch details."])
report = "\n".join(lines) + "\n"
if target := os.environ.get("GITHUB_STEP_SUMMARY"):
    with open(target, "a", encoding="utf-8") as stream:
        stream.write(report)
else:
    print(report.encode("ascii", "backslashreplace").decode("ascii"))
