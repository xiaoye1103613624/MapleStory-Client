$ErrorActionPreference = 'Continue'
$env:REA_ANALYSIS_PROVIDER = 'ida'
$env:REA_IDA_MCP_CONFIG = 'C:\Users\11036\.rea\ida-mcp.json'
$exe = 'E:\MXD\BeiDou-ClientV17.7\BeiDou.exe'
$rea = 'D:\software\nodejs-24\rea.cmd'
$out = 'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence\r17'
if (-not (Test-Path $out)) { New-Item -ItemType Directory -Path $out -Force | Out-Null }

# P0/P1 known EAs from r14/r15/r16
$targets = @(
  @{ name = 'StatusBar_8D01B2'; ea = '0x8D01B2' },
  @{ name = 'DrawWorldItems_624E13'; ea = '0x624E13' },
  @{ name = 'PartySearch_Alliance_49D0B6'; ea = '0x49D0B6' },
  @{ name = 'ViciousHammer_82A9BC'; ea = '0x82A9BC' },
  @{ name = 'VegaSpell_82BB83'; ea = '0x82BB83' },
  @{ name = 'CashGachapon_999E4D'; ea = '0x999E4D' },
  @{ name = 'Quest_83EA05'; ea = '0x83EA05' },
  @{ name = 'UserList_917546'; ea = '0x917546' },
  @{ name = 'ToolTip_8E49B5'; ea = '0x8E49B5' },
  @{ name = 'PartyRace_56BD3C'; ea = '0x56BD3C' },
  @{ name = 'DualMobGauge_56C9B7'; ea = '0x56C9B7' },
  @{ name = 'Maker_822350'; ea = '0x822350' }
)

foreach ($t in $targets) {
  $jsonPath = Join-Path $out ("rea_decompile_{0}.json" -f $t.name)
  $txtPath = Join-Path $out ("rea_decompile_{0}.txt" -f $t.name)
  Write-Host ("DECOMPILE {0} @ {1}" -f $t.name, $t.ea)
  $raw = & $rea decompile $exe $t.ea --provider ida --json 2>&1 | Out-String
  Set-Content -Path $jsonPath -Value $raw -Encoding UTF8

  # Extract pseudocode from observations even if schema unreadable
  $pseudo = New-Object System.Collections.Generic.List[string]
  $pseudo.Add(("; REA decompile {0} @ {1}" -f $t.name, $t.ea))
  $pseudo.Add(("; extracted {0}" -f (Get-Date -Format o)))
  $pseudo.Add('')

  # Common patterns: "code", "pseudocode", "decompiled", observations[].text
  $patterns = @(
    '(?s)"(?:code|pseudocode|decompiled|text|content)"\s*:\s*"(.*?)"(?=\s*[,}])',
    '(?s)observations.*?pseudocode'
  )
  $extracted = $false
  try {
    # Try to pull large string blobs that look like C
    $cm = [regex]::Matches($raw, '(?s)(?:void|int|char|BOOL|__int|sub_)[^\n]{0,80}\{')
    if ($cm.Count -gt 0) {
      # unescape common JSON sequences in chunks containing function-like text
      $blob = $raw
      # Find longest \"...\" that contains sub_ or CreateCtrl-like markers
      $sm = [regex]::Matches($raw, '(?s)"((?:\\.|[^"\\]){200,})"')
      $best = $null
      foreach ($m in $sm) {
        $s = $m.Groups[1].Value
        if ($s -match 'sub_|CreateCtrl|UIWindow|goto |if \(') {
          if (-not $best -or $s.Length -gt $best.Length) { $best = $s }
        }
      }
      if ($best) {
        $decoded = $best -replace '\\n', "`n" -replace '\\t', "`t" -replace '\\r', '' -replace '\\"', '"' -replace '\\\\', '\'
        $pseudo.Add($decoded)
        $extracted = $true
      }
    }
  } catch {}

  if (-not $extracted) {
    # Fallback: dump truncated raw for manual review
    $pseudo.Add('; WARN: could not cleanly extract pseudocode; raw JSON truncated below')
    $pseudo.Add(($raw.Substring(0, [Math]::Min(8000, $raw.Length))))
  }

  Set-Content -Path $txtPath -Value ($pseudo -join "`n") -Encoding UTF8
}

Write-Host 'DONE batch decompile'
