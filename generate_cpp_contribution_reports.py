#!/usr/bin/env python3
"""
generate_cpp_contribution_reports.py

Automatically generates C++ contribution reports for multiple students
using the git_cpp_contrib.py script (supports multiple email aliases per person).
"""

import subprocess
import sys
from pathlib import Path
from typing import List, Dict, Tuple
import shutil

# ----------------------------------------------------------------------
# Configuration: Add or modify students here
# ----------------------------------------------------------------------
STUDENTS: List[Dict[str, Tuple[str, List[str]]]] = [
    {
        "name": "Jacob Wilburn",
        "emails": [
            "jacob.wilburn@digipen.edu"
        ]
    },
    {
        "name": "Nick Shaw",
        "emails": [
            "nsshaw2@asu.edu",
            "77174374+hobbes144@users.noreply.github.com"
        ]
    },
    {
        "name": "Anish Murthy",
        "emails": [
            "anish.murthy@digipen.edu",
            "anish.murthy.dev@gmail.com"
        ]
    },
    {
        "name": "Daoming Wang",
        "emails": [
            "80877899+KeitherJinn@users.noreply.github.com"
        ]
    },
    {
        "name": "Alex Humphries",
        "emails": [
            "alex.humphries@digipen.edu"
        ]
    },
    # Add more students here easily!
]

# ----------------------------------------------------------------------
# Paths & Settings
# ----------------------------------------------------------------------
OUTPUT_DIR = Path("user-contributions/C++")
CPP_CONTRIB_SCRIPT = Path("./git_cpp_contrib.py")

# Create output directory
if OUTPUT_DIR.exists():
    shutil.rmtree(OUTPUT_DIR)
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

def run_report(name: str, emails: List[str]) -> None:
    """Run git_cpp_contrib.py for one student and save output."""
    safe_name = "".join(c if c.isalnum() or c in " ._-()" else "_" for c in name)
    output_file = OUTPUT_DIR / f"{safe_name}.report.txt"

    print(f"Generating report for {name} → {output_file.name}")

    try:
        # Build command: py -3 git_cpp_contrib.py email1 email2 ...
        cmd = [
            "py" if sys.platform.startswith("win") else "python3",
            str(CPP_CONTRIB_SCRIPT)
        ] + emails

        # Run and capture output
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            encoding="utf-8",
            check=True
        )

        # Write to file
        output_file.write_text(result.stdout, encoding="utf-8")
        print(f"   Success: Saved to {output_file}")

    except subprocess.CalledProcessError as e:
        print(f"   Failed: git_cpp_contrib.py exited with code {e.returncode}")
        print(f"   STDERR: {e.stderr.strip()}")
        # Still save whatever output we got for debugging
        debug_file = OUTPUT_DIR / f"{safe_name}.ERROR.txt"
        debug_file.write_text(f"COMMAND: {' '.join(cmd)}\n\nSTDOUT:\n{e.stdout}\n\nSTDERR:\n{e.stderr}", encoding="utf-8")
        print(f"   Error log saved to {debug_file.name}")
    except Exception as e:
        print(f"   Unexpected error for {name}: {e}")

def main() -> None:
    print("Starting C++ contribution report generation...\n")
    print(f"Output directory: {OUTPUT_DIR.resolve()}\n")

    for student in STUDENTS:
        run_report(student["name"], student["emails"])
        print()  # blank line between students

    print("All reports generated!")

if __name__ == "__main__":
    main()
