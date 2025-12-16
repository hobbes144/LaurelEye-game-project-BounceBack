# code_analysis_src/git_collect.py

import subprocess
import os
from datetime import datetime
from typing import List, Dict, Optional

def run_git(root: str, args: list[str]) -> str:
    """Run a git command in the given repo root and return stdout as UTF-8."""
    return subprocess.run(
        ["git", "-C", root] + args,
        capture_output=True,
        text=True,
        encoding="utf-8",   # <-- force UTF-8 decoding
        errors="replace"    # <-- replace undecodable chars instead of crashing
    ).stdout

def list_commits_with_numstat(root: str) -> List[str]:
    """
    Return git log with numstat lines.
    Format: commit header lines + numstat lines.
    """
    fmt = "%H|%an|%ae|%ai"
    out = run_git(root, ["log", "--numstat", f"--pretty=format:{fmt}"])
    return out.splitlines()


def parse_numstat_stream(lines: List[str]) -> List[Dict]:
    """
    Parse the combined log/numstat stream into structured commit dictionaries.
    """
    commits: List[Dict] = []
    current: Optional[Dict] = None
    for line in lines:
        # Commit header lines contain 3+ '|' separators
        if "|" in line and line.count("|") >= 3 and not line.strip().startswith("-"):
            parts = line.split("|")
            if len(parts) >= 4:
                if current:
                    commits.append(current)
                current = {
                    "hash": parts[0],
                    "author_name": parts[1],
                    "author_email": parts[2],
                    "author_date": parts[3],
                    "files": []
                }
        else:
            if current is None:
                continue
            fields = line.split("\t")
            if len(fields) == 3:
                ins, dels, path = fields
                try:
                    i = int(ins)
                except ValueError:
                    i = 0
                try:
                    d = int(dels)
                except ValueError:
                    d = 0
                current["files"].append({
                    "insertions": i,
                    "deletions": d,
                    "path": path
                })
    if current:
        commits.append(current)
    return commits


def commit_timestamp_iso(commit: Dict) -> datetime:
    """
    Parse the author_date string from git log.
    Example: '2025-11-24 13:11:39 -0800'
    """
    raw = commit["author_date"]
    try:
        return datetime.strptime(raw, "%Y-%m-%d %H:%M:%S %z")
    except ValueError:
        # Fallback if timezone missing
        return datetime.strptime(raw, "%Y-%m-%d %H:%M:%S")


def read_file_at_commit(root: str, commit_hash: str, path: str) -> Optional[str]:
    """Return file contents at a given commit, or None if not available."""
    try:
        blob = run_git(root, ["show", f"{commit_hash}:{path}"])
        return blob
    except subprocess.CalledProcessError:
        return None


def file_ext(path: str) -> str:
    """Return the file extension (including dot)."""
    _, ext = os.path.splitext(path)
    return ext
