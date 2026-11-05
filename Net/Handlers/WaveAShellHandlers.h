////////////////////////////////////////////////////////////////////////////////
//	Wave A 壳开窗 handler：Hammer/Vega/Gacha/Family/Memo
//	Fredrick 已迁至 FredrickHandlers；Duey 在 DueyHandlers。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	class ViciousHammerHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class VegaScrollHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CashGachaponOpenHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GachaponBoxHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyInfoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyChartHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyJoinRequestHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyJoinResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MemoResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WeddingProgressHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WeddingCeremonyEndHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WeddingGiftResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalStartHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalObtainedCPHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalPartyCPHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalMessageHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalLeaveHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// W4：公会副本 s2c（287/288）
	class GuildBossHealerMoveHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GuildBossPulleyStateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class RpsGameHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class AriantResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class AriantScoreHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class NewYearCardResHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class AdminResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MapleTVUseResHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 PacketCreator.enableTV / sendTV / removeTV
	class EnableTVHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SendTVHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class RemoveTVHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 MAPLELIFE_RESULT(349) / MAPLELIFE_ERROR(350)
	class MapleLifeResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MapleLifeErrorHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class AvatarMegaphoneHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ClaimResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class IncubatorResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// 活动场：椰子/雪球/OX/地龙洞 → PartyRace 或 FloatNotice
	class CoconutScoreHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
	class SnowballStateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class HitSnowballHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SnowballMessageHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CoconutHitHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
	class OxQuizHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
	class HorntailCaveHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 openUI / lockUI / disableUI / playerHint
	class OpenUIHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class LockUIHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class DisableUIHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class PlayerHintHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SpawnGuideHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class TalkGuideHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class DojoWarpUpHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 SendOpcode.CANCEL_CHAIR(205) — 座位结果
	class CancelChairResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class LeftKnockBackHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SheepRanchInfoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SheepRanchClothesHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class PyramidGaugeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class PyramidScoreHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 field / event shells
	class ClockHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class StopClockHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class BlowWeatherHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GmEventInstructionsHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ContiMoveHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ContiStateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ThrowGrenadeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FieldObstacleOnOffHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FieldObstacleOnOffListHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FieldObstacleAllResetHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 BLOCKED_MAP(131) / BLOCKED_SERVER(132) / FORCED_MAP_EQUIP(133)
	class BlockedMapHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class BlockedServerHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ForcedMapEquipHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 family / quest / rock / catch / gender
	class FamilyNotifyLoginHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilySummonRequestHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyJoinAcceptedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class QuestClearHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class BridleMobCatchFailHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CashPetFoodResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SpawnPortalHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MapTransferResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetGenderHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class TradeMoneyLimitHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class EntrustedShopCheckHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ShopLinkResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetTamingMobInfoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetExtraPendantSlotHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FakeGmNoticeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CancelTransferWorldResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetBackEffectHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CancelNameChangeResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ClaimStatusChangedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SueCharacterResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SessionValueHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ImitatedNpcDataHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ShopScannerResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class RelogResponseHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class LastConnectedWorldHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class RecommendedWorldMessageHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ForcedStatSetHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ForcedStatResetHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MakerResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyPrivilegeListHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilyRepGainHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FamilySetPrivilegeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalSummonHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MonsterCarnivalDiedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WitchTowerScoreHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SpawnKiteHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class RemoveKiteHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CannotSpawnKiteHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class QuickslotInitHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class DataCrcCheckFailedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class NotifyHpDecByFieldHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetNpcScriptableHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MoveMonsterResponseHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class DptTrackerHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class UpdateHpMpAlertHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetItcHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class TournamentHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class TournamentMatchTableHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class TournamentSetPrizeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class TournamentUewHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MtsOperationHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MtsOperation2Handler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class NpcActionHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SummonItemUnavailableHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GuestIdLoginHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class HourChangedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MinimapOnOffHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class BonusExpChangedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class PlayJukeboxHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MarriageRequestHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MarriageResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class NotifyMarriedPartnerMapTransferHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WeddingPhotoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class DestroyShopResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class RandomMorphResHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetPotionDiscountRateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ImitatedNpcResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class LimitedNpcDisableInfoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class PartyValueHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FieldSetVariableHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetMapObjectVisibleHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ClearBackEffectHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GuildBbsHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetQuestClearHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetQuestTimeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WarnMessageHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetObjectStateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ShowConsumeEffectHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ChatText1Handler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SkillUseResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CancelNameChangeByOtherHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ConsultAuthkeyUpdateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ClassCompetitionAuthkeyUpdateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class WebBoardAuthkeyUpdateHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class OpenFullClientDownloadLinkHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ClaimAvailableTimeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class SetBuyEquipExtHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GuildNameChangedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class GuildMarkChangedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class LucksackPassHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class LucksackFailHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class MesoBagMessageHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class KoreanEventHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ShowMagnetHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ResetMonsterAnimationHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class AriantThingHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class AccountInfoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class ChannelSelectedHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class HackshieldRequestHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class CheckCrcResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class KoreanInternetCafeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
