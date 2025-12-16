# code_analysis_src/classifiers.py

import re
from enum import Enum

class LineCategory(Enum):
    CODE = "code"
    COMMENT = "comment"
    WHITESPACE = "whitespace"

# Simple heuristics for C/C++-like files; extensible per language
CPP_SINGLE_COMMENT = re.compile(r'\s*//')
CPP_BLOCK_COMMENT_START = re.compile(r'\s*/\*')
CPP_BLOCK_COMMENT_END = re.compile(r'.*\*/\s*$')

def is_whitespace(line: str) -> bool:
    return line.strip() == ""

def is_line_comment(line: str) -> bool:
    return bool(CPP_SINGLE_COMMENT.match(line))

def categorize_line(line: str, in_block_comment: bool=False) -> tuple[LineCategory, bool]:
    """
    Categorize a line for diff parsing, tracking block comment state.
    Returns (LineCategory, in_block_comment_state).
    """
    if is_whitespace(line):
        return (LineCategory.WHITESPACE, in_block_comment)
    s = line.lstrip()
    if in_block_comment:
        if CPP_BLOCK_COMMENT_END.search(line):
            return (LineCategory.COMMENT, False)
        return (LineCategory.COMMENT, True)
    if is_line_comment(s):
        return (LineCategory.COMMENT, False)
    if CPP_BLOCK_COMMENT_START.match(s):
        if CPP_BLOCK_COMMENT_END.search(s):
            return (LineCategory.COMMENT, False)
        return (LineCategory.COMMENT, True)
    return (LineCategory.CODE, False)

def categorize_line_by_ext(line: str, ext: str) -> LineCategory:
    stripped = line.strip()
    if not stripped:
        return LineCategory.WHITESPACE
    if ext in {".cpp", ".c", ".h", ".hpp", ".java"}:
        if stripped.startswith("//") or stripped.startswith("/*") or stripped.startswith("*"):
            return LineCategory.COMMENT
    if ext == ".py":
        if stripped.startswith("#"):
            return LineCategory.COMMENT
    if ext == ".lua":
        if stripped.startswith("--"):
            return LineCategory.COMMENT
    return LineCategory.CODE

def is_meaningful_code(line: str, ext: str) -> bool:
    stripped = line.strip()
    if not stripped:
        return False
    if stripped in {"{", "}", ";"}:
        return False
    if ext in {".cpp", ".c", ".h", ".hpp", ".java"}:
        if stripped.startswith("//") or stripped.startswith("/*") or stripped.startswith("*"):
            return False
    if ext == ".py":
        if stripped.startswith("#"):
            return False
    if ext == ".lua":
        if stripped.startswith("--"):
            return False
    return True
