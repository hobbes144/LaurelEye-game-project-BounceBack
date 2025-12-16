# code_analysis_src/metrics.py

import subprocess
from collections import defaultdict
from typing import List, Dict
from code_analysis_src.classifiers import categorize_line_by_ext, LineCategory, is_meaningful_code
from code_analysis_src.git_collect import commit_timestamp_iso, file_ext, read_file_at_commit


def analyze_commits(root: str,
                    commits: List[Dict],
                    include_exts: List[str] = None,
                    exclude_paths: List[str] = None,
                    include_paths: List[str] = None) -> Dict:
    """
    Analyze commits to produce per-author, per-week, per-file stats and fairness metrics.
    Also computes meaningful LOC per author (blame-based at HEAD).
    """
    print(f"[INFO] Starting analysis of {len(commits)} commits...")

    per_author = defaultdict(lambda: {"effort": 0, "code": 0, "comment": 0, "whitespace": 0})
    per_author_week = defaultdict(lambda: defaultdict(int))
    per_file = defaultdict(lambda: {"effort": 0, "code": 0, "comment": 0, "whitespace": 0})

    for idx, commit in enumerate(commits, 1):
        print(f"[INFO] Processing commit {idx}/{len(commits)}: {commit['hash']} by {commit['author_name']}")
        author = commit["author_name"]
        ts = commit_timestamp_iso(commit)
        week = ts.strftime("%Y-W%U")

        for f in commit["files"]:
            path = f["path"]
            ext = file_ext(path)

            # Only include files in allowed directories
            if include_paths and not any(path.startswith(p) for p in include_paths):
                continue
            if include_exts and ext not in include_exts:
                continue
            if exclude_paths and any(path.startswith(p) for p in exclude_paths):
                continue

            effort = f["insertions"] + f["deletions"]
            per_author[author]["effort"] += effort
            per_author_week[author][week] += effort
            per_file[path]["effort"] += effort

            # Estimate code/comment/whitespace proportions
            content = read_file_at_commit(root, commit["hash"], path)
            if content:
                lines = content.splitlines()
                counts = {LineCategory.CODE: 0,
                          LineCategory.COMMENT: 0,
                          LineCategory.WHITESPACE: 0}
                for l in lines:
                    cat = categorize_line_by_ext(l, ext)
                    counts[cat] += 1
                total = sum(counts.values())
                if total > 0:
                    code_ratio = counts[LineCategory.CODE] / total
                    comment_ratio = counts[LineCategory.COMMENT] / total
                    ws_ratio = counts[LineCategory.WHITESPACE] / total
                    per_author[author]["code"] += int(f["insertions"] * code_ratio)
                    per_author[author]["comment"] += int(f["insertions"] * comment_ratio)
                    per_author[author]["whitespace"] += int(f["insertions"] * ws_ratio)
                    per_file[path]["code"] += int(f["insertions"] * code_ratio)
                    per_file[path]["comment"] += int(f["insertions"] * comment_ratio)
                    per_file[path]["whitespace"] += int(f["insertions"] * ws_ratio)

    print("[INFO] Finished commit analysis, computing fairness metrics...")
    fairness = {
        "gini_effort": gini([v["effort"] for v in per_author.values()]),
        "gini_code": gini([v["code"] for v in per_author.values()]),
        "gini_comment": gini([v["comment"] for v in per_author.values()])
    }

    print("[INFO] Running blame-based LOC attribution at HEAD...")
    loc_per_author = compute_loc_per_author(root, commits,
                                            include_paths=include_paths,
                                            include_exts=include_exts,
                                            exclude_paths=exclude_paths)
    print("[INFO] LOC attribution complete.")

    return {
        "per_author": per_author,
        "per_author_week": per_author_week,
        "per_file": per_file,
        "fairness": fairness,
        "loc_per_author": loc_per_author
    }


def gini(values: List[int]) -> float:
    """Compute Gini coefficient for a list of values."""
    values = sorted(v for v in values if v > 0)
    n = len(values)
    if n == 0:
        return 0.0
    cumvals = [sum(values[:i+1]) for i in range(n)]
    total = cumvals[-1]
    gini_sum = sum((i+1) * v for i, v in enumerate(values))
    return (2 * gini_sum) / (n * total) - (n + 1) / n


def _git(root: str, args: List[str]) -> str:
    """Run a git command and return stdout as text (UTF-8)."""
    result = subprocess.run(
        ["git"] + args,
        cwd=root,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace"
    )
    return result.stdout


def compute_loc_per_author(root: str,
                           commits: List[Dict],
                           include_paths: List[str] = None,
                           include_exts: List[str] = None,
                           exclude_paths: List[str] = None) -> Dict[str, Dict[str, int]]:
    """
    Compute meaningful LOC per author at HEAD using git blame.
    Attributes each current line to the author who last touched it.
    Returns total LOC, Lua LOC, and C/C++ LOC.
    """
    loc_per_author: Dict[str, Dict[str, int]] = defaultdict(lambda: {"total": 0, "lua": 0, "cpp": 0})
    if not commits:
        return loc_per_author

    head_hash = commits[0]["hash"]

    print("[INFO] Collecting files at HEAD for blame analysis...")
    ls = _git(root, ["ls-tree", "-r", "--name-only", head_hash])
    files = [p.strip() for p in ls.splitlines() if p.strip()]

    def path_allowed(path: str) -> bool:
        if include_paths and not any(path.startswith(p) for p in include_paths):
            return False
        if exclude_paths and any(path.startswith(p) for p in exclude_paths):
            return False
        ext = file_ext(path)
        if include_exts and ext not in include_exts:
            return False
        return True

    filtered_files = [p for p in files if path_allowed(p)]
    print(f"[INFO] Found {len(filtered_files)} files to blame...")

    for idx, path in enumerate(filtered_files, 1):
        print(f"[INFO] Blaming file {idx}/{len(filtered_files)}: {path}")
        try:
            blame = _git(root, ["blame", "--line-porcelain", "-w", head_hash, "--", path])
        except subprocess.CalledProcessError:
            print(f"[WARN] Failed to blame {path}, skipping.")
            continue

        current_author = None
        ext = file_ext(path).lower()
        for line in blame.splitlines():
            if line.startswith("author "):
                current_author = line[len("author "):].strip()
            elif line.startswith("\t"):
                content = line[1:]
                if current_author and is_meaningful_code(content, ext):
                    loc_per_author[current_author]["total"] += 1
                    if ext in {"lua", ".lua"}:
                        loc_per_author[current_author]["lua"] += 1
                    if ext in {"c", ".c", "cpp", ".cpp", "h", ".h", "hpp", ".hpp"}:
                        loc_per_author[current_author]["cpp"] += 1

    print("[INFO] Blame analysis finished.")
    return loc_per_author
