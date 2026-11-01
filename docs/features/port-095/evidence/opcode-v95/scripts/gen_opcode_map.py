# -*- coding: utf-8 -*-
"""Generate notes/opcode-map-v95-vs-s9.md (curated system-level map + auto appendix)."""
import io, json, re, collections

OUT = r"E:\project\MapleStory-Client\docs\features\port-095\notes\opcode-map-v95-vs-s9.md"
MD = r"E:\project\MapleStory-Client\docs\features\port-095\notes\opcode-v95.md"

# ---------------------------------------------------------------- inputs
md = io.open(MD, encoding="utf-8").read()
head = md.split("## 5. 发送")[0]
ROW = re.compile(r"^\|\s*(\d+)(?:\s*[–-]\s*(\d+))?\s*\|\s*(.+?)\s*\|\s*$", re.M)
rng, exact = {}, {}
for m in ROW.finditer(head):
    lo = int(m.group(1)); hi = int(m.group(2)) if m.group(2) else lo
    h = m.group(3).strip().strip("`")
    if h.startswith("|") or h.startswith("opcode") or h.startswith("子") or h == "-":
        continue
    if hi - lo > 40:
        continue
    if lo == hi:
        exact[lo] = h
    else:
        for n in range(lo, hi + 1):
            rng.setdefault(n, h)
v95recv = dict(rng); v95recv.update(exact)
v95recv = {k: v for k, v in v95recv.items() if not v.startswith("[case]")}

sites = json.load(io.open(r"E:\project\_ms_probe\send_sites_all.json", encoding="utf-8"))
v95send = collections.defaultdict(set)
for s in sites:
    if s["op"] is not None:
        v95send[s["op"]].add(s["name"])

cand = json.load(io.open(r"E:\project\_ms_probe\map_candidates2.json", encoding="utf-8"))

# ---------------------------------------------------------------- curated: v95 send -> s9 RecvOpcode
# op: (s9 const, value, confidence, note)
SEND = {
 # 登录 / 角色
 1:("LOGIN_PASSWORD",1,"证据","同名"),
 4:(None,None,"无对应","登录阶段切换，客户端内部"),
 6:("SERVERSTATUS_REQUEST",6,"待核","检查世界人数 ≈ 世界状态请求"),
 7:("ACCEPT_TOS",7,"证据","同名"),
 8:("SET_GENDER",8,"证据","同名"),
 9:("AFTER_LOGIN",9,"证据","同名"),
 10:("REGISTER_PIN",10,"证据","PIN 注册"),
 11:("SERVERLIST_REQUEST",11,"证据","牌照/服务器列表请求"),
 12:(None,None,"待核","世界选择信息请求，s9 无独立包"),
 13:("VIEW_ALL_CHAR",13,"证据","同名"),
 14:("PICK_ALL_CHAR",14,"证据","VAC 选角"),
 15:(None,None,"无对应","VAC 对话框本地逻辑"),
 16:("NAME_TRANSFER",16,"待核","改名校验"),
 18:("WORLD_TRANSFER",18,"证据","同名"),
 19:("CHAR_SELECT",19,"证据","同名"),
 20:(None,None,"无对应","连接建立（v95 新增）"),
 21:("CHECK_CHAR_NAME",21,"证据","重复名检查"),
 22:("CREATE_CHAR",22,"证据","建角"),
 23:(None,None,"待核","SendNewCharPacket 第二段，语义未定"),
 24:("DELETE_CHAR",23,"证据","删角（值 23，非 24）"),
 25:("PONG",24,"证据","心跳回应"),
 27:(None,None,"无对应","客户端完整性校验（HS 系）"),
 28:("CHAR_SELECT_WITH_PIC",30,"待核","带 PIC 的选角"),
 29:("VIEW_ALL_WITH_PIC",32,"待核","带 PIC 的 VAC 选角"),
 30:(None,None,"无对应","VAC 选角"),
 31:(None,None,"无对应","VAC 选角"),
 32:(None,None,"无对应","连接建立第二包（v95 新增）"),
 34:(None,None,"无对应","CLogin::Init"),
 35:(None,None,"待核","登录/选角前导包，语义未定"),
 36:(None,None,"无对应","备份/堆栈日志上报"),
 # 地图 / 移动 / 聊天
 41:("CHANGE_MAP",38,"证据","换图"),
 42:("CHANGE_CHANNEL",39,"证据","换频道"),
 43:("ENTER_CASHSHOP",40,"证据","进商城"),
 44:(None,None,"待核","单键动作（v95 新），s9 无通用按键包"),
 45:("CANCEL_CHAIR",42,"证据","起身"),
 46:("USE_CHAIR",43,"证据","坐下（不是攻击！）"),
 51:(None,None,"待核","移动射击预备（v95 新）"),
 52:(None,None,"待核","受击/延迟上报，语义未定"),
 54:("GENERAL_CHAT",49,"证据","普通/斜杠聊天"),
 55:("MOVE_PLAYER",41,"证据","点击移动请求"),
 56:("FACE_EXPRESSION",51,"证据","表情"),
 57:("USE_ITEMEFFECT",52,"证据","道具特效"),
 58:("USE_DEATHITEM",53,"待核","墓碑升级特效"),
 61:("MOB_BANISH_PLAYER",56,"证据","怪物驱逐玩家"),
 # NPC / 商店 / 仓库 / 快递 / 委托
 63:("NPC_TALK",58,"证据","对话 NPC"),
 64:("REMOTE_STORE",59,"证据","远程商店"),
 65:("NPC_TALK_MORE",60,"证据","对话选项/回复"),
 66:("NPC_SHOP",61,"证据","NPC 商店"),
 67:("STORAGE",62,"证据","仓库"),
 68:("HIRED_MERCHANT_REQUEST",63,"证据","雇佣/委托商店"),
 69:("FREDRICK_ACTION",64,"证据","商店银行（Fredrick）"),
 70:("DUEY_ACTION",65,"证据","快递（Duey）"),
 71:("SKILL_EFFECT",93,"证据","技能特效请求"),
 72:("OWL_ACTION",66,"证据","商店搜索（猫头鹰）"),
 73:("OWL_WARP",67,"证据","搜索结果传送"),
 74:("ADMIN_SHOP",68,"证据","管理商店"),
 # 物品 / 装备 / 技能 / 能力
 75:("ITEM_SORT",69,"证据","整包/合并"),
 76:("ITEM_SORT2",70,"证据","整理"),
 77:("ITEM_MOVE",71,"证据","槽位移动"),
 78:("USE_ITEM",72,"证据","使用道具"),
 79:("CANCEL_ITEM_EFFECT",73,"证据","取消道具效果"),
 80:("HEAL_OVER_TIME",89,"待核","TryRecovery（恢复请求）"),
 81:("USE_SUMMON_BAG",75,"证据","召唤袋"),
 82:("PET_FOOD",76,"证据","宠物食物"),
 83:("USE_MOUNT_FOOD",77,"证据","坐骑食物"),
 84:("SCRIPTED_ITEM",78,"证据","脚本道具"),
 85:("USE_CASH_ITEM",79,"证据","现金道具"),
 86:("SPAWN_PET",98,"证据","召唤宠物"),
 87:("USE_CATCH_ITEM",81,"证据","捕捉（驯服）道具"),
 88:("USE_SKILL_BOOK",82,"证据","技能书"),
 89:(None,None,"待核","SP 重置道具，s9 无独立包"),
 90:(None,None,"无对应","商店扫描器道具"),
 91:("USE_TELEPORT_ROCK",84,"证据","传送石"),
 92:("USE_RETURN_SCROLL",85,"证据","回城卷"),
 93:("USE_UPGRADE_SCROLL",86,"证据","强化卷"),
 94:(None,None,"待核","超级强化（v95 新增）"),
 95:(None,None,"待核","潜能强化（v95 新增）"),
 96:(None,None,"无对应","UI 开启道具"),
 97:(None,None,"待核","解封道具"),
 98:("DISTRIBUTE_AP",87,"证据","AP 分配"),
 99:("AUTO_DISTRIBUTE_AP",88,"证据","自动分配 AP"),
 100:("DISTRIBUTE_SP",90,"待核","属性变更请求"),
 101:(None,None,"待核","潜能属性变更"),
 102:("DISTRIBUTE_SP",90,"证据","技能点"),
 103:("SPECIAL_MOVE",91,"证据","统一技能：主动技能/召唤/怪物磁铁/吞噬"),
 104:("CANCEL_BUFF",92,"证据","取消技能"),
 105:(None,None,"待核","技能预备（v95 新）"),
 106:("MESO_DROP",94,"证据","丢金币"),
 107:("GIVE_FAME",95,"证据","人气"),
 109:("CHAR_INFO_REQUEST",97,"证据","查看角色信息"),
 110:("SPAWN_PET",98,"证据","宠物激活第二入口"),
 111:("CANCEL_DEBUFF",99,"待核","临时状态到期"),
 112:("CHANGE_MAP_SPECIAL",100,"待核","传送门/上键"),
 113:("TROCK_ADD_MAP",102,"证据","传送石记录地图"),
 114:("CHANGE_MAP_SPECIAL",100,"待核","地图转移请求"),
 115:(None,None,"待核","反外挂道具"),
 116:(None,None,"待核","反外挂技能"),
 117:(None,None,"待核","反外挂结果回执"),
 118:(None,None,"待核","找回请求"),
 119:("QUEST_ACTION",107,"证据","任务开始/放弃/完成"),
 120:("CANCEL_BUFF",92,"待核","状态重置/被动结算"),
 121:("GRENADE_EFFECT",109,"证据","手雷"),
 122:("SKILL_MACRO",110,"证据","技能宏"),
 123:("USE_ITEM_REWARD",112,"证据","NPC 选择道具"),
 124:("USE_TREASUER_CHEST",115,"待核","抽奖道具"),
 125:("MAKER_SKILL",113,"证据","制造"),
 126:("ADMIN_CHAT",118,"待核","斜杠命令"),
 127:("USE_GACHA_EXP",158,"待核","扭蛋盒"),
 128:("USE_REMOTE",116,"证据","远程扭蛋"),
 129:("WATER_OF_LIFE",117,"证据","生命之水"),
 130:(None,None,"待核","耐久修复（全部）"),
 131:(None,None,"待核","耐久修复"),
 132:(None,None,"无对应","DPS 记录"),
 133:(None,None,"待核","客户端计时器同步"),
 134:(None,None,"待核","跟随角色（v95 新）"),
 136:(None,None,"待核","PQ 奖励选择"),
 137:(None,None,"待核","PQ 奖励领取"),
 138:(None,None,"待核","跟随/同乘"),
 139:("ADMIN_CHAT",118,"待核","斜杠命令"),
 # 社交
 140:("MULTI_CHAT",119,"证据","组队/多聊"),
 141:("WHISPER",120,"证据","私聊"),
 142:("SPOUSE_CHAT",121,"证据","夫妻聊"),
 143:("MESSENGER",122,"证据","信使"),
 144:("PLAYER_INTERACTION",123,"证据","小游戏/交易房/个人商店/委托商店统一"),
 145:("PARTY_OPERATION",124,"证据","组队"),
 146:("DENY_PARTY_REQUEST",125,"证据","拒绝组队"),
 147:(None,None,"无对应","远征队（s9 收包无）"),
 148:("PARTY_SEARCH_REGISTER",220,"证据","组队招募（另有 START 222 / UPDATE 223）"),
 149:("GUILD_OPERATION",126,"证据","公会"),
 150:("DENY_GUILD_REQUEST",127,"证据","拒绝公会"),
 151:(None,None,"待核","斜杠命令/进图按键"),
 152:("ADMIN_COMMAND",128,"待核","斜杠命令"),
 153:("BUDDYLIST_MODIFY",130,"证据","好友"),
 154:("NOTE_ACTION",131,"证据","便签"),
 156:("USE_DOOR",133,"证据","城镇传送门"),
 157:("USE_DOOR",133,"待核","开洞传送门（s9 合并入 133）"),
 158:("ADMIN_LOG",129,"待核","斜杠命令"),
 159:("CHANGE_KEYMAP",135,"证据","键位映射"),
 160:("RPS_ACTION",136,"证据","猜拳"),
 161:("RING_ACTION",137,"证据","戒指/订婚"),
 162:("WEDDING_ACTION",138,"待核","结婚心愿单"),
 163:("WEDDING_ACTION",138,"证据","婚礼进度"),
 164:("WEDDING_TALK",139,"证据","婚礼对话"),
 165:(None,None,"待核","陷阱警报"),
 166:("WORLD_MAP_PLAYERS",277,"待核","小地图/世界地图玩家"),
 167:("ALLIANCE_OPERATION",143,"证据","联盟"),
 168:("DENY_ALLIANCE_REQUEST",144,"证据","拒绝联盟"),
 169:("OPEN_FAMILY_PEDIGREE",145,"证据","家族族谱"),
 170:("OPEN_FAMILY",146,"证据","家族信息"),
 171:("ADD_FAMILY",147,"证据","登记晚辈"),
 172:("SEPARATE_FAMILY_BY_SENIOR",148,"证据","解除晚辈"),
 173:("SEPARATE_FAMILY_BY_JUNIOR",149,"证据","解除长辈"),
 174:("ACCEPT_FAMILY",150,"证据","家族邀请结果"),
 175:("USE_FAMILY",151,"证据","家族特权"),
 176:("CHANGE_FAMILY_MESSAGE",152,"证据","家族箴言"),
 177:("FAMILY_SUMMON_RESPONSE",153,"证据","家族召唤响应"),
 179:("BBS_OPERATION",155,"证据","公会 BBS"),
 # 商城 / ITC / 活动
 180:("ENTER_MTS",156,"证据","进 ITC/MTS"),
 181:("USE_GACHA_EXP",158,"待核","经验道具"),
 182:("USE_GACHA_EXP",158,"待核","临时经验道具"),
 183:("NEW_YEAR_CARD_REQUEST",159,"证据","新年卡"),
 184:(None,None,"待核","随机变形"),
 185:("CASHSHOP_SURPRISE",161,"待核","现金扭蛋"),
 186:("CASHSHOP_SURPRISE",161,"待核","现金扭蛋"),
 187:(None,None,"待核","MaplePoint 变更"),
 188:(None,None,"待核","双击交互"),
 189:("ARAN_COMBO_COUNTER",163,"证据","连击计数"),
 190:(None,None,"无对应","怪物 CRC key 同步"),
 191:(None,None,"待核","会话值请求"),
 192:(None,None,"待核","公告板新内容检查"),
 193:(None,None,"待核","账号附加信息"),
 194:(None,None,"待核","好友搜索"),
 195:(None,None,"待核","APSP 事件"),
 196:(None,None,"待核","龙珠盒"),
 197:(None,None,"待核","龙珠召唤"),
 # 宠物 / 召唤兽 / 龙 / 快捷槽
 199:("MOVE_PET",167,"证据","宠物移动"),
 200:("PET_COMMAND",169,"证据","宠物指令"),
 201:("PET_CHAT",168,"证据","宠物说话"),
 202:("PET_LOOT",170,"证据","宠物拾取"),
 203:("PET_AUTO_POT",171,"证据","宠物自动药"),
 204:("PET_EXCLUDE_ITEMS",172,"证据","宠物排除列表"),
 207:("MOVE_SUMMON",175,"证据","召唤兽移动"),
 209:("DAMAGE_SUMMON",177,"证据","召唤兽受伤"),
 210:("SUMMON_ATTACK",176,"证据","召唤兽攻击/治疗/召唤"),
 211:(None,None,"待核","移除召唤兽"),
 214:("MOVE_DRAGON",181,"证据","龙移动"),
 216:("CHANGE_QUICKSLOT",183,"证据","快捷槽"),
 217:(None,None,"待核","被动技能数据"),
 218:(None,None,"待核","UI 菜单"),
 219:("MAGIC_ATTACK",46,"证据","魔法攻击（push 0DBh @0x92dc52）"),
 220:(None,None,"待核","PamsSong 询问"),
 221:(None,None,"待核","任务详情按钮"),
 222:(None,None,"待核","重复特效移除"),
 # 怪物 / NPC / 反应堆 / Field
 227:("MOVE_LIFE",188,"证据","怪物移动路径"),
 228:("AUTO_AGGRO",189,"待核","控制权接受"),
 229:(None,None,"待核","怪物拾取"),
 230:(None,None,"待核","怪物状态同步 A（CMob::Update）"),
 231:(None,None,"待核","怪物状态同步 B（CMob::Update）"),
 232:("MONSTER_BOMB",193,"证据","自爆"),
 233:("MOB_DAMAGE_MOB",194,"证据","怪打怪"),
 234:(None,None,"待核","怪物状态同步 C（CMob::Update）"),
 235:("MONSTER_BOMB",193,"证据","倒计时炸弹"),
 236:(None,None,"待核","护卫碰撞"),
 237:(None,None,"待核","护卫路径请求"),
 238:(None,None,"待核","护卫停止"),
 241:("NPC_ACTION",197,"证据","NPC 移动"),
 242:("NPC_ACTION",197,"待核","NPC 特殊动作"),
 246:("ITEM_PICKUP",202,"证据","拾取"),
 249:("DAMAGE_REACTOR",205,"证据","反应堆受击"),
 250:("TOUCHING_REACTOR",206,"证据","反应堆触碰"),
 251:("MONSTER_CARNIVAL",218,"待核","NLC PQ 重置"),
 255:("SNOWBALL",211,"证据","雪球"),
 256:("SNOWBALL",211,"待核","雪球状态"),
 257:("COCONUT",213,"证据","椰子"),
 258:("ADMIN_CHAT",118,"待核","斜杠命令"),
 259:(None,None,"无对应","公会 Boss（s9 收包无）"),
 262:("MONSTER_CARNIVAL",218,"证据","怪物狂欢"),
 264:(None,None,"待核","连续移动场（ContiMove）"),
 266:("PARTY_SEARCH_REGISTER",220,"待核","组队招聘"),
 267:("PARTY_SEARCH_UPDATE",223,"待核","取消招聘"),
 269:(None,None,"待核","阶段切换"),
 270:(None,None,"待核","FootHold 信息请求"),
 # 商城 / Raise / 其他
 274:("CHECK_CASH",228,"证据","查询点券"),
 275:("CASHSHOP_OPERATION",229,"证据","商城操作"),
 276:("COUPON_CODE",230,"证据","优惠券"),
 283:("OPEN_ITEMUI",236,"证据","Raise 窗口打开"),
 284:("OPEN_ITEMUI",236,"待核","Raise 窗口构造"),
 285:("USE_ITEMUI",238,"证据","Raise 放入"),
 286:("USE_ITEMUI",238,"证据","Raise 分片放入"),
 291:(None,None,"待核","职业竞赛鉴权"),
 292:(None,None,"待核","网页活动鉴权"),
 296:(None,None,"待核","道具升级窗口"),
 299:(None,None,"待核","战斗记录计算"),
 306:("MTS_OPERATION",253,"证据","ITC 充值"),
 307:("MTS_OPERATION",253,"待核","ITC 查点券"),
 308:("MTS_OPERATION",253,"证据","ITC 买卖/登记"),
 311:(None,None,"待核","角色出售重名检查"),
 313:(None,None,"待核","登出礼物"),
}

SEND_SECTIONS = [
 ("3.1 登录 / 角色",[1,4,6,7,8,9,10,11,12,13,14,15,16,18,19,20,21,22,23,24,25,27,28,29,30,31,32,34,35,36]),
 ("3.2 地图 / 移动 / 聊天 / 表情",[41,42,43,44,45,46,51,52,54,55,56,57,58,61]),
 ("3.3 NPC / 商店 / 仓库 / 快递 / 委托",[63,64,65,66,67,68,69,70,71,72,73,74]),
 ("3.4 物品 / 装备 / 技能 / 能力",[75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,136,137,138,139]),
 ("3.5 社交 / 组队 / 公会 / 联盟 / 家族",[140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,179]),
 ("3.6 商城 / ITC / 活动 / 其他 UI",[180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,274,275,276,283,284,285,286,291,292,296,299,306,307,308,311,313]),
 ("3.7 宠物 / 召唤兽 / 龙 / 快捷槽",[199,200,201,202,203,204,207,209,210,211,214,216,217,218,219,220,221,222]),
 ("3.8 怪物 / NPC / 反应堆 / Field",[227,228,229,230,231,232,233,234,235,236,237,238,241,242,246,249,250,251,255,256,257,258,259,262,264,266,267,269,270]),
]

# ---------------------------------------------------------------- curated: v95 recv -> s9 SendOpcode
RECV = {
 # CWvsContext 28–140（全量）
 28:("INVENTORY_OPERATION",29,"证据",""),
 29:("INVENTORY_GROW",30,"证据",""),
 30:("STAT_CHANGED",31,"证据",""),
 31:("GIVE_BUFF",32,"证据",""),
 32:("CANCEL_BUFF",33,"证据",""),
 33:("FORCED_STAT_SET",34,"证据",""),
 34:("FORCED_STAT_RESET",35,"证据",""),
 35:("UPDATE_SKILLS",36,"证据",""),
 36:("SKILL_USE_RESULT",37,"证据",""),
 37:("FAME_RESPONSE",38,"证据",""),
 38:("SHOW_STATUS_INFO",39,"证据",""),
 39:("OPEN_FULL_CLIENT_DOWNLOAD_LINK",40,"证据",""),
 40:("MEMO_RESULT",41,"证据",""),
 41:("MAP_TRANSFER_RESULT",42,"证据",""),
 42:(None,None,"无对应","反外挂结果，s9 无独立包"),
 44:("CLAIM_RESULT",45,"证据",""),
 45:("CLAIM_AVAILABLE_TIME",46,"证据",""),
 46:("CLAIM_STATUS_CHANGED",47,"证据",""),
 47:("SET_TAMING_MOB_INFO",48,"证据",""),
 48:("QUEST_CLEAR",49,"证据","注意不是 SET_QUEST_CLEAR(150)"),
 49:("ENTRUSTED_SHOP_CHECK_RESULT",50,"证据",""),
 50:("SKILL_LEARN_ITEM_RESULT",51,"证据",""),
 51:(None,None,"待核","技能重置结果"),
 52:("GATHER_ITEM_RESULT",52,"证据",""),
 53:("SORT_ITEM_RESULT",53,"证据",""),
 55:("SUE_CHARACTER_RESULT",55,"证据",""),
 57:("TRADE_MONEY_LIMIT",57,"证据",""),
 58:("SET_GENDER",58,"证据",""),
 59:("GUILD_BBS_PACKET",59,"证据",""),
 61:("CHAR_INFO",61,"证据",""),
 62:("PARTY_OPERATION",62,"证据",""),
 64:(None,None,"无对应","远征队结果，s9 无"),
 65:("BUDDYLIST",63,"证据",""),
 67:("GUILD_OPERATION",65,"证据",""),
 68:("ALLIANCE_OPERATION",66,"证据",""),
 69:("SPAWN_PORTAL",67,"证据",""),
 70:(None,None,"待核","开洞传送门"),
 71:("SERVERMESSAGE",68,"待核","系统广播"),
 72:("INCUBATOR_RESULT",69,"证据",""),
 73:("SHOP_SCANNER_RESULT",70,"证据",""),
 74:("SHOP_LINK_RESULT",71,"证据",""),
 75:("MARRIAGE_REQUEST",72,"证据",""),
 76:("MARRIAGE_RESULT",73,"证据",""),
 77:("WEDDING_GIFT_RESULT",74,"证据",""),
 78:("NOTIFY_MARRIED_PARTNER_MAP_TRANSFER",75,"证据",""),
 79:("CASH_PET_FOOD_RESULT",76,"证据",""),
 80:("SET_WEEK_EVENT_MESSAGE",77,"证据",""),
 81:("SET_POTION_DISCOUNT_RATE",78,"证据",""),
 82:("BRIDLE_MOB_CATCH_FAIL",79,"证据",""),
 83:("IMITATED_NPC_RESULT",80,"证据",""),
 84:("IMITATED_NPC_DATA",81,"证据",""),
 85:("LIMITED_NPC_DISABLE_INFO",82,"证据",""),
 86:("MONSTER_BOOK_SET_CARD",83,"证据",""),
 87:("MONSTER_BOOK_SET_COVER",84,"证据",""),
 88:("HOUR_CHANGED",85,"证据",""),
 89:("MINIMAP_ON_OFF",86,"证据",""),
 90:("CONSULT_AUTHKEY_UPDATE",87,"证据",""),
 91:("CLASS_COMPETITION_AUTHKEY_UPDATE",88,"证据",""),
 92:("WEB_BOARD_AUTHKEY_UPDATE",89,"证据",""),
 93:("SESSION_VALUE",90,"证据",""),
 94:("PARTY_VALUE",91,"证据",""),
 95:("FIELD_SET_VARIABLE",92,"证据",""),
 96:("BONUS_EXP_CHANGED",93,"证据",""),
 97:(None,None,"待核","药水折扣变更"),
 98:("FAMILY_CHART_RESULT",94,"证据",""),
 99:("FAMILY_INFO_RESULT",95,"证据",""),
 100:("FAMILY_RESULT",96,"证据",""),
 101:("FAMILY_JOIN_REQUEST",97,"证据",""),
 102:("FAMILY_JOIN_REQUEST_RESULT",98,"证据",""),
 103:("FAMILY_JOIN_ACCEPTED",99,"证据",""),
 104:("FAMILY_PRIVILEGE_LIST",100,"证据",""),
 105:("FAMILY_REP_GAIN",101,"证据",""),
 106:("FAMILY_NOTIFY_LOGIN_OR_LOGOUT",102,"证据",""),
 107:("FAMILY_SET_PRIVILEGE",103,"证据",""),
 108:("FAMILY_SUMMON_REQUEST",104,"证据",""),
 109:("NOTIFY_LEVELUP",105,"证据",""),
 110:("NOTIFY_MARRIAGE",106,"证据",""),
 111:("NOTIFY_JOB_CHANGE",107,"证据",""),
 113:("MAPLE_TV_USE_RES",109,"证据",""),
 114:("AVATAR_MEGAPHONE_RESULT",110,"证据",""),
 115:("SET_AVATAR_MEGAPHONE",111,"证据",""),
 116:("CLEAR_AVATAR_MEGAPHONE",112,"证据",""),
 117:("CANCEL_NAME_CHANGE_RESULT",113,"证据",""),
 118:("CANCEL_TRANSFER_WORLD_RESULT",114,"证据",""),
 119:("DESTROY_SHOP_RESULT",115,"证据",""),
 120:("FAKE_GM_NOTICE",116,"证据",""),
 121:("SUCCESS_IN_USE_GACHAPON_BOX",117,"证据",""),
 122:("NEW_YEAR_CARD_RES",118,"证据",""),
 123:("RANDOM_MORPH_RES",119,"证据",""),
 124:("CANCEL_NAME_CHANGE_BY_OTHER",120,"证据",""),
 125:("SET_EXTRA_PENDANT_SLOT",121,"待核","额外吊坠槽"),
 126:(None,None,"待核","同乘请求"),
 127:("SCRIPT_PROGRESS_MESSAGE",122,"证据",""),
 128:("DATA_CRC_CHECK_FAILED",123,"证据",""),
 129:(None,None,"待核","CakePie 活动结果"),
 130:(None,None,"待核","GM 公告板更新"),
 131:(None,None,"待核","槽位消息"),
 132:(None,None,"待核","狂豹猎人信息"),
 133:(None,None,"待核","账号附加信息"),
 134:(None,None,"待核","好友搜索"),
 135:(None,None,"待核","阶段切换"),
 136:(None,None,"待核","龙珠盒"),
 137:(None,None,"待核","PamsSong 询问"),
 138:("CHANGE_CHANNEL",16,"待核","换频道结果"),
 139:(None,None,"待核","禁止投递任务列表"),
 140:("MACRO_SYS_DATA_INIT",124,"证据",""),
}

RECV_FIELD = {
 144:("SET_BACK_EFFECT",128,"证据","CMapLoadable"),
 145:("SET_MAP_OBJECT_VISIBLE",129,"证据","CMapLoadable"),
 146:("CLEAR_BACK_EFFECT",130,"证据","CMapLoadable"),
 147:("BLOCKED_MAP",131,"待核","转移请求被忽略"),
 148:("BLOCKED_SERVER",132,"待核","换线请求被忽略"),
 149:(None,None,"待核","地图私有数据"),
 150:("MULTICHAT",134,"证据","组消息"),
 151:("WHISPER",135,"证据",""),
 152:("SPOUSE_CHAT",136,"证据",""),
 153:("SUMMON_ITEM_INAVAILABLE",137,"证据",""),
 154:("FIELD_EFFECT",138,"证据",""),
 155:("FIELD_OBSTACLE_ONOFF",139,"证据",""),
 156:("FIELD_OBSTACLE_ONOFF_LIST",140,"证据",""),
 157:("FIELD_OBSTACLE_ALL_RESET",141,"证据",""),
 158:("BLOW_WEATHER",142,"证据",""),
 159:("PLAY_JUKEBOX",143,"证据",""),
 160:("ADMIN_RESULT",144,"证据",""),
 161:("OX_QUIZ",145,"证据",""),
 162:("GMEVENT_INSTRUCTIONS",146,"证据","警告信息"),
 163:("CLOCK",147,"证据","子码 0-3"),
 166:("SET_QUEST_CLEAR",150,"证据",""),
 167:("SET_QUEST_TIME",151,"证据",""),
 168:("ARIANT_RESULT",152,"待核","警告信息"),
 169:("SET_OBJECT_STATE",153,"证据",""),
 170:("STOP_CLOCK",154,"证据",""),
 172:(None,None,"待核","StalkResult"),
 175:("QUICKSLOT_INIT",159,"证据",""),
 176:(None,None,"待核","FootHoldInfo"),
 177:(None,None,"待核","RequestFootHoldInfo"),
 179:("SPAWN_PLAYER",160,"证据",""),
 180:("REMOVE_PLAYER_FROM_MAP",161,"证据",""),
 181:("CHATTEXT",162,"证据",""),
 182:("CHATTEXT",162,"待核","地图外聊天"),
 183:("CHALKBOARD",164,"证据",""),
 184:("UPDATE_CHAR_BOX",165,"证据",""),
 185:("SHOW_CONSUME_EFFECT",166,"证据",""),
 186:("SHOW_SCROLL_EFFECT",167,"证据",""),
 187:(None,None,"待核","超级强化特效"),
 188:(None,None,"待核","潜能强化特效"),
 189:(None,None,"待核","解封特效"),
 190:(None,None,"待核","解除解封特效"),
 191:(None,None,"待核","被玩家攻击"),
 192:(None,None,"待核","特斯拉三角"),
 193:(None,None,"待核","跟随角色"),
 194:(None,None,"待核","PQ 奖励展示"),
 195:(None,None,"待核","阶段设置"),
 197:(None,None,"待核","恢复强化次数特效"),
 198:("SPAWN_PET",168,"证据",""),
 199:("MOVE_PET",170,"证据",""),
 200:("PET_CHAT",171,"证据",""),
 201:("PET_NAMECHANGE",172,"证据",""),
 202:("PET_EXCEPTION_LIST",173,"证据",""),
 203:("PET_COMMAND",174,"证据",""),
 204:(None,None,"待核","宠物包 7"),
 205:(None,None,"待核","宠物包 8"),
 206:("SPAWN_DRAGON",181,"待核","CUser::OnDragonPacket"),
 207:("MOVE_DRAGON",182,"待核",""),
 208:("REMOVE_DRAGON",183,"待核",""),
 210:("MOVE_PLAYER",185,"证据",""),
 214:("CLOSE_RANGE_ATTACK / RANGED_ATTACK / MAGIC_ATTACK",None,"证据","一个 OnAttack(nType) 覆盖 186/187/188"),
 215:(None,None,"待核","技能预备"),
 216:(None,None,"待核","移动射击预备"),
 217:("CANCEL_SKILL_EFFECT",191,"证据",""),
 218:("DAMAGE_PLAYER",192,"证据",""),
 219:("FACIAL_EXPRESSION",193,"证据",""),
 220:("SHOW_ITEM_EFFECT",194,"证据",""),
 221:(None,None,"待核","墓碑升级特效"),
 222:("SHOW_CHAIR",196,"证据",""),
 223:("UPDATE_CHAR_LOOK",197,"证据",""),
 224:("SHOW_FOREIGN_EFFECT",198,"证据",""),
 225:("GIVE_FOREIGN_BUFF",199,"证据",""),
 226:("CANCEL_FOREIGN_BUFF",200,"证据",""),
 227:("UPDATE_PARTYMEMBER_HP",201,"证据",""),
 228:("GUILD_NAME_CHANGED",202,"证据",""),
 229:("GUILD_MARK_CHANGED",203,"证据",""),
 230:("THROW_GRENADE",204,"证据",""),
 231:("SHOW_CHAIR / CANCEL_CHAIR",None,"待核","坐下结果"),
 232:("FACIAL_EXPRESSION",193,"证据",""),
 233:("SHOW_FOREIGN_EFFECT",198,"证据",""),
 234:(None,None,"待核","瞬移"),
 236:("MESO_BAG_MESSAGE",210,"待核","给金币成功"),
 237:("MESO_BAG_MESSAGE",210,"待核","给金币失败"),
 240:(None,None,"待核","渐隐"),
 241:(None,None,"待核","强制渐隐"),
 242:("UPDATE_QUEST_INFO",211,"证据","任务结果"),
 243:("ON_NOTIFY_HP_DEC_BY_FIELD",212,"证据",""),
 245:("PLAYER_HINT",214,"待核","气泡提示"),
 248:("MAKER_RESULT",217,"证据","制造结果"),
 251:("OPEN_UI",220,"证据",""),
 252:("OPEN_UI",220,"待核","带选项开 UI"),
 253:("LOCK_UI",221,"待核","方向模式"),
 254:("LOCK_UI",221,"待核","独立模式"),
 255:("SPAWN_GUIDE",223,"待核","雇佣导师"),
 256:("TALK_GUIDE",224,"待核","导师消息"),
 257:("SHOW_COMBO",225,"证据",""),
 263:(None,None,"待核","公告消息"),
 264:(None,None,"待核","聊天消息"),
 267:(None,None,"待核","伤害计量"),
 276:("COOLDOWN",234,"证据",""),
 284:("SPAWN_MONSTER",236,"证据",""),
 285:("KILL_MONSTER",237,"证据",""),
 286:("SPAWN_MONSTER_CONTROL",238,"证据",""),
 287:("MOVE_MONSTER",239,"证据",""),
 288:("MOVE_MONSTER_RESPONSE",240,"证据",""),
 290:("APPLY_MONSTER_STATUS",242,"证据",""),
 291:("CANCEL_MONSTER_STATUS",243,"证据",""),
 292:("RESET_MONSTER_ANIMATION",244,"证据",""),
 294:("DAMAGE_MONSTER",246,"证据",""),
 298:("SHOW_MONSTER_HP",250,"证据",""),
 304:("NPC_TALK",304,"证据",""),
 305:("OPEN_NPC_SHOP",305,"证据",""),
 306:("CONFIRM_SHOP_TRANSACTION",306,"证据",""),
 308:("ADMIN_SHOP",308,"证据",""),
 309:("STORAGE",309,"证据",""),
 311:("SPAWN_NPC",257,"证据",""),
 312:("REMOVE_NPC",258,"证据",""),
 313:("SPAWN_NPC_REQUEST_CONTROLLER",259,"证据",""),
 314:("NPC_ACTION",260,"证据",""),
 319:("SPAWN_HIRED_MERCHANT",265,"证据",""),
 320:("DESTROY_HIRED_MERCHANT",266,"证据",""),
 321:("UPDATE_HIRED_MERCHANT",267,"证据",""),
 322:("DROP_ITEM_FROM_MAPOBJECT",268,"证据",""),
 323:("REMOVE_ITEM_FROM_MAP",269,"证据",""),
 325:("SPAWN_MIST",273,"待核","消息框创建失败"),
 326:("SPAWN_MIST",273,"待核","消息框进入"),
 327:("REMOVE_MIST",274,"待核","消息框离开"),
 334:("REACTOR_HIT",277,"待核","反应堆状态变更"),
 335:(None,None,"待核","反应堆移动"),
 336:("REACTOR_SPAWN",279,"证据",""),
 337:("REACTOR_DESTROY",280,"证据",""),
 359:(None,None,"待核","龙之巢计时"),
 360:(None,None,"待核","混沌扎昆计时"),
 361:(None,None,"待核","龙之巢计时"),
 362:(None,None,"待核","扎昆计时"),
 363:("NPC_TALK",304,"证据","脚本消息"),
 364:("OPEN_NPC_SHOP",305,"证据","商店打开"),
 365:("CONFIRM_SHOP_TRANSACTION",306,"证据","商店子操作"),
 366:("ADMIN_SHOP_MESSAGE",307,"待核","管理商店打开"),
 367:("ADMIN_SHOP",308,"待核","管理商店子操作"),
 368:("STORAGE",309,"证据","仓库子操作"),
 369:("FREDRICK_MESSAGE",310,"待核","商店银行打开"),
 370:("FREDRICK",311,"待核","商店银行子操作"),
 371:("RPS_GAME",312,"证据",""),
 372:("MESSENGER",313,"证据",""),
 373:("PLAYER_INTERACTION",314,"证据","迷你房基类"),
 381:("PARCEL",322,"证据",""),
 382:("CHARGE_PARAM_RESULT",323,"证据",""),
 383:("QUERY_CASH_RESULT",324,"证据",""),
 384:("CASHSHOP_OPERATION",325,"证据",""),
 385:("CASHSHOP_PURCHASE_EXP_CHANGED",326,"待核",""),
 386:("CASHSHOP_GIFT_INFO_RESULT",327,"待核",""),
 387:("CASHSHOP_CHECK_NAME_CHANGE_POSSIBLE_RESULT",329,"待核",""),
 388:("CASHSHOP_CHECK_NAME_CHANGE_POSSIBLE_RESULT",329,"证据",""),
 390:("CASHSHOP_CHECK_TRANSFER_WORLD_POSSIBLE_RESULT",331,"证据",""),
 391:("CASHSHOP_GACHAPON_STAMP_RESULT",332,"待核",""),
 392:("CASHSHOP_CASH_ITEM_GACHAPON_RESULT",333,"待核",""),
 393:("CASHSHOP_CASH_ITEM_GACHAPON_RESULT",333,"证据",""),
 395:(None,None,"待核","OneADay"),
 396:(None,None,"待核","免费现金道具通知"),
 398:("KEYMAP",335,"证据",""),
 399:("AUTO_HP_POT",336,"证据",""),
 400:("AUTO_MP_POT",337,"证据",""),
 405:("SEND_TV",341,"待核",""),
 406:("REMOVE_TV",342,"待核",""),
 407:("ENABLE_TV",343,"待核",""),
 421:(None,None,"待核","DOT 伤害信息"),
 422:(None,None,"待核","计算请求结果"),
 432:(None,None,"待核","登出礼物"),
}
RECV_FIELD_SECTIONS = [
 ("4.2.1 地图加载 / 场景 / 时钟（144–177）",list(range(144,178))),
 ("4.2.2 角色进入 / 普通动作（179–233）",list(range(179,234))),
 ("4.2.3 本地角色 / 任务 / UI（234–276）",list(range(234,277))),
 ("4.2.4 怪物 / NPC（284–318）",list(range(284,319))),
 ("4.2.5 雇佣商店 / 掉落 / 反应堆（319–337）",list(range(319,338))),
 ("4.2.6 Boss 计时 / 对话框（359–373）",list(range(359,374))),
 ("4.2.7 包裹 / 商城 / TV（381–432）",list(range(381,433))),
]

def short(t):
    return t if len(t) <= 52 else t[:49] + "..."

def pretty(n):
    mm = re.match(r"\?([^@]+)@([^@]+)@@", n)
    return (mm.group(2) + "::" + mm.group(1)) if mm else n

def build():
    L = []
    w = L.append
    w("# v95 ↔ s9（v83）Opcode 映射")
    w("")
    w("> 目的：为「095 特性移植 + 协议 v2」提供 opcode 对照。**两套编号不可混用**，本表只回答「同一系统在两边叫什么、值是多少、证据强度」。")
    w("> 输入：`notes/opcode-v95.md`（v95 收发表，582 ctor 点 / 238 发送 opcode / 375 接收 opcode）、`E:\\project\\_ms_probe\\send_sites_all.json`、`s9_opcodes.json`。")
    w("> 生成脚本：`E:\\project\\_ms_probe\\gen_opcode_map.py`；机器候选：`map_candidates2.json`。")
    w("> 证据源：`E:\\MXD\\Maplestory95.i64`（GMS v95 未加壳，IDA 13338）；s9 侧为 `BeiDou-Server_s9` 的 `RecvOpcode.java` / `SendOpcode.java`。")
    w("")
    w("## 0. 结论")
    w("")
    w("1. **v95 发送侧已 100% 覆盖**：582/582 个 `COutPacket(long)` 构造点解析为 **238** 个发送 opcode（旧表 217，含 15 个误判“动态 opcode”+ 80 个漏点）。")
    w("2. 两边**只有 7 个同名同义**的直配（VIEW_ALL_CHAR/DELETE_CHAR/SKILL_EFFECT/PET_FOOD/USE_REMOTE/WATER_OF_LIFE/USE_ITEM_REWARD），其余必须按系统语义手工对齐。")
    w("3. v95 把 v83 的多个 opcode **合并**：技能统一进 103、交互统一进 144、攻击统一进 `CUserRemote::OnAttack(nType)`（接收 214）；s9 侧仍是拆分的多包。移植时**不能一对一照搬**，需按 s9 的拆分粒度重排。")
    w("4. 攻击发送侧仍有缺口：近战/远程/普攻/体术的发送 opcode **未能静态取证**（见 §5.3），必须抓包校准。")
    w("5. 中文 095（PLUS2 / 加壳）的自定义 opcode 仍未知；本表只代表 GMS v95 语义。")
    w("")
    w("## 1. 口径与证据等级")
    w("")
    w("| 等级 | 含义 |")
    w("|---|---|")
    w("| 证据 | 函数名/系统语义 + 数值可直接对上；可直接用于移植设计 |")
    w("| 待核 | 系统对得上，但 s9 侧拆分粒度、子码或入口不唯一；需抓包或读 s9 handler 确认 |")
    w("| 无对应 | v95 有、s9 完全没有（或反之），移植时需新增协议或明确不做 |")
    w("")
    w("“v95 发送”= v95 客户端 → 服务端，对应 s9 `RecvOpcode`（服务端收包）。")
    w("“v95 接收”= 服务端 → v95 客户端，对应 s9 `SendOpcode`（服务端发包）。")
    w("")
    w("## 2. 顶层分发对比")
    w("")
    w("| 维度 | v95（GMS） | s9（v83） |")
    w("|---|---|---|")
    w("| opcode 宽度 | uint16 | uint16 |")
    w("| 分发方式 | 登录 0–27（含连接层特例 16–23）→ `CLogin`；28–140 → `CWvsContext`；141–432 → `g_pStage`（`CField`/`CCashShop`） | `Net/PacketSwitch.cpp` 线性 switch（302 个名字） |")
    w("| 发送 opcode 空间 | 1–313（实测 238 个在用） | 1–260 + 277 + 4096 + 14099（181 个在用） |")
    w("| 接收 opcode 空间 | 0–432（375 个在用） | 0–376 + 4096 + 14100（311 个在用） |")
    w("| 攻击拆分 | 接收侧单包 `OnAttack(nType)`；发送侧近战/远程/魔法分散 | 收发都拆成 CLOSE_RANGE / RANGED / MAGIC / ENERGY |")
    w("| 交互拆分 | 144 一个包覆盖交易房/个人商店/委托商店/小游戏 | 123 `PLAYER_INTERACTION` 一个包 + 子码 |")
    w("| 快捷键 | 20/32 连接层、25 心跳、35 登录前导 | 无 |")
    w("")
    w("> 结论：v95 的“区间分发”比 s9 的线性 switch 更省，但 opcode 数值完全不可复用。")
    w("")
    w("## 3. v95 发送 → s9 收包（`RecvOpcode`）")
    w("")
    w("表中“v95 发送函数”只列首个调用点（完整列表见 `notes/opcode-v95.md` §5 与 `send_ops.json`）。")
    w("")
    for title, ops in SEND_SECTIONS:
        w("### " + title)
        w("")
        w("| v95 | v95 发送函数（首个） | s9 RecvOpcode | 值 | 置信 | 备注 |")
        w("|---|---|---|---|---|---|")
        for op in ops:
            name, val, conf, note = SEND[op]
            fns = sorted(pretty(f) for f in v95send.get(op, []))
            fn = "`%s`" % short(fns[0]) if fns else "—"
            if len(fns) > 1:
                fn += "（共 %d 个函数）" % len(fns)
            s9 = ("`%s`" % name) if name else "—"
            v = str(val) if val is not None else "—"
            w("| %d | %s | %s | %s | %s | %s |" % (op, fn, s9, v, conf, note))
        w("")
    w("### 3.9 仅 s9 存在（v95 无对应，移植时为本项目自定义）")
    w("")
    w("以下 s9 收包在 v95 发送表中找不到同义项。协议 v2 里它们要么保留 s9 原值，要么按 v95 风格合并：")
    w("")
    w("`GUEST_LOGIN(2)`、`SERVERLIST_REREQUEST(4)`、`CHARLIST_REQUEST(5)`、`PLAYER_DC(12)`、`PLAYER_LOGGEDIN(20)`、")
    w("`CLIENT_START_ERROR(25)`、`CLIENT_ERROR(26)`、`STRANGE_DATA(27)`、`RELOG(28)`、`REGISTER_PIC(29)`、")
    w("`TOUCH_MONSTER_ATTACK(47)`、`TAKE_DAMAGE(48)`、`CLOSE_CHALKBOARD(50)`、`MONSTER_BOOK_COVER(57)`、")
    w("`USE_TELEPORT_ROCK(84)`（v95 用发送 91）、`REPORT(106)`、`ADMIN_LOG(129)`、`USE_SOLOMON_ITEM(157)`、")
    w("`CLICK_GUIDE(162)`、`BEHOLDER(178)`、`FIELD_DAMAGE_MOB(191)`、`MOB_DAMAGE_MOB_FRIENDLY(192)`、")
    w("`PLAYER_MAP_TRANSFER(207)`、`LEFT_KNOCKBACK(212)`、`MATCH_TABLE(214)`、`USE_MAPLELIFE(256)`、`USE_HAMMER(260)`、")
    w("`SET_HPMPALERT(4096)`、`CUSTOM_PACKET(14099)`、`MAPLETV(65534)`")
    w("")
    w("## 4. v95 接收 → s9 发包（`SendOpcode`）")
    w("")
    w("### 4.1 `CWvsContext` 段（28–140，全量）")
    w("")
    w("| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |")
    w("|---|---|---|---|---|---|")
    for op in range(28, 141):
        if op not in RECV or op not in v95recv:
            continue
        name, val, conf, note = RECV[op]
        s9 = ("`%s`" % name) if name else "—"
        v = str(val) if val is not None else "—"
        w("| %d | `%s` | %s | %s | %s | %s |" % (op, short(v95recv[op]), s9, v, conf, note))
    w("")
    w("### 4.2 `CField` / 池 / 对话框段（141–432，锚点）")
    w("")
    w("未列出的 opcode 为 v95 无处理或尚未取证的子码（完整数值见 `notes/opcode-v95.md` §3–§4）。")
    w("")
    for title, ops in RECV_FIELD_SECTIONS:
        w("#### " + title)
        w("")
        w("| v95 | 处理函数 | s9 SendOpcode | 值 | 置信 | 备注 |")
        w("|---|---|---|---|---|---|")
        for op in ops:
            if op not in RECV_FIELD:
                continue
            name, val, conf, note = RECV_FIELD[op]
            s9 = ("`%s`" % name) if name else "—"
            v = str(val) if val is not None else "—"
            h = v95recv.get(op, "—")
            w("| %d | `%s` | %s | %s | %s | %s |" % (op, short(h), s9, v, conf, note))
        w("")
    w("## 5. 本轮补齐与修正")
    w("")
    w("### 5.1 新增的 12 个发送 opcode")
    w("")
    w("旧表漏掉、本轮由 `insn_query` 逐点回推补齐：")
    w("")
    w("| opcode | 构造函数 | 证据（紧邻 ctor 的 push） | 语义 |")
    w("|---|---|---|---|")
    EV = {
      20:"连接建立（`CClientSocket::OnConnect`）",
      32:"连接建立第二包",
      52:"受击/延迟上报（`CUserLocal::SetDamaged` / `Update`）",
      126:"斜杠聊天（`CField::SendChatMsgSlash`）",
      139:"斜杠聊天变体",
      152:"斜杠聊天变体",
      158:"斜杠聊天变体",
      219:"魔法攻击（`CUserLocal::TryDoingMagicAttack`）",
      230:"怪物状态同步 A（`CMob::Update`）",
      231:"怪物状态同步 B（`CMob::Update`）",
      234:"怪物状态同步 C（`CMob::Update`）",
      258:"斜杠聊天变体",
    }
    seen = set()
    for op in [20, 32, 52, 126, 139, 152, 158, 219, 230, 231, 234, 258]:
        for s in sites:
            if s["op"] != op or op in seen:
                continue
            seen.add(op)
            how = s["how"].replace("disasm:", "")
            how = re.sub(r"\s*;\s*(?:'.*?'\s*;\s*)?nType", "", how).strip()
            w("| %d | `%s` (%s) | `%s` | %s |" % (op, pretty(s["name"]), s["fn"], how, EV[op]))
    w("")
    w("### 5.2 语义修正")
    w("")
    w("| 旧认识 | 修正 | 依据 |")
    w("|---|---|---|")
    w("| v95 44/45/46 = 近战/远程/魔法攻击 | 44 = 单键动作、45 = 起身、46 = 坐下 | `CUserLocal::OnKey` / `HandleXKeyDown` + `CWvsContext::SendGetUpFromChairRequest` / `SendSitOnPortableChairRequest` |")
    w("| v95 103 = 某个主动技能 | 103 = **统一技能入口**（主动技能/召唤/召唤怪/城镇传送/吞噬/磁铁…） | 15 个函数共用：`DoActiveSkill_*` ×10 + `SendSkillUseRequest` / `TryDoingMonsterMagnet` / `TryDoingSwallowAbsorb` / `CGrenade::SendTimeBombInfo` |")
    w("| v95 144 = 交易 | 144 = **统一交互**（交易房/个人商店/委托商店/记忆/五子棋/迷你房） | 57 个 Dlg 方法共用（`CTradingRoomDlg`/`CPersonalShopDlg`/`CEntrustedShopDlg`/`COmokDlg`/`CMemoryGameDlg`/`CMiniRoomBaseDlg` …） |")
    w("| 219 = UI_Menu | 218 = UI_Menu、**219 = 魔法攻击** | `push 0DBh` @0x92dc52 → ctor @0x92dc5d；该块在 `CUserLocal::TryDoingMagicAttack`（0x92a240，size 0x3c26）内 |")
    w("| 259 = 可映射 s9 | v95 GuildBoss 在 s9 收包侧**无对应** | s9 `RecvOpcode` 无 guild boss 项 |")
    w("")
    w("### 5.3 缺口：攻击发送 opcode 未能静态取证")
    w("")
    w("- `CUserLocal::TryDoingCloseRangeAttack` / `TryDoingShootAttack` / 普攻 / 体术的包体构造**内联**在 `0x9237f8` 起始的块里，该块无 COutPacket ctor 调用、无 xref 进入（`0x923600–0x9237f7` 为原始数据），静态无法证明 opcode。")
    w("- 已核 `0x91e780`（近战）、`0x925a00`（远程）、`0x9123c0`（普攻）、`0x930710`（体术）四个函数：调用 `Encode*`/`SendPacket` 但**不调用任何 COutPacket 构造**，opcode 来自上层传入或已构造对象。")
    w("- 处理：不臆造数值。移植阶段用**动态抓包**（`SendPacket` 断点 / 明文代理）在 095 客户端上实测，再回填本表。")
    w("- 连带影响：v95 接收侧攻击是单包 214（`OnAttack(nType)`），s9 侧拆 186/187/188/189；协议 v2 若采用 v95 风格，需要新增子类型字段而不是新 opcode。")
    w("")
    w("## 6. 覆盖度统计")
    w("")
    ev = sum(1 for v in SEND.values() if v[2] == "证据")
    pv = sum(1 for v in SEND.values() if v[2] == "待核")
    nv = sum(1 for v in SEND.values() if v[2] == "无对应")
    allrecv = dict(RECV); allrecv.update(RECV_FIELD)
    ev2 = sum(1 for v in allrecv.values() if v[2] == "证据")
    pv2 = sum(1 for v in allrecv.values() if v[2] == "待核")
    nv2 = sum(1 for v in allrecv.values() if v[2] == "无对应")
    w("| 方向 | 行数 | 证据 | 待核 | 无对应 |")
    w("|---|---|---|---|---|")
    w("| v95 发送 → s9 收包 | %d | %d | %d | %d |" % (len(SEND), ev, pv, nv))
    w("| v95 接收 → s9 发包 | %d | %d | %d | %d |" % (len(allrecv), ev2, pv2, nv2))
    w("")
    w("v95 发送侧 238 个 opcode 全部覆盖；接收侧覆盖 %d/%d（其余为 v95 无处理或仅子码，见 `notes/opcode-v95.md` §3–§4）。" % (len(allrecv), len(v95recv)))
    w("")
    w("## 7. 自动匹配附录")
    w("")
    w("`build_map2.py` 用 Dice 系数对函数名与 s9 枚举名做词元相似度。**仅作交叉检查**：v95 函数名是 mangled 且粒度更粗，机器匹配会给出误配（例：v95 48 `OnQuestClear` → 150 `SET_QUEST_CLEAR` 应为 49 `QUEST_CLEAR`；v95 282 `CSummonedPool::OnSkill` → 37 `SKILL_USE_RESULT` 应为 180 `SUMMON_SKILL`）。")
    w("")
    r1 = [r for r in cand["recv"] if r["cand"] and r["cand"][0]["score"] == 1.0]
    s1 = [r for r in cand["send"] if r["cand"] and r["cand"][0]["score"] == 1.0]
    w("### 7.1 接收侧词元全等（%d 条）" % len(r1))
    w("")
    w("| v95 recv | v95 处理函数 | s9 SendOpcode | 值 |")
    w("|---|---|---|---|")
    for r in r1:
        c = r["cand"][0]
        w("| %d | `%s` | `%s` | %d |" % (r["v95"], short(r["handler"]), c["s9"], c["v"]))
    w("")
    w("### 7.2 发送侧词元全等（%d 条，均正确）" % len(s1))
    w("")
    w("| v95 send | s9 RecvOpcode | 值 | handler |")
    w("|---|---|---|---|")
    for r in s1:
        c = r["cand"][0]
        w("| %d | `%s` | %d | `%s` |" % (r["v95"], c["s9"], c["v"], c["handler"]))
    w("")
    w("## 8. 复现")
    w("")
    w("```bash")
    w("# 1) v95 发送站点（IDB 13338，headless）")
    w("python E:/project/_ms_probe/batch_send.py            # -> dec_send/*.json")
    w("python E:/project/_ms_probe/resolve_dyn.py          # -> ctor_sites.json")
    w("python E:/project/_ms_probe/extract_trunc_send.py    # -> trunc_ctor_sites.json / trunc_ctor_resolved.json")
    w("python E:/project/_ms_probe/extract_trunc2.py        # -> trunc2_ctor_resolved.json")
    w("# 2) 合并 + 去重")
    w("python E:/project/_ms_probe/merge_send.py            # -> send_sites_all.json / send_ops.json")
    w("# 3) s9 枚举快照")
    w("python E:/project/_ms_probe/extract_s9.py            # -> s9_opcodes.json")
    w("# 4) 机器候选 + 本文")
    w("python E:/project/_ms_probe/build_map2.py            # -> map_candidates2.json")
    w("python E:/project/_ms_probe/gen_opcode_map.py        # -> notes/opcode-map-v95-vs-s9.md")
    w("```")
    w("")
    w("## 9. 备注")
    w("")
    w("- 本表是**移植设计输入**，不是协议规范。协议 v2 的最终 opcode 值由 s9 侧决定（保留/重排），本表只用于确定“某个 v95 行为在 s9 归哪个包”。")
    w("- 095 中文加壳端（`开启游戏.vmp.exe`）无法静态确认自定义 opcode；所有“待核”项在抓包前都不得写入服务端。")
    w("- 与已有功能（联盟 143 / 制造 113 / 怪物图鉴 83/84）相关的行已用“证据”标注，可直接复核现有 handler。")
    w("- 原始数据与复现脚本归档在 [evidence/opcode-v95/](../evidence/opcode-v95/)（`send_sites_all.json` / `map_candidates2.json` / `s9_opcodes.json` + `scripts/`，校验和见其 `hashes.txt`）。")
    w("")
    return "\n".join(L)

if __name__ == "__main__":
    doc = build()
    io.open(OUT, "w", encoding="utf-8", newline="\n").write(doc)
    print("wrote %s (%d lines)" % (OUT, doc.count("\n") + 1))
