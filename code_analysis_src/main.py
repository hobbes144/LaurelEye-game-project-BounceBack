# code_analysis_src/main.py

import json
import argparse
import os

# ✅ Use package-prefixed imports
from code_analysis_src.git_collect import list_commits_with_numstat, parse_numstat_stream
from code_analysis_src.metrics import analyze_commits
from code_analysis_src.report_csv import (
    write_per_author_csv,
    write_per_author_week_csv,
    write_per_file_csv,
    write_fairness_csv,
    write_loc_per_author_csv   # <-- added import
)
from code_analysis_src.report_html import render_html


def load_config(path: str) -> dict:
    with open(path, "r") as f:
        return json.load(f)


def analyze_repo(root: str, cfg: dict, out_dir: str):
    os.makedirs(out_dir, exist_ok=True)
    lines = list_commits_with_numstat(root)
    commits = parse_numstat_stream(lines)
    data = analyze_commits(
        root,
        commits,
        include_exts=cfg.get("include_exts"),
        exclude_paths=cfg.get("exclude_paths"),
        include_paths=cfg.get("include_paths")
    )

    # Write CSV reports
    write_per_author_csv(os.path.join(out_dir, "per_author.csv"), data)
    write_per_author_week_csv(os.path.join(out_dir, "per_author_week.csv"), data)
    write_per_file_csv(os.path.join(out_dir, "per_file.csv"), data)
    write_fairness_csv(os.path.join(out_dir, "fairness.csv"), data)
    write_loc_per_author_csv(os.path.join(out_dir, "loc_per_author.csv"), data)

    # Write HTML dashboard
    html = render_html(data)
    with open(os.path.join(out_dir, "index.html"), "w", encoding="utf-8") as f:
        f.write(html)


def main():
    ap = argparse.ArgumentParser(description="Student contribution analysis (Git-based).")
    ap.add_argument("-c", "--config", required=True, help="Path to config.json")
    ap.add_argument("-r", "--root", default=".", help="Repository root (or umbrella dir)")
    ap.add_argument("-o", "--out", default="_gitreport", help="Output directory")
    ap.add_argument("--multi", action="store_true", help="Treat root as umbrella dir with per-team subfolders")
    args = ap.parse_args()

    cfg = load_config(args.config)
    if args.multi:
        for name in sorted(os.listdir(args.root)):
            path = os.path.join(args.root, name)
            if not os.path.isdir(path):
                continue
            try:
                analyze_repo(path, cfg, os.path.join(args.out, name))
                print(f"Analyzed {name}")
            except Exception as e:
                print(f"Skip {name}: {e}")
    else:
        analyze_repo(args.root, cfg, args.out)
        print(f"Analyzed {args.root}")


if __name__ == "__main__":
    main()
