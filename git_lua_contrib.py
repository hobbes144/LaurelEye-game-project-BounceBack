#!/usr/bin/env python3
"""
git_lua_contrib.py

Robust per-file effective Lua LOC contribution report.
- Supports multiple author patterns
- Handles --[[ block comments ]], single-line -- comments
- Skips blank lines, punctuation-only lines (e.g., end, }, ), etc.)
- Excludes vendor dirs by default
- Same clean 100-char table output as your C++ version
"""

import sys
import re
import subprocess
import os
from pathlib import Path
from typing import List, Tuple, Dict, Set
from argparse import ArgumentParser

# ----------------------------------------------------------------------
# Force UTF-8
# ----------------------------------------------------------------------
os.environ["PYTHONIOENCODING"] = "utf-8"
os.environ["LANG"] = "C.UTF-8"

# ----------------------------------------------------------------------
# 1. Lua LOC classification
# ----------------------------------------------------------------------
BLANK_RE = re.compile(r'^\s*$')
PUNCT_ONLY_RE = re.compile(r'^\s*[\{\}\(\)\[\];,]+\s*$')
SINGLE_COMMENT_RE = re.compile(r'^\s*--')
BLOCK_COMMENT_START_RE = re.compile(r'--\[\[')
BLOCK_COMMENT_END_RE = re.compile(r'\]\]')

def is_blank(line: str) -> bool:
    return bool(BLANK_RE.match(line))

def is_punctuation_only(line: str) -> bool:
    return bool(PUNCT_ONLY_RE.match(line.strip()))

def is_single_line_comment(line: str) -> bool:
    return bool(SINGLE_COMMENT_RE.match(line))

def count_effective_lua_loc(lines: List[str]) -> int:
    effective = 0
    in_block_comment = False

    for line in lines:
        raw = line.rstrip('\n')

        if is_blank(raw) or is_punctuation_only(raw):
            continue

        if in_block_comment:
            if BLOCK_COMMENT_END_RE.search(raw):
                in_block_comment = False
            continue

        if is_single_line_comment(raw):
            continue

        if BLOCK_COMMENT_START_RE.search(raw):
            if BLOCK_COMMENT_END_RE.search(raw):
                # --[[ comment ]] on same line → only skip comment part
                before = raw.split('--[[', 1)[0]
                if before.strip():
                    effective += 1
                in_block_comment = False
            else:
                before = raw.split('--[[', 1)[0]
                if before.strip():
                    effective += 1
                in_block_comment = True
            continue

        # Regular code line
        effective += 1

    return effective

# ----------------------------------------------------------------------
# 2. Git helpers (identical logic to C++ version)
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

def get_commits(authors: List[str]) -> List[str]:
    if not authors:
        return []

    author_args = [item for a in authors for item in ["--author", a]]
    try:
        out = run_git([
            "git", "rev-list",
            "--no-merges",
            "--reverse",
            "--all",
            *author_args
        ])
    except RuntimeError:
        print(f"No commits found for authors: {', '.join(authors)}", file=sys.stderr)
        return []

    commits = [line.strip() for line in out.splitlines() if line.strip()]
    seen = set()
    unique = []
    for c in commits:
        if c not in seen:
            seen.add(c)
            unique.append(c)
    return unique

def get_changed_lua_files(commit: str, exclude_dirs: Set[str]) -> List[str]:
    out = run_git(["git", "diff-tree", "--no-commit-id", "--name-only", "-r", commit])
    files = []
    for f in out.splitlines():
        f = f.strip()
        if not f or any(f.startswith(d + '/') or f == d for d in exclude_dirs):
            continue
        if f.endswith('.lua'):
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
    parser = ArgumentParser(description="Lua contribution report – supports multiple author identifiers")
    parser.add_argument(
        "authors",
        nargs="+",
        help="Author name, email, or pattern for 'git log --author='"
    )
    parser.add_argument(
        "--exclude-dir", action="append", default=[],
        help="Directory to ignore (e.g. libs, vendor). Can be repeated."
    )
    args = parser.parse_args()

    default_excludes = {"external", "vendor", "third_party", "libs", "lib", "dependencies"}
    exclude_dirs = default_excludes.union(set(args.exclude_dir))

    author_display = " + ".join(args.authors)
    print(f"Analyzing combined Lua contributions for: {author_display}")
    print(f"Excluded directories: {', '.join(sorted(exclude_dirs))}\n")

    commits = get_commits(args.authors)
    if not commits:
        print("No commits found matching any of the provided author patterns.")
        return

    file_stats: Dict[str, Tuple[int, int]] = {}
    total_added = total_removed = 0

    for commit in commits:
        lua_files = get_changed_lua_files(commit, exclude_dirs)
        for path in lua_files:
            added_lines, removed_lines = get_patch_lines(commit, path)
            a = count_effective_lua_loc(added_lines)
            r = count_effective_lua_loc(removed_lines)
            if path not in file_stats:
                file_stats[path] = (0, 0)
            fa, fr = file_stats[path]
            file_stats[path] = (fa + a, fr + r)
            total_added += a
            total_removed += r

    net_loc = total_added - total_removed

    # === Summary ===
    print("=" * 78)
    print(f"{'LUA CONTRIBUTION SUMMARY':^78}")
    print("=" * 78)
    print(f"{'Contributor(s)':<20}: {author_display}")
    print(f"{'Commits':<20}: {len(commits)}")
    print(f"{'Lua Files Touched':<20}: {len(file_stats)}")
    print(f"{'Effective LOC Added':<20}: {total_added}")
    print(f"{'Effective LOC Removed':<20}: {total_removed}")
    print(f"{'Net Effective LOC':<20}: {net_loc:+}")
    print("=" * 78)

    # === Per-File Details ===
    print(f"\n{'FILE':<100} {'ADDED':>10} {'REMOVED':>10} {'NET':>11}")
    print("-" * 78)
    sorted_files = sorted(
        file_stats.items(),
        key=lambda x: x[1][0] - x[1][1],
        reverse=True
    )
    for filename, (added, removed) in sorted_files:
        net = added - removed
        print(f"{filename:<100} {added:>10} {removed:>10} {net:>+10}")
    print("-" * 78)
    print(f"{'TOTAL':<100} {total_added:>10} {total_removed:>10} {net_loc:>+10}")

if __name__ == "__main__":
    main()
    