$roots = @("$env:APPDATA", "$env:LOCALAPPDATA", "C:\Users\11036", "D:\", "E:\")
$cut = Get-Date "2026-10-24"
foreach ($r in $roots) {
    if (-not (Test-Path $r)) { continue }
    Get-ChildItem -Path $r -Recurse -Filter *.png -ErrorAction SilentlyContinue |
        Where-Object { $_.LastWriteTime -gt $cut -and $_.Name -like "XiaoYe*" } |
        Select-Object -First 12 FullName, LastWriteTime, Length
}
