# code_analysis_src/report_html.py

from typing import Dict

HTML_TMPL = """<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>Git Contribution Report</title>
<style>
body {{ font-family: system-ui, sans-serif; margin: 2rem; }}
h1, h2 {{ color: #333; }}
table {{ border-collapse: collapse; margin-bottom: 2rem; width: 100%; }}
th, td {{ border: 1px solid #aaa; padding: 0.4rem 0.6rem; text-align: left; }}
th {{ background: #f3f3f3; }}
.section {{ margin-bottom: 2rem; }}
.metric {{ font-weight: bold; }}
</style>
</head>
<body>
<h1>Git Contribution Report</h1>

<div class="section">
<h2>Fairness Metrics</h2>
<ul>
<li><span class="metric">Gini (effort):</span> {gini_effort:.3f}</li>
<li><span class="metric">Gini (code):</span> {gini_code:.3f}</li>
<li><span class="metric">Gini (comment):</span> {gini_comment:.3f}</li>
</ul>
</div>

<div class="section">
<h2>Per Author</h2>
<table>
<thead><tr><th>Author</th><th>Effort</th><th>Code inserts est.</th><th>Comment inserts est.</th><th>Whitespace inserts est.</th></tr></thead>
<tbody>
{rows_authors}
</tbody>
</table>
</div>

<div class="section">
<h2>LOC per Author (meaningful lines)</h2>
<table>
<thead><tr><th>Author</th><th>Total LOC</th><th>Lua LOC</th><th>C/C++ LOC</th></tr></thead>
<tbody>
{rows_loc}
</tbody>
</table>
</div>

<div class="section">
<h2>Weekly Effort (per author)</h2>
<table>
<thead><tr><th>Author</th><th>Week</th><th>Effort</th></tr></thead>
<tbody>
{rows_weeks}
</tbody>
</table>
</div>

<div class="section">
<h2>Per File</h2>
<table>
<thead><tr><th>File</th><th>Effort</th><th>Code inserts est.</th><th>Comment inserts est.</th><th>Whitespace inserts est.</th></tr></thead>
<tbody>
{rows_files}
</tbody>
</table>
</div>

</body>
</html>
"""

def render_html(data: Dict) -> str:
    rows_authors = "\n".join(
        f"<tr><td>{a}</td><td>{v['effort']}</td><td>{v['code']}</td><td>{v['comment']}</td><td>{v['whitespace']}</td></tr>"
        for a, v in data["per_author"].items()
    )
    rows_loc = "\n".join(
        f"<tr><td>{a}</td><td>{locs.get('total',0)}</td><td>{locs.get('lua',0)}</td><td>{locs.get('cpp',0)}</td></tr>"
        for a, locs in data.get("loc_per_author", {}).items()
    )
    rows_weeks = "\n".join(
        f"<tr><td>{a}</td><td>{w}</td><td>{eff}</td></tr>"
        for a, weeks in data["per_author_week"].items()
        for w, eff in weeks.items()
    )
    rows_files = "\n".join(
        f"<tr><td>{p}</td><td>{v['effort']}</td><td>{v['code']}</td><td>{v['comment']}</td><td>{v['whitespace']}</td></tr>"
        for p, v in data["per_file"].items()
    )
    return HTML_TMPL.format(
        gini_effort=data["fairness"]["gini_effort"],
        gini_code=data["fairness"]["gini_code"],
        gini_comment=data["fairness"]["gini_comment"],
        rows_authors=rows_authors,
        rows_loc=rows_loc,
        rows_weeks=rows_weeks,
        rows_files=rows_files
    )
