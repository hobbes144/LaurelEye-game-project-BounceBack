#!/usr/bin/env python3
"""
git_cpp_contrib.py

Robust per-file effective C++ LOC contribution report.
- UTF-8 safe
- Handles renames, binary files, deleted files
- Skips merge commits
- Excludes vendor dirs
- Your 100-char table format
"""

import sys
import re
import subprocess
import os
from pathlib import Path
from typing import List, Tuple, Dict, Set
from argparse import ArgumentParser

# ----------------------------------------------------------------------
# Force UTF-8 for git subprocesses
# ----------------------------------------------------------------------
os.environ["PYTHONIOENCODING"] = "utf-8"
os.environ["LANG"] = "C.UTF-8"

# ----------------------------------------------------------------------
# 1. LOC classification
# ----------------------------------------------------------------------
PUNCT_RE = re.compile(r'^[ \t]*[{}();,\[\]!~%&*+/=?:.|<>-]*[ \t]*$')
MACRO_OR_DIRECTIVE_RE = re.compile(r'^[ \t]*#')
TEMPLATE_RE = re.compile(r'^[ \t]*template\s*<')
RETURN_RE = re.compile(r'^[ \t]*return\s+')
OPERATOR_RE = re.compile(r'^[ \t]*[a-zA-Z_][\w]*\s*[+*/%&|^!=<>]=')

CPP_COMMENT_RE = re.compile(r'^[ \t]*//')
BLOCK_START_RE = re.compile(r'/\*')
BLOCK_END_RE   = re.compile(r'\*/')

def is_blank(line: str) -> bool:
    return line.strip() == ""

def is_macro_or_directive(line: str) -> bool:
    return bool(MACRO_OR_DIRECTIVE_RE.match(line))

def is_template_line(line: str) -> bool:
    return bool(TEMPLATE_RE.match(line))

def is_return_stmt(line: str) -> bool:
    return bool(RETURN_RE.match(line))

def is_operator_assignment(line: str) -> bool:
    return bool(OPERATOR_RE.match(line))

def is_punctuation_only(line: str) -> bool:
    if (is_macro_or_directive(line) or is_template_line(line) or
        is_return_stmt(line) or is_operator_assignment(line)):
        return False
    return bool(PUNCT_RE.fullmatch(line))

def classify_line(line: str, in_block: bool) -> Tuple[bool, bool]:
    stripped = line.strip()
    if not stripped:
        return False, in_block

    if CPP_COMMENT_RE.match(line):
        return False, in_block

    if in_block:
        if BLOCK_END_RE.search(line):
            pre = line.split('*/', 1)[0]
            if pre.strip():
                return True, False
            return False, False
        return False, True

    if BLOCK_START_RE.search(line):
        before, after = line.split('/*', 1)
        if before.strip():
            return True, True
        if BLOCK_END_RE.search(after):
            return False, False
        return False, True

    return True, in_block

def count_effective_loc(lines: List[str]) -> int:
    effective = 0
    in_block = False
    for raw in lines:
        if is_blank(raw) or is_punctuation_only(raw):
            continue
        is_code, in_block = classify_line(raw, in_block)
        if is_code:
            effective += 1
    return effective

# ----------------------------------------------------------------------
# 2. Git helpers
# ----------------------------------------------------------------------
def run_git(cmd: List[str]) -> str:
    try:
        cp = subprocess.run(
            cmd,
            cwd=Path.cwd(),
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=True
        )
        return cp.stdout
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"git {' '.join(cmd)} failed: {e.stderr.strip()}")

def get_commits(author: str) -> List[str]:
    out = run_git([
        "git", "rev-list", "--all", "--author", author,
        "--no-merges", "--reverse"
    ])
    return [line.strip() for line in out.splitlines() if line.strip()]

def get_changed_cpp_files(commit: str, exclude_dirs: Set[str]) -> List[str]:
    out = run_git(["git", "diff-tree", "--no-commit-id", "--name-only", "-r", commit])
    files = []
    for f in out.splitlines():
        f = f.strip()
        if not f or any(f.startswith(d + '/') or f == d for d in exclude_dirs):
            continue
        if f.endswith(('.cpp', '.hpp')):
            files.append(f)
    return files

def get_patch_lines(commit: str, path: str) -> Tuple[List[str], List[str]]:
    try:
        out = run_git(["git", "show", "-p", "--unified=0", f"{commit}", "--", path])
    except RuntimeError:
        return [], []
    if not out.strip():
        return [], []

    added: List[str] = []
    removed: List[str] = []
    for line in out.splitlines():
        if line.startswith('+') and not line.startswith('+++'):
            added.append(line[1:])
        elif line.startswith('-') and not line.startswith('---'):
            removed.append(line[1:])
    return added, removed

# ----------------------------------------------------------------------
# 3. Main
# ----------------------------------------------------------------------
def main() -> None:
    parser = ArgumentParser(description="C++ contribution report per author")
    parser.add_argument("author", help="Author name or email")
    parser.add_argument(
        "--exclude-dir", action="append", default=[],
        help="Directory to ignore (e.g. external). Can be repeated."
    )
    args = parser.parse_args()

    default_excludes = {"external", "vendor", "third_party"}
    exclude_dirs = default_excludes.union(set(args.exclude_dir))

    print(f"Analyzing contributions for: {args.author}")
    print(f"Excluded directories: {', '.join(sorted(exclude_dirs))}\n")

    commits = get_commits(args.author)
    if not commits:
        print("No commits found for this author.")
        return

    file_stats: Dict[str, Tuple[int, int]] = {}
    total_added = total_removed = 0

    for commit in commits:
        cpp_files = get_changed_cpp_files(commit, exclude_dirs)
        for path in cpp_files:
            added_lines, removed_lines = get_patch_lines(commit, path)
            a = count_effective_loc(added_lines)
            r = count_effective_loc(removed_lines)
            if path not in file_stats:
                file_stats[path] = (0, 0)
            fa, fr = file_stats[path]
            file_stats[path] = (fa + a, fr + r)
            total_added += a
            total_removed += r

    net_loc = total_added - total_removed

    # === Summary ===
    print("=" * 78)
    print(f"{'SUMMARY':^78}")
    print("=" * 78)
    print(f"{'Author':<20}: {args.author}")
    print(f"{'Commits':<20}: {len(commits)}")
    print(f"{'C++ Files Touched':<20}: {len(file_stats)}")
    print(f"{'Effective LOC Added':<20}: {total_added}")
    print(f"{'Effective LOC Removed':<20}: {total_removed}")
    print(f"{'Net Effective LOC':<20}: {net_loc:+}")
    print("=" * 78)

    # === Per-File Details (YOUR 100-char format) ===
    print(f"\n{'FILE':<100} {'ADDED':>10} {'REMOVED':>10} {'NET':>11}")
    print("-" * 74)
    sorted_files = sorted(
        file_stats.items(),
        key=lambda x: x[1][0] - x[1][1],
        reverse=True
    )
    for filename, (added, removed) in sorted_files:
        net = added - removed
        print(f"{filename:<100} {added:>10} {removed:>10} {net:>+10}")
    print("-" * 74)
    print(f"{'TOTAL':<100} {total_added:>10} {total_removed:>10} {net_loc:>+10}")

if __name__ == "__main__":
    main()
