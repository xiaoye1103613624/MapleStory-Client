$ErrorActionPreference = 'Continue'
$env:REA_ANALYSIS_PROVIDER = 'ida'
$env:REA_IDA_MCP_CONFIG = 'C:\Users\11036\.rea\ida-mcp.json'
$exe = 'E:\MXD\BeiDou-ClientV17.7\BeiDou.exe'
$rea = 'D:\software\nodejs-24\rea.cmd'
$out = 'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\evidence\r17'
if (-not (Test-Path $out)) { New-Item -ItemType Directory -Path $out -Force | Out-Null }

$kws = @(
  'VegaSpell','CashGachapon','Family','Memo','StoreBank','RemoteGachapon','Incubator','Ranking',
  'Wedding','Minigame','MonsterCarnival','EnergyBar','SlideMenu','SkillMacro','EnchantSkill',
  'AranSkillGuide','NewYearsCard','TreasureBox','KarmaScissors','ItemProtector','Radio','MapleTV',
  'MapleEvent','DualMobGauge','PartyRace','MonsterKilling','muruengRaid','classMatch','AriantMatch',
  'CharacterClone','createCygnus','Transform','counsel','GMBoard','AdminClaim','SoftKeyboard',
  'FadeYesNo','Megaphone','AvatarMegaphone','ItemMegaphone','ArtMegaphone','Coupon','itemSearch',
  'ExceptionItemSearch','Book','MemberShop','EntrustedShop','PersonalShop','CashTradingRoom',
  'TradingRoom','TemporaryStatView','MobGage','QuestAlarm','GameMenu','ShortCut','ContextMenu',
  'FloatNotice','MultiLine','RpsGame','MinigameTable','IconBase','BetaEdition','CancelRequests',
  'MateMessage','plant','raise','MacroSkill','SkillUp','SkillEx','Title','BtUIClose','Reset',
  'Teleport','Report','Admin','Claim','Macro','QuestIcon','InitialQuiz','SpeedQuiz','UtilDlgEx',
  'WorldMap','KeyConfig','Trunk','Channel','UserInfo','MonsterBook','EventWindow','Shop','Stat',
  'Equip','Item','Skill','Notice','MiniMap','GameOpt','SysOpt','signboard','Loading','BtNPT',
  'chatTarget','charInfo','Delivery','PartySearch','ViciousHammer','Maker','UserList','Quest',
  'ToolTip','Messenger','FamilyTree','Duey','Fredrick','ITC','CashShop','StatusBar','WorldSelect',
  'CharSelect','UILogin','LoginNotice','goPartySearch','goAlliance'
)

$summary = @{}
foreach ($kw in $kws) {
  $safe = ($kw -replace '[^\w]', '_')
  $dest = Join-Path $out ("rea_search_{0}.json" -f $safe)
  Write-Host ("SEARCH {0}" -f $kw)
  $raw = & $rea search $exe $kw --kind strings --provider ida --json 2>&1 | Out-String
  Set-Content -Path $dest -Value $raw -Encoding UTF8
  $count = 0
  $samples = @()
  try {
    $j = $raw | ConvertFrom-Json -ErrorAction Stop
    if ($j.observations) {
      foreach ($o in $j.observations) {
        if ($o.kind -eq 'strings' -or $o.type -eq 'string' -or $o.address) {
          $count++
          if ($samples.Count -lt 5) {
            $samples += @{ value = [string]$o.value; address = [string]$o.address }
          }
        }
      }
    }
    if ($j.results) {
      foreach ($o in $j.results) {
        $count++
        if ($samples.Count -lt 5) {
          $samples += @{ value = [string]$o.value; address = [string]$o.address }
        }
      }
    }
    # fallback: count address-like patterns
    if ($count -eq 0) {
      $m = [regex]::Matches($raw, '"address"\s*:\s*"([^"]+)"')
      $count = $m.Count
      $vm = [regex]::Matches($raw, '"value"\s*:\s*"([^"]*)"')
      for ($i = 0; $i -lt [Math]::Min(5, $m.Count); $i++) {
        $samples += @{ value = $(if ($i -lt $vm.Count) { $vm[$i].Groups[1].Value } else { '' }); address = $m[$i].Groups[1].Value }
      }
    }
  } catch {
    $m = [regex]::Matches($raw, '"address"\s*:\s*"([^"]+)"')
    $count = $m.Count
  }
  $summary[$kw] = @{ count = $count; samples = $samples; file = ("rea_search_{0}.json" -f $safe) }
}

$summary | ConvertTo-Json -Depth 6 | Set-Content -Path (Join-Path $out 'r17_rea_search_summary.json') -Encoding UTF8
Write-Host 'DONE batch search'
Write-Host ("keywords={0}" -f $kws.Count)
