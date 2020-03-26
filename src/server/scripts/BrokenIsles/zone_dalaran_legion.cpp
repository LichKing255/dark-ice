/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Conversation.h"
#include "GameObject.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Log.h"
/*
 * Dalaran above Karazhan
 *
 * Legion Intro
 */

enum
{
    PHASE_DALARAN_KARAZHAN  = 5829,
    QUEST_BLINK_OF_AN_EYE   = 44663,
};

// TODO : All this script is temp fix,
// remove it when legion start quests are properly fixed
class OnLegionArrival : public PlayerScript
{
public:
    OnLegionArrival() : PlayerScript("OnLegionArrival") { }

    enum
    {
        SPELL_MAGE_LEARN_GUARDIAN_HALL_TP   = 204287,
        SPELL_WAR_LEARN_JUMP_TO_SKYHOLD     = 192084,
        SPELL_DRUID_CLASS_HALL_TP           = 204874,
        SPELL_CREATE_CLASS_HALL_ALLIANCE    = 185506,
        SPELL_CREATE_CLASS_HALL_HORDE       = 192191,

        CONVERSATION_KHADGAR_BLINK_OF_EYE   = 3827,
    };

    void OnLogin(Player* player, bool firstLogin) override
    {
        // Can happen in recovery cases
        if (player->getLevel() >= 100 && firstLogin)
            HandleLegionArrival(player);
    }

    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (oldLevel < 100 && player->getLevel() >= 100)
            HandleLegionArrival(player);
    }

    void HandleLegionArrival(Player* player)
    {
        switch (player->getClass())
        {
            case CLASS_MAGE:
                player->CastSpell(player, SPELL_MAGE_LEARN_GUARDIAN_HALL_TP, true);
                break;
            case CLASS_WARRIOR:
                player->CastSpell(player, SPELL_WAR_LEARN_JUMP_TO_SKYHOLD, true);
                break;
            case CLASS_DRUID:
                player->CastSpell(player, SPELL_DRUID_CLASS_HALL_TP, true);
                break;
            case CLASS_HUNTER:
                player->m_taxi.SetTaximaskNode(1848); // Hunter Class Hall
                break;
            default:
                break;
        }

        player->CastSpell(player, player->IsInAlliance() ? SPELL_CREATE_CLASS_HALL_ALLIANCE : SPELL_CREATE_CLASS_HALL_HORDE, true);

        if (player->GetQuestStatus(QUEST_BLINK_OF_AN_EYE) == QUEST_STATUS_NONE)
        {
            Conversation::CreateConversation(CONVERSATION_KHADGAR_BLINK_OF_EYE, player, player->GetPosition(), { player->GetGUID() });

            if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_BLINK_OF_AN_EYE))
                player->AddQuest(quest, nullptr);
        }
    }
};

class On110Arrival : public PlayerScript
{
public:
    On110Arrival() : PlayerScript("On110Arrival") { }

    enum
    {
        QUEST_UNITING_THE_ISLES     = 43341,
    };

    void OnLogin(Player* player, bool firstLogin) override
    {
        // Can happen in recovery cases
        if (player->getLevel() >= 110 && firstLogin)
            Handle110Arrival(player);
    }

    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (oldLevel < 110 && player->getLevel() >= 110)
            Handle110Arrival(player);
    }

    void Handle110Arrival(Player* player)
    {
        if (player->GetQuestStatus(QUEST_UNITING_THE_ISLES) == QUEST_STATUS_NONE)
            if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_UNITING_THE_ISLES))
                player->AddQuest(quest, nullptr);
    }
};

// 228329 & 228330 - T??ortation
class spell_dalaran_teleportation : public SpellScript
{
    PrepareSpellScript(spell_dalaran_teleportation);

    void EffectTeleportDalaranKarazhan(SpellEffIndex effIndex)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->getLevel() < 100 || player->GetQuestStatus(QUEST_BLINK_OF_AN_EYE) != QUEST_STATUS_INCOMPLETE)
                PreventHitEffect(effIndex);
            else
            {
                if (SpellTargetPosition const* targetPosition = sSpellMgr->GetSpellTargetPosition(GetSpellInfo()->Id, effIndex))
                    if (Map* map = sMapMgr->FindMap(targetPosition->target_mapId, 0))
                        map->LoadGrid(targetPosition->target_X, targetPosition->target_Y);
            }
        }
    }

    void EffectTeleportDalaranLegion(SpellEffIndex effIndex)
    {
        if (Player* player = GetCaster()->ToPlayer())
            if (player->getLevel() < 100 || player->GetQuestStatus(QUEST_BLINK_OF_AN_EYE) == QUEST_STATUS_INCOMPLETE)
                PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_dalaran_teleportation::EffectTeleportDalaranKarazhan, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectLaunch += SpellEffectFn(spell_dalaran_teleportation::EffectTeleportDalaranLegion, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 113986 - Khadgar
class npc_dalaran_karazhan_khadgar : public CreatureScript
{
public:
    npc_dalaran_karazhan_khadgar() : CreatureScript("npc_dalaran_karazhan_khadgar") { }

    enum
    {
        SPELL_PLAY_DALARAN_TELEPORTATION_SCENE = 227861
    };

    bool OnGossipHello(Player* player, Creature* /*creature*/)
    {
        player->CastSpell(player, SPELL_PLAY_DALARAN_TELEPORTATION_SCENE, true);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*uiSender*/, uint32 /*uiAction*/) override
    {
        player->CastSpell(player, SPELL_PLAY_DALARAN_TELEPORTATION_SCENE, true);
        return true;
    }
};

class scene_dalaran_kharazan_teleportion : public SceneScript
{
public:
    scene_dalaran_kharazan_teleportion() : SceneScript("scene_dalaran_kharazan_teleportion") { }

    void OnSceneTriggerEvent(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/, std::string const& triggerName) override
    {
        if (triggerName == "invisibledalaran")
            PhasingHandler::AddPhase(player, PHASE_DALARAN_KARAZHAN);
    }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->KilledMonsterCredit(114506);
        player->TeleportTo(1220, -827.82f, 4369.25f, 738.64f, 1.893364f);
    }
};

/*
 * Legion Dalaran
 */

// Zone 8392
class zone_legion_dalaran_underbelly : public ZoneScript
{
public:
    zone_legion_dalaran_underbelly() : ZoneScript("zone_legion_dalaran_underbelly") { }

    void OnPlayerEnter(Player* player) override
    {
        player->SeamlessTeleportToMap(MAP_DALARAN_UNDERBELLY);
    }

    void OnPlayerExit(Player* player) override
    {
        if(player->GetMapId() == MAP_DALARAN_UNDERBELLY)
            player->SeamlessTeleportToMap(MAP_BROKEN_ISLANDS);

    }
};

enum
{
    ///Druid Quest
    ///start  from Archdruid Hamuul Runetotem once you arrive in Dalaran
    QUEST_A_SUMMONS_FROM_MOONGLADE = 40643,
    NPC_ARCHDRUID_HAMUUL_RUNETOTEM = 101061,

    ///quest 47221 The Hand of Fate
    QUEST_A_THE_HAND_OF_FATE = 47221,
    QUEST_H_THE_HAND_OF_FATE = 47835,
    CONVERSATION_A_THE_HAND_OF_FATE = 4904,
    CONVERSATION_H_THE_HAND_OF_FATE = 5337,

    QUEST_THE_DALARAN_FOUNTAIN = 40961,
    NPC_NAT_PAGLE = 88317,

    ///Paradin Quest
    ///start  from Archdruid Hamuul Runetotem once you arrive in Dalaran
    QUEST_AN_URGENT_GATHERING = 38710,
    NPC_LORD_MAXWELL_TYROSUS = 92909,

    ///Mage Quest
    NPC_EMISSARY_AULDBRIDGE_111109 = 111109,
    NPC_MERYL_FELSTORM_102700 = 102700,

    CONVERSATION_MAGE_QUEST_START = 1263,
    QUEST_FELSTORMS_PLEA = 41035, /// after quest 44184 In the Blink of an Eye complete

    ///Death Knight Quest
    QUEST_THE_CALL_TO_WAR = 40714,
    SEPLL_CONVERSATION_THE_CALL_TO_WAR = 200226,

    ///Roge Quest
    NPC_RAVENHOLDT_COURIER_102018 = 102018,
    QUEST_CALL_OF_THE_UNCROWNED = 40832,

    ///Hunter Quest
    NPC_SNOWFEATHER_100786 = 100786,
    QUEST_NEEDS_OF_THE_HUNTERS = 40384,

    ///DH Quest
    NPC_KORVAS_BLOODTHORN_99343 = 99343,
    QUEST_CALL_OF_THE_ILLIDARI_39261 = 39261,
    QUEST_CALL_OF_THE_ILLIDARI_39047 = 39047,

    ///Monk Quest before the storm
    NPC_INITIATE_DA_NEL = 98519,
    QUEST_DA_NEL = 12103,

    ///Priest Quest
    NPC_A_HOODED_PRIESTESS = 102333, ///A
    NPC_H_HOODED_PRIESTESS = 101344, ///H
    QUEST_PRIESTLY_MATTERS = 40705,

    ///WARLOCK Quest 
    NPC_RITSSYN_FLAMESCOWL_103506 = 103506,
    QUEST_THE_SIXTH = 40716,   

    ///WARRIOR Quest
    NPC_EITRIGG_93775 = 93775,
    NPC_SERGEANT_DALTON_108961 = 108961,
    QUEST_H_A_DESPERATE_PLEA = 41052,
    QUEST_A_AN_IMPORTANT_MISSION = 42814,
    
};
//
class zone_legion_dalaran: public ZoneScript
{
public:
    zone_legion_dalaran() : ZoneScript("zone_legion_dalaran") { }
    uint32 _insideNpc;
    uint32 _killCredit;
    Creature* creature;
    Position _positon;

    
    void OnTempSummonNPC(uint32 insideNpc, Player* player)
    {
        _insideNpc = insideNpc;
        if (creature = player->FindNearestCreature(_insideNpc, 100.0f))
        {
            // TODO : Remove this line when phasing is done properly
            creature->DestroyForPlayer(player);
        }

        _positon = Position(player->GetPositionX() + 20, player->GetPositionY() + 20, player->GetPositionZ());
        //   _positon= player->GetPosition();


        if (TempSummon* personalCreature = player->SummonCreature(_insideNpc, _positon, TEMPSUMMON_TIMED_DESPAWN, 60000, 0, true))
        {
            //personalCreature->GetMotionMaster()->MoveCloserAndStop(personalCreature->GetEntry(), player, 5.0f);
            personalCreature->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, personalCreature->GetFollowAngle());
            // TODO : personalCreature->Talk(0);
        }
    }
    void OnTempSummonNPCOnPOS(uint32 insideNpc, Player* player,Position npcPos)
    {
        _insideNpc = insideNpc;
        if (creature = player->FindNearestCreature(_insideNpc, 100.0f))
        {
            // TODO : Remove this line when phasing is done properly
            creature->DestroyForPlayer(player);
        }

        _positon = npcPos;
        TempSummon* personalCreature = player->SummonCreature(_insideNpc, _positon, TEMPSUMMON_TIMED_DESPAWN, 3600000, 0, true);  
    }
    enum BROKEN_ISLES_PATHFINDE
    {
        SPELL_BROKEN_ISLES_PATHFINDER_1 = 226342,
        SPELL_BROKEN_ISLES_PATHFINDER_2 = 233368,
  
    };

    void OnPlayerEnter(Player* player) override
    {
        if (player->HasAchieved(11190))//[BROKEN_ISLES_PATHFINDER]
        {
            //226342 SPELL_BROKEN_ISLES_PATHFINDER_1
            player->LearnSpell(SPELL_BROKEN_ISLES_PATHFINDER_1, true);
        }
        if (player->HasAchieved(11446))//[BROKEN_ISLES_PATHFINDER]
        {
            //233368 SPELL_BROKEN_ISLES_PATHFINDER_2
            player->LearnSpell(SPELL_BROKEN_ISLES_PATHFINDER_2, true);
        }

        //AGUS ARRAIVE
        if (player->IsInAlliance() && player->GetQuestStatus(QUEST_A_THE_HAND_OF_FATE) == QUEST_STATUS_NONE && player->getLevel() >= 110 && player->GetZoneId() == 7502)
        {
            Conversation::CreateConversation(CONVERSATION_A_THE_HAND_OF_FATE, player, player->GetPosition(), { player->GetGUID() });

            if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_A_THE_HAND_OF_FATE))
                player->AddQuest(quest, nullptr);
        }
        if (player->IsInHorde() && player->GetQuestStatus(QUEST_A_THE_HAND_OF_FATE) == QUEST_STATUS_NONE && player->getLevel() >= 110 && player->GetZoneId() == 7502)
        {
            Conversation::CreateConversation(CONVERSATION_H_THE_HAND_OF_FATE, player, player->GetPosition(), { player->GetGUID() });

            if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_H_THE_HAND_OF_FATE))
                player->AddQuest(quest, nullptr);
        }
        

        if (player->GetQuestStatus(QUEST_BLINK_OF_AN_EYE) == QUEST_STATUS_REWARDED && player->getLevel() >= 98 && player->GetZoneId() == 7502)
        {
            //Druid Quest
            if (player->getClass() == CLASS_DRUID && player->GetQuestStatus(QUEST_A_SUMMONS_FROM_MOONGLADE) == QUEST_STATUS_NONE)
            {
                OnTempSummonNPC(NPC_ARCHDRUID_HAMUUL_RUNETOTEM, player);
            }
            //Paladin Quest
            if (player->getClass() == CLASS_PALADIN && player->GetQuestStatus(QUEST_AN_URGENT_GATHERING) == QUEST_STATUS_NONE)
            {
                OnTempSummonNPC(NPC_LORD_MAXWELL_TYROSUS, player);
            }
            //Mage Quest phase 4598
            if (player->getClass() == CLASS_MAGE && player->GetQuestStatus(QUEST_FELSTORMS_PLEA) == QUEST_STATUS_NONE)
            {
                Conversation::CreateConversation(CONVERSATION_MAGE_QUEST_START, player, player->GetPosition(), { player->GetGUID() });

                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_FELSTORMS_PLEA))
                    player->AddQuest(quest, nullptr);
            }
            //Death Knight Quest ServerToClient: SMSG_SPELL_START (0x2C3A) Length: 101 ConnIdx: 0 Time: 06/06/2018 13:41:43.782 Number: 6386
            if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetQuestStatus(QUEST_THE_CALL_TO_WAR) == QUEST_STATUS_NONE)
            {
                player->CastSpell(player, SEPLL_CONVERSATION_THE_CALL_TO_WAR, true);
                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_THE_CALL_TO_WAR))
                    player->AddQuest(quest, nullptr);

            }
            //Hunter Quest
            if (player->getClass() == CLASS_HUNTER && player->GetQuestStatus(QUEST_NEEDS_OF_THE_HUNTERS) == QUEST_STATUS_NONE)
            {
                OnTempSummonNPC(NPC_SNOWFEATHER_100786, player);
            }
            //DEMON_HUNTER Quest
            if (player->getClass() == CLASS_DEMON_HUNTER && (player->GetQuestStatus(QUEST_CALL_OF_THE_ILLIDARI_39261) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_CALL_OF_THE_ILLIDARI_39047) == QUEST_STATUS_NONE))
            {
                ///DH Quest
                OnTempSummonNPC(NPC_KORVAS_BLOODTHORN_99343, player);
            }
            //WARRIOR Quest
            if (player->getClass() == CLASS_WARRIOR)
            {
                if (player->IsInAlliance() && player->GetQuestStatus(QUEST_A_AN_IMPORTANT_MISSION) == QUEST_STATUS_NONE)
                    OnTempSummonNPC(NPC_SERGEANT_DALTON_108961, player);
                if (player->IsInHorde() && player->GetQuestStatus(QUEST_H_A_DESPERATE_PLEA) == QUEST_STATUS_NONE)
                    OnTempSummonNPC(NPC_EITRIGG_93775, player);
            }
            //ROGUE Quest
            if (player->getClass() == CLASS_ROGUE && player->GetQuestStatus(QUEST_CALL_OF_THE_UNCROWNED) == QUEST_STATUS_NONE)
            {
                OnTempSummonNPC(NPC_RAVENHOLDT_COURIER_102018, player);
            }
            //Priest Quest
            if (player->getClass() == CLASS_PRIEST && player->GetQuestStatus(QUEST_PRIESTLY_MATTERS) == QUEST_STATUS_NONE)
            {
                uint32 priestNpc = player->IsInAlliance() ? NPC_A_HOODED_PRIESTESS : NPC_H_HOODED_PRIESTESS;
                OnTempSummonNPC(priestNpc, player);
            }
            //SHAMAN Quest
            if (player->getClass() == CLASS_SHAMAN && player->GetQuestStatus(QUEST_THE_CALL_TO_WAR) == QUEST_STATUS_NONE)
            {

            }
            //WARLOCK Quest
            if (player->getClass() == CLASS_WARLOCK && player->GetQuestStatus(QUEST_THE_SIXTH) == QUEST_STATUS_NONE)
            {
                OnTempSummonNPC(NPC_RITSSYN_FLAMESCOWL_103506, player);
            }
            //MONK Quest
            if (player->getClass() == CLASS_MONK && player->GetQuestStatus(QUEST_DA_NEL) == QUEST_STATUS_NONE)
            {
                OnTempSummonNPC(NPC_INITIATE_DA_NEL, player);
            }

            //achievement 10596 npc 88317
            if (player->HasAchieved(10596) && player->GetQuestStatus(QUEST_THE_DALARAN_FOUNTAIN) == QUEST_STATE_COMPLETE )
            {
                OnTempSummonNPC(NPC_NAT_PAGLE, player);
            }
        }

    }

    void OnPlayerExit(Player* player) override
    {

        if (creature = player->FindNearestCreature(_insideNpc, 100.0f))
        {
            // TODO : Remove this line when phasing is done properly
            creature->DestroyForPlayer(player);
        }
    }
};

enum CLASS_PHASE
{
    PHASE_CLASS_WARRIOR_HORDE = 7300,
    PHASE_CLASS_WARRIOR_ALIANCE = 7302,
    PHASE_CLASS_PALADIN = 7319,
    PHASE_CLASS_HUNTER_1 = 7341,
    PHASE_CLASS_HUNTER_2 = 6763,
    PHASE_CLASS_ROGUE = 4,
    PHASE_CLASS_PRIEST = 5,
    PHASE_CLASS_DEATH_KNIGHT = 6,
    //PHASE_CLASS_SHAMAN = 7,
    PHASE_CLASS_MAGE = 8,
    PHASE_CLASS_WARLOCK = 9,
    PHASE_CLASS_MONK = 10,
    //PHASE_CLASS_DRUID = 11,
    PHASE_CLASS_DEMON_HUNTER = 12
};
class PhaseOnDaralanArea : public PlayerScript
{
public:
    PhaseOnDaralanArea() : PlayerScript("PhaseOnDaralanArea") { }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/)
    {
        if (player->GetAreaId() == 7505 && player->getClass() == CLASS_PALADIN)
            PhasingHandler::AddPhase(player, PHASE_CLASS_PALADIN);
        if (player->getClass() == CLASS_HUNTER)
        {
            if (player->GetAreaId() == 7502 || player->GetAreaId() == 7505)
                PhasingHandler::AddPhase(player, PHASE_CLASS_HUNTER_1);
            if (player->GetAreaId() == 7505)
                PhasingHandler::AddPhase(player, PHASE_CLASS_HUNTER_2);
        }
        if (player->GetAreaId() == 7502 && player->getClass() == CLASS_WARRIOR)
        {
            if(player->IsInHorde() && player->HasQuest(QUEST_H_A_DESPERATE_PLEA))
                PhasingHandler::AddPhase(player, PHASE_CLASS_WARRIOR_HORDE);
            if (player->IsInAlliance() && player->HasQuest(QUEST_A_AN_IMPORTANT_MISSION))
                PhasingHandler::AddPhase(player, PHASE_CLASS_WARRIOR_ALIANCE);
        }
        
        
    }
};

struct npc_emissary_auldbridge_111109 : public ScriptedAI
{
    npc_emissary_auldbridge_111109(Creature* creature) : ScriptedAI(creature) { Initialize(); }
    enum Texts
    {
        SAY_FIRST_LINE = 0, ///
        SAY_SECOND_LINE = 1,///
        SAY_THIRD_LINE = 2, ///
    };
    enum DataTypes
    {
        DATA_TALK_1 = 1,
        DATA_TALK_2 = 2,
        DATA_TALK_3 = 3,
        DATA_GIVE_QUEST_CHECK = 4,
    };

    void Initialize()
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/)  override
    {
        if (quest->GetQuestId() == QUEST_BLINK_OF_AN_EYE)
        {
                m_playerGUID = player->GetGUID();
                events.ScheduleEvent(DATA_TALK_1, 1s);  
        }            
    }
    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case DATA_TALK_1:
                {
                    Talk(SAY_FIRST_LINE);
                    events.ScheduleEvent(DATA_TALK_2, 3s);
                    break;
                }
                case DATA_TALK_2:
                {
                    Talk(SAY_SECOND_LINE);
                    events.ScheduleEvent(DATA_TALK_3, 3s);
                    break;
                }
                case DATA_TALK_3:
                {
                    Talk(SAY_THIRD_LINE);
                    events.ScheduleEvent(DATA_GIVE_QUEST_CHECK, 3s);
                    break;
                }
                case DATA_GIVE_QUEST_CHECK:
                {
                    CheckQuestGive();
                    break;
                }
            }
        }
    }
    void OnTempSummonNPC(uint32 insideNpc, Player* player)
    {
        if (Creature* creature = player->FindNearestCreature(insideNpc, 100.0f))
        {
            creature->DespawnOrUnsummon(1000);
            //creature->DestroyForPlayer(player);
        }
        Position _positon = me->GetPosition();       
        GetPositionWithDistInFront(me, 15.f, _positon);

        if (TempSummon* personalCreature = player->SummonCreature(insideNpc, _positon, TEMPSUMMON_TIMED_DESPAWN, 60000, 0, true))
        {
            personalCreature->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, personalCreature->GetFollowAngle());
        }
    }
    void CheckQuestGive()
    {
 
        if (!m_playerGUID.IsEmpty())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (player->GetQuestStatus(QUEST_BLINK_OF_AN_EYE) == QUEST_STATUS_REWARDED && player->getLevel() >= 98 && player->GetZoneId() == 7502 && player->IsInWorld() || player->IsAlive())
                {
                    //Druid Quest
                    if (player->getClass() == CLASS_DRUID && player->GetQuestStatus(QUEST_A_SUMMONS_FROM_MOONGLADE) == QUEST_STATUS_NONE)
                    {
                        OnTempSummonNPC(NPC_ARCHDRUID_HAMUUL_RUNETOTEM, player);
                    }
                    //Paladin Quest
                    if (player->getClass() == CLASS_PALADIN && player->GetQuestStatus(QUEST_AN_URGENT_GATHERING) == QUEST_STATUS_NONE)
                    {
                        OnTempSummonNPC(NPC_LORD_MAXWELL_TYROSUS, player);
                    }
                    //Mage Quest phase 4598
                    if (player->getClass() == CLASS_MAGE && player->GetQuestStatus(QUEST_FELSTORMS_PLEA) == QUEST_STATUS_NONE)
                    {
                        Conversation::CreateConversation(CONVERSATION_MAGE_QUEST_START, player, player->GetPosition(), { player->GetGUID() });

                        if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_FELSTORMS_PLEA))
                            player->AddQuest(quest, nullptr);
                    }
                    //Death Knight Quest ServerToClient: SMSG_SPELL_START (0x2C3A) Length: 101 ConnIdx: 0 Time: 06/06/2018 13:41:43.782 Number: 6386
                    if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetQuestStatus(QUEST_THE_CALL_TO_WAR) == QUEST_STATUS_NONE)
                    {
                        player->CastSpell(player, SEPLL_CONVERSATION_THE_CALL_TO_WAR, true);
                        if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_THE_CALL_TO_WAR))
                            player->AddQuest(quest, nullptr);

                    }
                    //Hunter Quest
                    if (player->getClass() == CLASS_HUNTER && player->GetQuestStatus(QUEST_NEEDS_OF_THE_HUNTERS) == QUEST_STATUS_NONE)
                    {
                        OnTempSummonNPC(NPC_SNOWFEATHER_100786, player);
                    }
                    //DEMON_HUNTER Quest
                    if (player->getClass() == CLASS_DEMON_HUNTER && (player->GetQuestStatus(QUEST_CALL_OF_THE_ILLIDARI_39261) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_CALL_OF_THE_ILLIDARI_39047) == QUEST_STATUS_NONE ) )
                    {
                        ///DH Quest
                        OnTempSummonNPC(NPC_KORVAS_BLOODTHORN_99343, player);
                    }
                    //WARRIOR Quest
                    if (player->getClass() == CLASS_WARRIOR)
                    {
                        if(player->IsInAlliance()&& player->GetQuestStatus(QUEST_A_AN_IMPORTANT_MISSION) == QUEST_STATUS_NONE)
                            OnTempSummonNPC(NPC_SERGEANT_DALTON_108961, player);
                        if (player->IsInHorde() && player->GetQuestStatus(QUEST_H_A_DESPERATE_PLEA) == QUEST_STATUS_NONE)
                            OnTempSummonNPC(NPC_EITRIGG_93775, player);
                    }
                    //ROGUE Quest
                    if (player->getClass() == CLASS_ROGUE && player->GetQuestStatus(QUEST_CALL_OF_THE_UNCROWNED) == QUEST_STATUS_NONE)
                    {
                        OnTempSummonNPC(NPC_RAVENHOLDT_COURIER_102018, player);
                    }
                    //Priest Quest
                    if (player->getClass() == CLASS_PRIEST && player->GetQuestStatus(QUEST_PRIESTLY_MATTERS) == QUEST_STATUS_NONE)
                    {
                        uint32 priestNpc = player->IsInAlliance() ? NPC_A_HOODED_PRIESTESS : NPC_H_HOODED_PRIESTESS;   
                        OnTempSummonNPC(priestNpc, player);
                    }
                    //SHAMAN Quest
                    if (player->getClass() == CLASS_SHAMAN && player->GetQuestStatus(QUEST_THE_CALL_TO_WAR) == QUEST_STATUS_NONE)
                    {

                    }
                    //WARLOCK Quest
                    if (player->getClass() == CLASS_WARLOCK && player->GetQuestStatus(QUEST_THE_SIXTH) == QUEST_STATUS_NONE)
                    {
                        OnTempSummonNPC(NPC_RITSSYN_FLAMESCOWL_103506, player);
                    }
                    //MONK Quest
                    if (player->getClass() == CLASS_MONK && player->GetQuestStatus(QUEST_DA_NEL) == QUEST_STATUS_NONE)
                    {
                         OnTempSummonNPC(NPC_INITIATE_DA_NEL, player);
                    }
                }

        }
            
        //RemovePlayer();  
    }
private:
    EventMap events;
    std::set<ObjectGuid> pList;
    ObjectGuid   m_playerGUID;
};


/*******************************************************
* npc_guard_dalaran_legion
*******************************************************/

enum Spells
{
    SPELL_TRESPASSER_A = 225512,
    SPELL_TRESPASSER_H = 225574,
};

enum NPCs // All outdoor guards are within 35.0f of these NPCs
{
    NPC_APPLEBOUGH_A = 96990,
    NPC_SWEETBERRY_H = 96784,
    NPC_GILNEAN_GUARD = 108323,
    NPC_FORSAKEN_GUARD = 103626,
};

struct npc_guard_dalaran_legion : public ScriptedAI
{
    npc_guard_dalaran_legion(Creature* creature) : ScriptedAI(creature)
    {
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_NORMAL, true);
        creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld() || who->GetZoneId() != 7502)
            return;

        if (!me->IsWithinDist(who, 65.0f, false))
            return;

        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();

        if (!player || player->IsGameMaster() || player->IsBeingTeleported())
            return;

        switch (me->GetEntry())
        {
        case NPC_GILNEAN_GUARD:
            if (player->GetTeam() == HORDE)              // Horde unit found in Alliance area
            {
                if (GetClosestCreatureWithEntry(me, NPC_APPLEBOUGH_A, 32.0f))
                {
                    if (me->isInBackInMap(who, 12.0f))   // In my line of sight, "outdoors", and behind me
                        player->CastSpell(player, SPELL_TRESPASSER_A, true); // Teleport the Horde unit out                          
                }
                else                                      // In my line of sight, and "indoors"
                {
                    if (me->isInFrontInMap(who, 12.0f))
                        player->CastSpell(player, SPELL_TRESPASSER_A, true);    // Teleport the Horde unit out
                }

            }
            break;
        case NPC_FORSAKEN_GUARD:
            if (player->GetTeam() == ALLIANCE)           // Alliance unit found in Horde area
            {
                if (GetClosestCreatureWithEntry(me, NPC_SWEETBERRY_H, 32.0f))
                {
                    if (me->isInBackInMap(who, 12.0f))   // In my line of sight, "outdoors", and behind me
                        player->CastSpell(player, SPELL_TRESPASSER_H, true); // Teleport the Alliance unit out
                }
                else                                      // In my line of sight, and "indoors"
                {
                    if (me->isInFrontInMap(who, 12.0f))
                        player->CastSpell(player, SPELL_TRESPASSER_H, true);     // Teleport the Alliance unit out
                }
            }
            break;
        }
        return;
    }
};

///A Mysterious Note 266619 QUEST 45185
class go_a_mysterious_note : public GameObjectScript
{
public:
    go_a_mysterious_note() : GameObjectScript("go_a_mysterious_note") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        if (player->HasQuest(45185))
            player->CastSpell(player, 231604, true);
        return false;
    }
};

struct npc_credit_portal_taken_110409 : public ScriptedAI
{
    npc_credit_portal_taken_110409(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld())
            return;
        if (!me->IsWithinDist(who, 15.0f, false))
            return;
        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
            player->KilledMonsterCredit(me->GetEntry());
            player->KilledMonsterCredit(97481);          
    }
};

void AddSC_dalaran_legion()
{
    new OnLegionArrival();
    new On110Arrival();

    RegisterSpellScript(spell_dalaran_teleportation);
    new npc_dalaran_karazhan_khadgar();
    new scene_dalaran_kharazan_teleportion();
    new zone_legion_dalaran_underbelly();
    new zone_legion_dalaran();
    new PhaseOnDaralanArea();
    RegisterCreatureAI(npc_emissary_auldbridge_111109);
    RegisterCreatureAI(npc_guard_dalaran_legion);
    new go_a_mysterious_note();
    RegisterCreatureAI(npc_credit_portal_taken_110409);
}
