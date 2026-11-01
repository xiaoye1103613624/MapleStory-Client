$ErrorActionPreference = 'Continue'
$out = 'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence\r17'
$raw = Get-Content (Join-Path $out 'r17_rea_search_summary.json') -Raw
$s = $raw | ConvertFrom-Json

$rows = @()
foreach ($p in $s.PSObject.Properties) {
  $c = 0
  if ($null -ne $p.Value.count) { $c = [int]$p.Value.count }
  $samp = ''
  if ($p.Value.samples -and $p.Value.samples.Count -gt 0) {
    $first = $p.Value.samples[0]
    $samp = ("{0}@{1}" -f $first.value, $first.address)
  }
  $rows += [PSCustomObject]@{ kw = $p.Name; count = $c; sample = $samp }
}

$rows | Sort-Object count -Descending | Format-Table -AutoSize | Out-String -Width 200 | Set-Content (Join-Path $out '_hit_table.txt') -Encoding UTF8

$hit = ($rows | Where-Object { $_.count -gt 0 }).Count
$zero = ($rows | Where-Object { $_.count -eq 0 }).Count
"hit=$hit zero=$zero total=$($rows.Count)" | Set-Content (Join-Path $out '_hit_stats.txt') -Encoding UTF8

# Also re-parse individual JSON files for better counts (summary may undercount)
$better = @{}
Get-ChildItem $out -Filter 'rea_search_*.json' | ForEach-Object {
  $name = $_.BaseName -replace '^rea_search_',''
  $txt = Get-Content $_.FullName -Raw
  $addrs = [regex]::Matches($txt, '"address"\s*:\s*"(0x[0-9a-fA-F]+)"')
  $vals = [regex]::Matches($txt, '"value"\s*:\s*"((?:\\.|[^"\\])*)"')
  $samples = @()
  for ($i = 0; $i -lt [Math]::Min(3, $addrs.Count); $i++) {
    $v = if ($i -lt $vals.Count) { $vals[$i].Groups[1].Value } else { '' }
    $samples += @{ value = $v; address = $addrs[$i].Groups[1].Value }
  }
  $better[$name] = @{ count = $addrs.Count; samples = $samples }
}
$better | ConvertTo-Json -Depth 6 | Set-Content (Join-Path $out 'r17_rea_search_summary_recount.json') -Encoding UTF8
$bh = ($better.GetEnumerator() | Where-Object { $_.Value.count -gt 0 }).Count
$bz = ($better.GetEnumerator() | Where-Object { $_.Value.count -eq 0 }).Count
"recount hit=$bh zero=$bz total=$($better.Count)" | Add-Content (Join-Path $out '_hit_stats.txt') -Encoding UTF8

# Decompile extract quality
Get-ChildItem $out -Filter 'rea_decompile_*.txt' | ForEach-Object {
  $t = Get-Content $_.FullName -Raw
  $ok = ($t -notmatch 'WARN: could not') -and ($t.Length -gt 500)
  $hasCreate = $t -match 'CreateCtrl|CreateButton|UIWindow|gauge|Bt'
  "{0}: len={1} ok={2} hasUI={3}" -f $_.Name, $t.Length, $ok, $hasCreate
} | Set-Content (Join-Path $out '_decompile_quality.txt') -Encoding UTF8

Get-Content (Join-Path $out '_hit_stats.txt')
Get-Content (Join-Path $out '_decompile_quality.txt')
Get-Content (Join-Path $out '_hit_table.txt') | Select-Object -First 80
