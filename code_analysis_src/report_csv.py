# code_analysis_src/report_csv.py

import csv
from typing import Dict

def write_per_author_csv(path: str, data: Dict):
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["author", "effort", "code_inserts_est", "comment_inserts_est", "whitespace_inserts_est"])
        for author, v in data["per_author"].items():
            w.writerow([author, v["effort"], v["code"], v["comment"], v["whitespace"]])

def write_per_author_week_csv(path: str, data: Dict):
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["author", "week", "effort"])
        for author, weeks in data["per_author_week"].items():
            for week, eff in weeks.items():
                w.writerow([author, week, eff])

def write_per_file_csv(path: str, data: Dict):
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["file", "effort", "code_inserts_est", "comment_inserts_est", "whitespace_inserts_est"])
        for file, v in data["per_file"].items():
            w.writerow([file, v["effort"], v["code"], v["comment"], v["whitespace"]])

def write_fairness_csv(path: str, data: Dict):
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["metric", "value"])
        for k, v in data["fairness"].items():
            w.writerow([k, v])

def write_loc_per_author_csv(path: str, data: dict):
    """
    Write LOC per author, including total, Lua, and C/C++ LOC.
    """
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["Author", "Total LOC", "Lua LOC", "C/C++ LOC"])
        for a, locs in data["loc_per_author"].items():
            total = locs.get("total", 0)
