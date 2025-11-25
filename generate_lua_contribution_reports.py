#!/usr/bin/env python3
"""
generate_lua_contribution_reports.py

Automatically generates Lua contribution reports for multiple students
using git_lua_contrib.py (supports multiple email aliases per person).
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
OUTPUT_DIR = Path("user-contributions/lua")        # ← Only change: lowercase 'lua'
LUA_CONTRIB_SCRIPT = Path("./git_lua_contrib.py")  # ← Points to your Lua script

# Clean and recreate output directory
if OUTPUT_DIR.exists():
    shutil.rmtree(OUTPUT_DIR)
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

def run_report(name: str, emails: List[str]) -> None:
    """Run git_lua_contrib.py for one student and save output."""
    safe_name = "".join(c if c.isalnum() or c in " ._-()" else "_" for c in name)
    output_file = OUTPUT_DIR / f"{safe_name}.report.txt"

    print(f"Generating Lua report for {name} → {output_file.name}")

    try:
        cmd = [
            "py" if sys.platform.startswith("win") else "python3",
            str(LUA_CONTRIB_SCRIPT)
        ] + emails

        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            encoding="utf-8",
            check=True
        )

        output_file.write_text(result.stdout, encoding="utf-8")
        print(f"   Success: Saved to {output_file}")

    except subprocess.CalledProcessError as e:
        print(f"   Failed: git_lua_contrib.py failed (code {e.returncode})")
        debug_file = OUTPUT_DIR / f"{safe_name}.ERROR.txt"
        debug_file.write_text(
            f"COMMAND: {' '.join(cmd)}\n\n"
            f"STDOUT:\n{e.stdout}\n\n"
            f"STDERR:\n{e.stderr}",
            encoding="utf-8"
        )
        print(f"   Error log saved to {debug_file.name}")
    except Exception as e:
        print(f"   Unexpected error for {name}: {e}")

def main() -> None:
    print("Starting Lua contribution report generation...\n")
    print(f"Output directory: {OUTPUT_DIR.resolve()}\n")

    for student in STUDENTS:
        run_report(student["name"], student["emails"])
        print()  # blank line

    print("All Lua reports generated successfully!")

if __name__ == "__main__":
    main()
    