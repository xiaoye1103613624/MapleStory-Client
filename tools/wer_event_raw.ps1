$ErrorActionPreference = 'SilentlyContinue'
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$out = 'E:\project\MapleStory-Client\docs\features\s9-protocol-align\evidence\crash\wer-event-1000-raw.txt'
$ev = Get-WinEvent -FilterHashtable @{ LogName='Application'; Id=1000; StartTime=(Get-Date).AddHours(-3) } |
    Where-Object { $_.Message -match 'MapleStory' }
$lines = @()
foreach ($e in $ev) {
    $lines += ('==== ' + $e.TimeCreated.ToString('yyyy-MM-dd HH:mm:ss'))
    $lines += ($e.Message -split "`r?`n")
}
$lines -join "`r`n" | Set-Content -Path $out -Encoding UTF8
Write-Output ('wrote ' + $out + ' events=' + ($ev | Measure-Object).Count)
