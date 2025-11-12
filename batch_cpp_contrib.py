#!/usr/bin/env python3
"""
batch_cpp_contrib.py

Runs git_cpp_contrib.py for every UNIQUE EMAIL in the repo.
- Clears user-contributions/ first
- Safe filenames: if exists, uses .2.report, .3.report, etc.
- Output: ./user-contributions/<email-username>.report
"""

import subprocess
import sys
import re
import shutil
from pathlib import Path
from typing import List

# ----------------------------------------------------------------------
# Config
# ----------------------------------------------------------------------
CONTRIB_SCRIPT = "git_cpp_contrib.py"
OUTPUT_DIR = Path("user-contributions")
EXCLUDE_DIRS = []  # e.g. ["external", "vendor"]

# ----------------------------------------------------------------------
def run_cmd(cmd: List[str]) -> str:
    try:
        cp = subprocess.run(
            cmd, cwd=Path.cwd(), capture_output=True, text=True, check=True
        )
        return cp.stdout
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Command failed: {' '.join(cmd)}\n{e.stderr.strip()}")

def get_all_emails() -> List[str]:
    """Return list of unique author emails."""
    out = run_cmd(["git", "log", "--all", "--format=%aE"])
    emails = {line.strip() for line in out.splitlines() if line.strip()}
    return sorted(emails)

def sanitize_filename(email: str) -> str:
    """Convert email to safe base filename: username@domain → username"""
    local_part = email.split('@')[0]
    safe = re.sub(r'[^a-zA-Z0-9._+-]', '_', local_part)
    return safe or "unknown"

def get_unique_path(base_path: Path) -> Path:
    """Return base_path, or base_path.with_suffix('.2.report'), etc."""
    if not base_path.exists():
        return base_path
    i = 2
    while True:
        new_path = base_path.parent / f"{base_path.stem}.{i}{base_path.suffix}"
        if not new_path.exists():
            return new_path
        i += 1

def main() -> None:
    # === Clear output directory ===
    if OUTPUT_DIR.exists():
        print(f"Clearing existing directory: {OUTPUT_DIR}")
        shutil.rmtree(OUTPUT_DIR)
    OUTPUT_DIR.mkdir(exist_ok=True)

    # === Get authors ===
    try:
        emails = get_all_emails()
    except RuntimeError as e:
        print(f"Error: {e}")
        sys.exit(1)

    if not emails:
        print("No authors found in repository.")
        return

    print(f"Found {len(emails)} unique email(s). Generating reports...")

    base_cmd = [sys.executable, CONTRIB_SCRIPT]
    if EXCLUDE_DIRS:
        for d in EXCLUDE_DIRS:
            base_cmd.extend(["--exclude-dir", d])

    success = 0
    for email in emails:
        cmd = base_cmd + [email]
        try:
            output = run_cmd(cmd)
        except RuntimeError as e:
            print(f"Failed for {email}: {e}")
            continue

        safe_name = sanitize_filename(email)
        report_path = OUTPUT_DIR / f"{safe_name}.report"
        final_path = get_unique_path(report_path)

        try:
            final_path.write_text(output, encoding="utf-8")
            if final_path != report_path:
                print(f"  -> {final_path} (name conflict)")
            else:
                print(f"  -> {final_path}")
            success += 1
        except Exception as e:
            print(f"  !! Failed to write {final_path}: {e}")

    print(f"\nDone! {success}/{len(emails)} reports generated in '{OUTPUT_DIR}/'")

if __name__ == "__main__":
    main()
