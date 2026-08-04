#!/usr/bin/env python3

"""Validate the step sequence, its links, and the optional-goal invariants.

The single property that makes this repository learn-by-doing is that every
step ends in something you can run and something you should see. That is
checked mechanically here, not left to discipline.
"""

from pathlib import Path
import re
import sys
from urllib.parse import unquote


ROOT = Path(__file__).resolve().parent.parent
ERRORS: list[str] = []
WARNINGS: list[str] = []

SKIPPED_DIRS = {"build-cpu", "build-cuda", ".git", ".venv", "reference"}

# reference/ is an archive of the previous course structure: its files link to
# paths that structure defined and this one deleted, so it is skipped wholesale.
# These two are not archive -- they are the maintained entry points into it, and
# their links must resolve like any other.
CHECKED_REFERENCE_FILES = ["reference/README.md", "reference/resources.md"]

# Sections every step must carry. "跑起来" and "你应该看到" are the load-bearing
# pair: a step without them cannot tell the reader whether they succeeded.
REQUIRED_STEP_SECTIONS = ["你会做出", "跑起来", "你应该看到"]

# An additional goal is only worth anything if it forces the analysis. A vague
# "try optimising this" is exactly what these three sections rule out.
ADDITIONAL_GOAL_MARKER = "Additional goal"
REQUIRED_GOAL_SECTIONS = ["假设", "测什么", "为什么"]

# Keep additional goals a garnish, not a second curriculum.
MAX_GOAL_FRACTION = 0.40


def relative(path: Path) -> str:
    return str(path.relative_to(ROOT))


def has_section(text: str, name: str) -> bool:
    """True when `name` appears as a heading or bold lead-in, not just anywhere.

    A bare substring test is not enough: an additional goal that mentions
    "为什么" in its title and closing line would pass even after its
    「为什么」 section had been deleted.
    """
    return re.search(r"(?:^#{1,4}\s*|\*\*)" + re.escape(name), text, re.MULTILINE) is not None


def split_main_and_goal(source: str) -> tuple[str, str]:
    """Return (main-line text, additional-goal text) for one step file."""
    marker = re.search(r"^#{2,4}\s*.*" + ADDITIONAL_GOAL_MARKER, source, re.MULTILINE)
    if marker is None:
        return source, ""
    return source[: marker.start()], source[marker.start() :]


step_files = sorted(ROOT.glob("steps/phase-*/step-*.md"))
if not step_files:
    ERRORS.append("no step files found under steps/phase-*/")

# ---------------------------------------------------------------- numbering --
by_phase: dict[str, list[int]] = {}
step_numbers: list[int] = []
for step_file in step_files:
    match = re.fullmatch(r"step-(\d{3})\.md", step_file.name)
    if match is None:
        ERRORS.append(f"{relative(step_file)}: name must be step-NNN.md")
        continue
    number = int(match.group(1))
    step_numbers.append(number)
    by_phase.setdefault(step_file.parent.name, []).append(number)

for phase, numbers in sorted(by_phase.items()):
    numbers.sort()
    expected = list(range(numbers[0], numbers[0] + len(numbers)))
    if numbers != expected:
        missing = sorted(set(expected) - set(numbers))
        ERRORS.append(
            f"steps/{phase}: numbering is not contiguous; "
            f"got {numbers[0]}..{numbers[-1]} with gaps at {missing}"
        )

duplicates = {n for n in step_numbers if step_numbers.count(n) > 1}
if duplicates:
    ERRORS.append(f"step numbers reused across phases: {sorted(duplicates)}")

# ------------------------------------------------------- per-step structure --
goal_steps: list[str] = []
parsed: list[tuple[int, Path, str, str]] = []

for step_file in step_files:
    source = step_file.read_text(encoding="utf-8")
    main, goal = split_main_and_goal(source)
    number_match = re.fullmatch(r"step-(\d{3})\.md", step_file.name)
    if number_match:
        parsed.append((int(number_match.group(1)), step_file, main, goal))

    if not re.match(r"^#\s+Step\s+\d{3}\s+—", source):
        ERRORS.append(f"{relative(step_file)}: first line must be '# Step NNN — title'")

    for section in REQUIRED_STEP_SECTIONS:
        if not has_section(main, section):
            ERRORS.append(f"{relative(step_file)}: missing required section 「{section}」")

    # The always-runnable guarantee: a fenced block holding a real command.
    runnable = re.search(r"```(?:bash|sh)\n(.+?)```", main, re.DOTALL)
    if runnable is None:
        ERRORS.append(
            f"{relative(step_file)}: no runnable bash block in the main line; "
            f"every step must end in something you can execute"
        )

    if goal:
        goal_steps.append(relative(step_file))
        for section in REQUIRED_GOAL_SECTIONS:
            if not has_section(goal, section):
                ERRORS.append(
                    f"{relative(step_file)}: additional goal is missing 「{section}」; "
                    f"an optional goal without the analysis is not worth doing"
                )

if step_files and len(goal_steps) > len(step_files) * MAX_GOAL_FRACTION:
    WARNINGS.append(
        f"{len(goal_steps)}/{len(step_files)} steps carry an additional goal; "
        f"above {MAX_GOAL_FRACTION:.0%} they stop reading as optional"
    )

# ------------------------------- main line must not depend on optional work --
# A symbol counts as introduced by an optional goal only if no main line up to
# and including that step already used it -- otherwise generic names such as
# `nbytes` would be flagged everywhere. Any *later* main line that then relies
# on it would break for a reader who skipped the goal.
parsed.sort(key=lambda entry: entry[0])
for index, (number, step_file, _, goal) in enumerate(parsed):
    if not goal:
        continue
    main_so_far = "\n".join(main for n, _, main, _ in parsed if n <= number)
    introduced = {
        symbol
        for symbol in re.findall(r"`([A-Za-z_][A-Za-z0-9_]{2,})`", goal)
        if symbol not in main_so_far
    }
    for later_number, later_file, later_main, _ in parsed[index + 1 :]:
        for symbol in sorted(introduced):
            if re.search(r"`" + re.escape(symbol) + r"`", later_main):
                ERRORS.append(
                    f"{relative(later_file)}: main line references `{symbol}`, which "
                    f"only exists if the optional goal in {relative(step_file)} was done"
                )

# ------------------------------------------------------- STEPS.md agreement --
steps_index = ROOT / "STEPS.md"
if not steps_index.is_file():
    ERRORS.append("missing STEPS.md")
else:
    index_source = steps_index.read_text(encoding="utf-8")
    linked = [
        (int(m.group(1)), m.group(2))
        for m in re.finditer(r"\[(\d{3})\]\((steps/phase-[^)]+/step-(\d{3})\.md)\)", index_source)
    ]
    for label, target in linked:
        if int(Path(target).stem.split("-")[1]) != label:
            ERRORS.append(f"STEPS.md: row {label:03d} links to {target}")
        if not (ROOT / target).is_file():
            ERRORS.append(f"STEPS.md: links to missing step file {target}")

    listed = [label for label, _ in linked]
    if listed != sorted(listed):
        ERRORS.append("STEPS.md: step links are not in ascending order")
    for repeated in {n for n in listed if listed.count(n) > 1}:
        ERRORS.append(f"STEPS.md: step {repeated:03d} listed more than once")

    written = set(step_numbers)
    for number in sorted(written - set(listed)):
        ERRORS.append(f"STEPS.md: step {number:03d} exists but is not listed")

    # 🎯 in the index must match an actual additional goal in the file.
    for line in index_source.splitlines():
        row = re.match(r"\|\s*\[(\d{3})\]\((steps/[^)]+)\)\s*\|(.*)\|", line)
        if row is None:
            continue
        target, marked = ROOT / row.group(2), "🎯" in row.group(3)
        if not target.is_file():
            continue
        has_goal = ADDITIONAL_GOAL_MARKER in target.read_text(encoding="utf-8")
        if marked and not has_goal:
            ERRORS.append(f"STEPS.md: step {row.group(1)} marked 🎯 but has no additional goal")
        if has_goal and not marked:
            ERRORS.append(f"STEPS.md: step {row.group(1)} has an additional goal but is not marked 🎯")

    # Phases that are not written yet declare a range like "028–036" instead of
    # links. Written steps and declared ranges together must cover one
    # contiguous span: a gap means a phase can never be written into it, and an
    # overlap means a number belongs to two phases at once. This is what keeps
    # the arc honest while most of it is still only titles.
    declared_ranges = [
        (int(first), int(last))
        for first, last in re.findall(r"\|\s*(\d{3})[–-](\d{3})\s*\|", index_source)
    ]
    declared_numbers: dict[int, tuple[int, int]] = {}
    for first, last in declared_ranges:
        if first > last:
            ERRORS.append(f"STEPS.md: declared range {first:03d}–{last:03d} runs backwards")
            continue
        for number in range(first, last + 1):
            if number in declared_numbers:
                ERRORS.append(
                    f"STEPS.md: step {number:03d} falls in two declared ranges, "
                    f"{declared_numbers[number][0]:03d}–{declared_numbers[number][1]:03d} "
                    f"and {first:03d}–{last:03d}"
                )
            declared_numbers[number] = (first, last)

    for number in sorted(set(listed) & set(declared_numbers)):
        first, last = declared_numbers[number]
        ERRORS.append(
            f"STEPS.md: step {number:03d} is both written and inside the declared "
            f"range {first:03d}–{last:03d}; remove it from the range"
        )

    covered = set(listed) | set(declared_numbers)
    if covered:
        span = range(min(covered), max(covered) + 1)
        for number in (n for n in span if n not in covered):
            ERRORS.append(
                f"STEPS.md: step {number:03d} is in neither a written step nor a "
                f"declared range, leaving a hole in the arc"
            )

for phase_dir in sorted(ROOT.glob("steps/phase-*")):
    if not (phase_dir / "README.md").is_file():
        ERRORS.append(f"{relative(phase_dir)}: missing README.md")

# ----------------------------------------------------------- link integrity --
markdown_files = [
    path
    for path in ROOT.rglob("*.md")
    if not any(part in SKIPPED_DIRS for part in path.parts)
]
for maintained in CHECKED_REFERENCE_FILES:
    maintained_path = ROOT / maintained
    if maintained_path.is_file():
        markdown_files.append(maintained_path)
    else:
        ERRORS.append(f"missing maintained reference entry point: {maintained}")
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
            WARNINGS.append(f"{relative(markdown_file)}: cannot decode local link {target}")
        if not (markdown_file.parent / target).resolve().exists():
            ERRORS.append(f"{relative(markdown_file)}: broken local link {original_target}")

# ------------------------------------------------------- docs/ reachability --
# docs/gh600-checklist.md was deleted during a restructure and nobody noticed,
# because its only inbound links lived in reference/, which is skipped. An
# archive can keep a file looking referenced while it is actually unreachable
# from the live structure. Every doc must be linked from somewhere live.
live_link_text = "\n".join(
    path.read_text(encoding="utf-8")
    for path in markdown_files
    if path.name != "README.md" or path.parent != ROOT / "docs"
)
for doc in sorted(ROOT.glob("docs/*.md")):
    if re.search(r"]\([^)]*" + re.escape(doc.name), live_link_text):
        continue
    ERRORS.append(
        f"{relative(doc)}: not linked from any live document; it is reachable "
        f"only from reference/, which means deleting it would go unnoticed"
    )

# --------------------------------------------------------------- stale URLs --
STALE_PATTERNS = [
    ("MCP 2025-06-18", r"https://modelcontextprotocol\.io/specification/2025-06-18"),
    ("legacy CUDA guide", r"https://docs\.nvidia\.com/cuda/cuda-c-programming-guide"),
    ("legacy Compute Sanitizer", r"https://docs\.nvidia\.com/compute-sanitizer/ComputeSanitizer"),
    ("removed PyTorch tensor tutorial URL",
     r"docs\.pytorch\.org/tutorials/beginner/basics/tensor_tutorial\.html"),
]
for markdown_file in markdown_files:
    source = markdown_file.read_text(encoding="utf-8")
    for label, pattern in STALE_PATTERNS:
        if re.search(pattern, source):
            ERRORS.append(f"{relative(markdown_file)}: {label}")

# ------------------------------------------------- agent instruction pointers --
AGENT_POINTERS = ["CLAUDE.md", ".github/copilot-instructions.md"]
MAX_POINTER_CHARACTERS = 1200
if not (ROOT / "AGENTS.md").is_file():
    ERRORS.append("missing AGENTS.md: the canonical repository instructions")
for pointer_path in AGENT_POINTERS:
    pointer_file = ROOT / pointer_path
    if not pointer_file.is_file():
        ERRORS.append(f"missing agent instruction pointer: {pointer_path}")
        continue
    pointer_source = pointer_file.read_text(encoding="utf-8")
    if "AGENTS.md" not in pointer_source:
        ERRORS.append(f"{pointer_path}: does not point at AGENTS.md")
    if len(pointer_source) > MAX_POINTER_CHARACTERS:
        ERRORS.append(
            f"{pointer_path}: {len(pointer_source)} characters; it should point "
            f"at AGENTS.md, not restate it"
        )

# ------------------------------------------------------------------ output --
if WARNINGS:
    print(f"Warnings ({len(WARNINGS)}):")
    for warning in WARNINGS:
        print(f"- {warning}")

if ERRORS:
    print(f"Step validation failed ({len(ERRORS)}):", file=sys.stderr)
    for error in ERRORS:
        print(f"- {error}", file=sys.stderr)
    raise SystemExit(1)

print(
    f"Step validation passed: {len(step_files)} steps across {len(by_phase)} phases, "
    f"{len(goal_steps)} additional goals, {len(markdown_files)} Markdown files."
)
