/*
scenario_artifact_brokenshore
lyosky
*/
#include "Log.h"
#include "Scenario.h"
#include "GameObject.h"
#include "InstanceScenario.h"
#include "PhasingHandler.h"
#include "SceneMgr.h"
#include "ScriptMgr.h"

enum DataTypes
{
    DATA_ARTIFACT_BROKENSHORE = 0,
    DATA_STAGE_1 = 1,
    DATA_STAGE_2 = 2,
    DATA_STAGE_3 = 3,
    DATA_STAGE_4 = 4,
    DATA_STAGE_5 = 5,
    DATA_STAGE_6 = 6,
    DATA_STAGE_7 = 7,
    DATA_MAX_ENCOUNTERS = 8,

    ///paladin DATA_STAGE_7
    ///paladin area 7797
    SCENARIO_ID_PALADIN = 775, ///DATA_STAGE_7
    PHASE_PALADIN = 5171,

    SCENARIO_ID_WARRIOR = 961, ///DATA_STAGE_5
    PHASE_WARRIOR = 6815,
    QUEST__RETURN_TO_THE_BROKEN_SHORE = 38904,
    DATA_STEP_1 = 1939,
    DATA_STEP_2 = 2327,
    DATA_STEP_3 = 2030,
    DATA_STEP_4 = 2033,
    DATA_STEP_5 = 2034,
    DATA_STEP_6 = 2035,
    DATA_STEP_7 = 2040,
    DATA_STEP_8 = 2041,

    EVENT_STEP_1 = 1,
    EVENT_STEP_2 = 2,
    EVENT_STEP_3 = 3,
    EVENT_STEP_4 = 4,
    EVENT_STEP_5 = 5,
    EVENT_STEP_6 = 6,
    EVENT_STEP_7 = 7,
    EVENT_STEP_8 = 8,

    SPELL_SCENE_MALGALOR_ARRIVES = 197593, ///warrior SceneID: 1144     PlaybackFlags : 25    SceneInstanceID : 1    SceneScriptPackageID : 1515
};

struct scenario_artifact_brokenshore : public InstanceScript
{
    scenario_artifact_brokenshore(InstanceMap* map) : InstanceScript(map) { }

    void Initialize() override
    {
        SetBossNumber(DATA_MAX_ENCOUNTERS);
        //events.ScheduleEvent(DATA_STEP_1, 3s);
        SetData(DATA_ARTIFACT_BROKENSHORE, NOT_STARTED);
        ///default in paradin
        ClassMode = DATA_STAGE_7;
        for (uint8 i = 1; i < ClassMode; ++i)
            SetData(i, NOT_STARTED);
        isIntr = false;
        needKillCount = 0;
        KillCount = 0;
        StepID = DATA_STAGE_1;
        isComplete = false;
    }
    /*
    https://cn.wowhead.com/spell=193478/%E9%A3%9E%E5%BE%80%E7%A0%B4%E7%A2%8E%E6%B5%B7%E6%BB%A9


    ServerToClient: SMSG_SPELL_EXECUTE_LOG (0x2C40) Length: 58 ConnIdx: 0 Time: 07/15/2018 09:23:33.102 Number: 1000
    Caster: Full: 0x080114000000000000000000000453AC Player/0 R69/S0 Map: 0 Low: 283564
    SpellID: 193478 (-Unknown-)
    EffectsCount: 1
    [0] Effect: 28
    [0] PowerDrainTargetsCount: 0
    [0] ExtraAttacksTargetsCount: 0
    [0] DurabilityDamageTargetsCount: 0
    [0] GenericVictimTargetsCount: 1
    [0] TradeSkillTargetsCount: 0
    [0] FeedPetTargetsCount: 0
    [0] [0] Victim: Full: 0x24011498805FDE800000000000CBFB98 Vehicle/0 R69/S0 Map: 1220 Entry: 98170 Low: 13368216
    HasLogData: False

    ServerToClient: SMSG_SPELL_GO (0x2C39) Length: 128 ConnIdx: 0 Time: 07/15/2018 09:23:33.102 Number: 1001
    (Cast) CasterGUID: Full: 0x080114000000000000000000000453AC Player/0 R69/S0 Map: 0 Low: 283564
    (Cast) CasterUnit: Full: 0x080114000000000000000000000453AC Player/0 R69/S0 Map: 0 Low: 283564
    (Cast) CastID: Full: 0xBC01149880C13DC30000000000A1521E Cast/3 R69/S0 Map: 1220 Low: 10572318
    (Cast) OriginalCastID: Full: 0x0
    (Cast) SpellID: 197879 (-Unknown-)
    (Cast) SpellXSpellVisualID: 91633
    (Cast) CastFlags: 256
    (Cast) CastTime: 55266499
    (Cast) (MissileTrajectory) TravelTime: 0
    (Cast) (MissileTrajectory) Pitch: 0
    (Cast) Ammo.DisplayID: 0
    (Cast) DestLocSpellCastIndex: 0
    (Cast) (Immunities) School: 0
    (Cast) (Immunities) Value: 0
    (Cast) (Predict) Points: 0
    (Cast) (Predict) Type: 0
    (Cast) (Predict) BeaconGUID: Full: 0x0
    (Cast) CastFlagsEx: 0
    (Cast) HitTargetsCount: 1
    (Cast) MissTargetsCount: 0
    (Cast) MissStatusCount: 0
    (Cast) RemainingPowerCount: 0
    (Cast) HasRuneData: False
    (Cast) TargetPointsCount: 0
    (Cast) (Target) Flags: 66 (Unit, DestinationLocation)
    (Cast) (Target) HasSrcLocation: False
    (Cast) (Target) HasDstLocation: True
    (Cast) (Target) HasOrientation: True
    (Cast) (Target) hasMapID : True
    (Cast) (Target) Unit: Full: 0x0
    (Cast) (Target) Item: Full: 0x0
    (DstLocation) Transport: Full: 0x0
    (DstLocation) Location: X: -1873.97 Y: -1207.44 Z: 0.14
    (Cast) (Target) Orientation: 3.45
    (Cast) (Target) MapID: 1500
    (Cast) (Target) Name:
    (Cast) [0] HitTarget: Full: 0x080114000000000000000000000453AC Player/0 R69/S0 Map: 0 Low: 283564

    ServerToClient: SMSG_QUEST_UPDATE_ADD_CREDIT (0x2A8B) Length: 15 ConnIdx: 0 Time: 07/15/2018 09:23:33.102 Number: 1002
    VictimGUID: Full: 0x0
    QuestID: 38904
    ObjectID: 96678
    Count: 1
    Required: 1
    ObjectiveType: 0

    */
    void OnPlayerAreaUpdate(Player* player, uint32 newAreaId, uint32 /*oldAreaId*/)
    {
        /*
        spell 197879
        Map: 1500
        Position: X: -1873.97 Y: -1207.44 Z: 0.14 O: 3.45
        */
        if (newAreaId == 7796 && player->HasQuest(QUEST__RETURN_TO_THE_BROKEN_SHORE) && player->getClass() == CLASS_WARRIOR)
        {
            PhasingHandler::AddPhase(player, PHASE_WARRIOR);
            ClassMode = DATA_STAGE_5;
            //conversation
            SendScenarioState(ScenarioData(SCENARIO_ID_WARRIOR, DATA_STEP_1));
            ///SMSG_SET_DUNGEON_DIFFICULTY 12
        }
        ///.go -2421.58 144.59 7.7694 1500
        if (newAreaId == 7797 && player->getClass() == CLASS_PALADIN)
        {
            PhasingHandler::AddPhase(player, PHASE_PALADIN);
            ClassMode = DATA_STAGE_7;
            //conversation
            ///SendScenarioState(ScenarioData(SCENARIO_ID_PALADIN, DATA_STEP_1));
            ///SMSG_SET_DUNGEON_DIFFICULTY 12
        }
    }

    void NextStep()
    {
        if (StepID < ClassMode)
        {
            ++StepID;
            if (Scenario* scenario = instance->GetInstanceScenario())
                scenario->CompleteCurrStep();
        }
        else if (StepID == ClassMode)
        {
            if (!isComplete)
                if (Scenario* scenario = instance->GetInstanceScenario())
                    scenario->CompleteCurrStep();

            ///COMPLETE SCENARIO
            if (Scenario* scenario = instance->GetInstanceScenario())
                scenario->CompleteScenario();
        }
    }

private:
    EventMap events;
    bool isIntr;
    ObjectGuid GUID_SKYLORD_SHADEGROVE;
    ObjectGuid GUID_CYLINDER_COLLISION;
    std::set<ObjectGuid> m_guardMobs;
    std::set<ObjectGuid> m_targetMobs;
    uint32 needKillCount;
    uint32 KillCount;
    uint8 StepID;
    uint8 ClassMode;
    bool isComplete;
};

///SceneID: 1144     PlaybackFlags : 25    SceneInstanceID : 1    SceneScriptPackageID : 1515
class scene_malgalor_arrives : public SceneScript
{
public:
    scene_malgalor_arrives() : SceneScript("scene_malgalor_arrives") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        ///SUMMON MALGALOR 92608 ON Position: X: -2211.66 Y: -1215.43 Z: 62.9837
    }
};

void AddSC_scenario_artifact_brokenshore()
{
    RegisterInstanceScript(scenario_artifact_brokenshore, 1500);
    new scene_malgalor_arrives();
}
