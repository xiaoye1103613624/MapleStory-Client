$ErrorActionPreference = 'SilentlyContinue'
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8
$start = (Get-Date).AddDays(-14)
$out = 'E:\project\MapleStory-Client\docs\features\s9-protocol-align\evidence\crash\wer-maplestory.txt'
New-Item -ItemType Directory -Force -Path (Split-Path $out) | Out-Null
$ev = Get-WinEvent -FilterHashtable @{ LogName = 'Application'; StartTime = $start } |
    Where-Object { ($_.Id -eq 1000 -or $_.Id -eq 1001) -and $_.Message -match 'MapleStory' }
$lines = @()
$lines += 'MapleStory crash events (14 days): ' + ($ev | Measure-Object).Count
foreach ($e in $ev) {
    $m = $e.Message
    $sig = @()
    foreach ($k in 'P1', 'P2', 'P3', 'P4', 'P5', 'P6', 'P7', 'P8') {
        if ($m -match ("(?m)^" + $k + ":\s*(\S*)")) { $sig += ($k + '=' + $Matches[1]) }
    }
    $appErr = @()
    if ($m -match '(?m)^.*MapleStory\.exe.*$') { $appErr += 'app-line' }
    if ($m -match '0x([0-9a-fA-F]{8})') { $appErr += ('hex1=' + $Matches[1]) }
    $matches2 = [regex]::Matches($m, '0xc0000005|0x([0-9a-fA-F]{16})')
    foreach ($x in $matches2) { $appErr += $x.Value }
    $lines += ($e.TimeCreated.ToString('yyyy-MM-dd HH:mm:ss') + ' Id=' + $e.Id + ' ' + $e.ProviderName)
    if ($sig.Count -gt 0) { $lines += ('   ' + ($sig -join ' ')) }
    if ($appErr.Count -gt 0) { $lines += ('   ' + (($appErr | Select-Object -Unique) -join ' ')) }
}
$lines -join "`r`n" | Set-Content -Path $out -Encoding UTF8
Write-Output ('wrote ' + $out + '  events=' + ($ev | Measure-Object).Count)
