$ErrorActionPreference = 'Stop'
$live = 'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
$src  = 'E:\project\MapleStory-Client\build-weather-element-2026-11\MapleStory.exe'

Write-Output '--- preconditions ---'
$holders = Get-CimInstance Win32_Process -Filter "Name='MapleStory.exe'" |
  Where-Object { $_.ExecutablePath -like '*BeiDou-Client-xy*' }
if ($holders) { $holders | Select-Object ProcessId, ExecutablePath | Format-Table -AutoSize; Write-Output 'ABORT: live exe held'; exit 2 }
Write-Output ("live before=" + (Get-FileHash $live -Algorithm SHA256).Hash)

Write-Output '--- backup ---'
$bak = "$live.bak-before-weatherelement-20261123"
Copy-Item -LiteralPath $live -Destination $bak -Force
Write-Output ("backup sha=" + (Get-FileHash $bak -Algorithm SHA256).Hash)

Write-Output '--- overwrite ---'
Copy-Item -LiteralPath $src -Destination $live -Force
$sha1 = (Get-FileHash $live -Algorithm SHA256).Hash
Write-Output ("live immediately=" + $sha1)
Write-Output ("expected         =" + (Get-FileHash $src -Algorithm SHA256).Hash)

Write-Output '--- anti-rollback wait 8s ---'
Start-Sleep -Seconds 8
$sha2 = (Get-FileHash $live -Algorithm SHA256).Hash
Write-Output ("live after 8s    =" + $sha2)
if ($sha2 -ne $sha1) { Write-Output 'WARN: sha changed (rollback?)' } else { Write-Output 'OK: stable' }
