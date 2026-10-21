param(
  [string]$Inv = 'C:\Users\11036\AppData\Local\Temp\opencode\ida_ijl15_inventory.json',
  [string]$Names = 'C:\Users\11036\AppData\Local\Temp\opencode\module_names.json',
  [string]$Out = 'C:\Users\11036\AppData\Local\Temp\opencode\r01-module-map.md'
)

$j = Get-Content $Inv -Raw -Encoding UTF8 | ConvertFrom-Json
$map = Get-Content $Names -Raw -Encoding UTF8 | ConvertFrom-Json

$strByEa = @{}
foreach ($s in $j.strings) { $strByEa[[string]$s.ea] = $s.val }
$fnByEa = @{}
foreach ($f in $j.functions) { $fnByEa[[string]$f.ea] = $f.name }

$funcStrs = @{}
foreach ($x in $j.string_xrefs) {
  if ($x.func -eq 0) { continue }
  $k = [string]$x.func
  if (-not $funcStrs.ContainsKey($k)) { $funcStrs[$k] = New-Object System.Collections.Generic.List[string] }
  $v = $strByEa[[string]$x.str_ea]
  if ($v) { $funcStrs[$k].Add($v) }
}

$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine('# ijl15.dll module map (auto-generated)')
[void]$sb.AppendLine('')
[void]$sb.AppendLine("Source inventory: ``$Inv``")
[void]$sb.AppendLine("functions=$($j.function_count) strings=$($j.string_count) string_xrefs=$($j.string_xrefs.Count)")
[void]$sb.AppendLine('')
[void]$sb.AppendLine('Function names are real C++ decorated symbols recovered by IDA, not guesses.')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('| module | key functions (EA / name) | evidence strings (truncated) |')
[void]$sb.AppendLine('|---|---|---|')

foreach ($p in $map.PSObject.Properties) {
  $k = $p.Name
  $disp = $p.Value
  $hits = @()
  foreach ($f in $funcStrs.Keys) {
    $nm = $fnByEa[$f]
    if (-not $nm) { continue }
    if ($nm -match [regex]::Escape($k)) { $hits += $f }
  }
  $hits = $hits | Sort-Object -Unique
  $fnList = @()
  foreach ($h in $hits) { $fnList += ('0x{0:X8} `{1}`' -f [int64]$h, $fnByEa[$h]) }
  if ($fnList.Count -gt 6) { $fnList = $fnList[0..5] + @("... (+$($hits.Count - 6) more)") }

  $ev = @()
  foreach ($h in $hits) {
    foreach ($s in $funcStrs[$h]) {
      if ($s -match '^0x[0-9a-fA-F]{2,4}$' -or $s -match 'UI/' -or $s -match '\.img' -or $s -match '^\[') {
        $ev += $s
      }
    }
  }
  $ev = $ev | Sort-Object -Unique
  if ($ev.Count -gt 8) { $ev = $ev[0..7] }
  $evStr = ($ev | ForEach-Object { $_.Replace('|', '\|') }) -join '<br>'
  if (-not $evStr) { $evStr = '-' }
  $fnStr = $fnList -join '<br>'
  if (-not $fnStr) { $fnStr = '**NOT LOCATED**' }
  [void]$sb.AppendLine("| $disp ($k) | $fnStr | $evStr |")
}

$sb.ToString() | Set-Content $Out -Encoding UTF8
"written: $Out"
