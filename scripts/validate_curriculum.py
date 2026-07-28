#!/usr/bin/env python3

"""Validate the structure and local links of the 48-week curriculum."""

from pathlib import Path
import re
import sys
from urllib.parse import unquote


ROOT = Path(__file__).resolve().parent.parent
ERRORS: list[str] = []
WARNINGS: list[str] = []


def relative(path: Path) -> str:
    return str(path.relative_to(ROOT))


week_files = [
    ROOT / "weeks" / f"week-{week:02d}.md"
    for week in range(1, 49)
]

for week_file in week_files:
    if not week_file.exists():
        ERRORS.append(f"missing week file: {relative(week_file)}")
        continue

    source = week_file.read_text(encoding="utf-8")
    day_matches = list(re.finditer(r"^## Day ([1-6])\b.*$", source, re.MULTILINE))
    actual_days = [int(match.group(1)) for match in day_matches]
    if actual_days != [1, 2, 3, 4, 5, 6]:
        ERRORS.append(
            f"{relative(week_file)}: expected Day 1–6 once and in order; "
            f"got {actual_days}"
        )

    required_patterns = [
        ("Unit", r"\bUnit [1-8]\b"),
        ("prerequisite/remediation", r"先修[\s\S]{0,1000}remediation|硬先修"),
        ("observable objectives", r"Observable [Oo]bjectives"),
        ("terminology", r"术语表"),
        ("required theory", r"必读理论材料|必修理论材料"),
        ("guided questions", r"带问题阅读"),
        ("worked example", r"Worked [Ee]xample"),
        ("cumulative increment", r"累计项目 [Ii]ncrement"),
        ("exit/unlock", r"Exit [Tt]icket[\s\S]{0,500}Unlock|解锁"),
    ]
    for label, pattern in required_patterns:
        if re.search(pattern, source, re.IGNORECASE) is None:
            ERRORS.append(f"{relative(week_file)}: missing {label}")

    feed_matches = re.findall(
        r"^## Optional Feed Card（30[–-]45 分钟",
        source,
        re.MULTILINE,
    )
    if len(feed_matches) != 1:
        ERRORS.append(
            f"{relative(week_file)}: expected exactly one 30–45 minute "
            f"Feed Card; got {len(feed_matches)}"
        )
    expected_feed_path = f"notes/feed/week-{int(week_file.stem[-2:]):02d}.md"
    if expected_feed_path not in source:
        ERRORS.append(
            f"{relative(week_file)}: missing canonical Feed artifact "
            f"{expected_feed_path}"
        )

    must_leave_count = source.count("必须留下")
    if must_leave_count < 6:
        ERRORS.append(
            f"{relative(week_file)}: only {must_leave_count} “必须留下” markers"
        )
    external_links = len(re.findall(r"https?://", source))
    if external_links < 2:
        ERRORS.append(
            f"{relative(week_file)}: only {external_links} direct external links"
        )

    for index, day_match in enumerate(day_matches):
        start = day_match.start()
        end = (
            day_matches[index + 1].start()
            if index + 1 < len(day_matches)
            else len(source)
        )
        section = source[start:end]
        declared = re.search(
            r"（(\d+)(?:[–-](\d+))?\s*分钟(?:\s*/[^）]+)?）",
            section,
        )
        if declared is None:
            ERRORS.append(
                f"{relative(week_file)} Day {index + 1}: "
                "missing declared total minutes"
            )
            continue
        low = int(declared.group(1))
        high = int(declared.group(2) or declared.group(1))
        if low < 90 or high > 120:
            ERRORS.append(
                f"{relative(week_file)} Day {index + 1}: "
                f"declared {low}–{high} minutes"
            )

        task_minutes = [
            int(match.group(1))
            for match in re.finditer(r"\*\*(\d+)\s*分钟(?:[^*]*)\*\*", section)
        ]
        if task_minutes:
            total = sum(task_minutes)
            if total != low or high != low:
                WARNINGS.append(
                    f"{relative(week_file)} Day {index + 1}: "
                    f"task minutes sum {total}, declared {low}–{high}"
                )

for gate_week in [6, 12, 18, 24, 30, 36, 42, 48]:
    gate_file = week_files[gate_week - 1]
    if not gate_file.exists():
        continue
    source = gate_file.read_text(encoding="utf-8")
    gate_checks = [
        ("Knowledge 25", r"Knowledge(?: exam)?[^0-9]{0,40}25"),
        ("Unseen practical 30", r"Unseen practical[^0-9]{0,40}30"),
        ("Cumulative project 30", r"Cumulative project[^0-9]{0,40}30"),
        ("Oral/design defense 15", r"Oral(?:/design)? defense[^0-9]{0,40}15"),
        ("pass threshold", r"(?:≥|>=)\s*80"),
        ("hard blockers", r"blocker"),
    ]
    for label, pattern in gate_checks:
        if re.search(pattern, source, re.IGNORECASE) is None:
            ERRORS.append(f"{relative(gate_file)}: Gate missing {label}")
    if re.search(
        r"(?:45|50|55|60)\s*分钟[^\n]{0,80}Knowledge|"
        r"Knowledge[^\n]{0,80}(?:45|50|55|60)\s*分钟",
        source,
        re.IGNORECASE,
    ) is None:
        ERRORS.append(
            f"{relative(gate_file)}: scored Knowledge needs 45–60 minutes"
        )

markdown_files = [
    path
    for path in ROOT.rglob("*.md")
    if not any(part in {"build-cpu", "build-cuda", ".git"} for part in path.parts)
]
link_pattern = re.compile(r"!?\[[^\]]*]\(([^)\n]+)\)")
for markdown_file in markdown_files:
    source = markdown_file.read_text(encoding="utf-8")
    for match in link_pattern.finditer(source):
        original_target = match.group(1)
        target = original_target.strip()
        if target.startswith("<") and target.endswith(">"):
            target = target[1:-1]
        if re.match(r"^(?:https?:|mailto:|data:|#)", target):
            continue
        target = target.split("#", 1)[0].split("?", 1)[0]
        if not target:
            continue
        try:
            target = unquote(target)
        except ValueError:
            WARNINGS.append(
                f"{relative(markdown_file)}: cannot decode local link {target}"
            )
        resolved = (markdown_file.parent / target).resolve()
        if not resolved.exists():
            ERRORS.append(
                f"{relative(markdown_file)}: broken local link {original_target}"
            )

primary_files = [
    path
    for path in markdown_files
    if not relative(path).startswith("core-modules/")
]
stale_patterns = [
    (
        "MCP 2025-06-18",
        r"https://modelcontextprotocol\.io/specification/2025-06-18",
    ),
    (
        "legacy CUDA guide",
        r"https://docs\.nvidia\.com/cuda/cuda-c-programming-guide",
    ),
    (
        "legacy Compute Sanitizer",
        r"https://docs\.nvidia\.com/compute-sanitizer/ComputeSanitizer",
    ),
    (
        "invalid MCP security path",
        r"modelcontextprotocol\.io/specification/2025-11-25/basic/"
        r"security_best_practices",
    ),
    (
        "removed PyTorch tensor tutorial URL",
        r"docs\.pytorch\.org/tutorials/beginner/basics/tensor_tutorial\.html",
    ),
    (
        "invalid CUDA introduction URL",
        r"cuda-programming-guide/02-basics/"
        r"understanding-cuda-programming-model\.html",
    ),
    (
        "noncanonical diagnostic path",
        r"notes/diagnostic/",
    ),
    (
        "noncanonical Resize starter path",
        r"labs/student/resize/",
    ),
    (
        "noncanonical Feed artifact path",
        r"notes/feeds/",
    ),
]
for primary_file in primary_files:
    source = primary_file.read_text(encoding="utf-8")
    for label, pattern in stale_patterns:
        if re.search(pattern, source):
            ERRORS.append(f"{relative(primary_file)}: {label}")

# Output directories the week files, docs/getting-started.md and
# docs/mastery-gates.md require learners to write into. Week-numbered buckets
# such as artifacts/week-33/ are created on demand and are not checked here.
REQUIRED_DIRS = [
    "notes/feed",
    "notes/weekly",
    "notes/problems",
    "notes/gates",
    "notes/curriculum",
    "notes/concepts",
    "notes/diagnostics",
    "notes/daily",
    "notes/ai-app",
    "notes/templates",
    "labs/ml",
    "labs/resize_starter",
    "artifacts/environment",
]
for required_dir in REQUIRED_DIRS:
    if not (ROOT / required_dir).is_dir():
        ERRORS.append(f"missing required output directory: {required_dir}/")

if WARNINGS:
    print(f"Warnings ({len(WARNINGS)}):")
    for warning in WARNINGS:
        print(f"- {warning}")

if ERRORS:
    print(f"Curriculum validation failed ({len(ERRORS)}):", file=sys.stderr)
    for error in ERRORS:
        print(f"- {error}", file=sys.stderr)
    raise SystemExit(1)

print(
    f"Curriculum validation passed: {len(week_files)} weeks, "
    f"{len(week_files) * 6} Day sections, "
    f"{len(markdown_files)} Markdown files."
)
