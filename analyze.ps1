# Get all unique authors
$authors = git log --pretty="%aN" | Sort-Object | Get-Unique

foreach ($author in $authors) {
    Write-Host "`n$author"

    # Initialize counters
    $add = 0
    $del = 0

    # Get line-level stats for this author
    git log --author="$author" --pretty=tformat: --numstat |
        Where-Object { $_ -match "^\d+\s+\d+" } |
        ForEach-Object {
            $parts = $_ -split "\s+"
            $add = $add + [int]$parts[0]
            $del = $del + [int]$parts[1]
        }

    $net = $add - $del
    Write-Host "  Added:  $add"
    Write-Host "  Deleted: $del"
    Write-Host "  Net:     $net"
}
