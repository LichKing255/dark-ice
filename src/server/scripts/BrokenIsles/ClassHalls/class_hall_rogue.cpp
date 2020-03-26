/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
 * Copyright (C) 2008-2017 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "PhasingHandler.h"
#include "GameObject.h"
#include "ScriptedGossip.h"
#include "Log.h"
enum
{
    NPC_RAVENHOLDT_COURIER_102018 = 102018,
    NPC_LORD_JORACH_RAVENHOLDT_101513 = 101513,
    NPC_VALEERA_SANGUINAR_98102 = 98102,
    NPC_FLEET_ADMIRAL_TETHYS_94159 = 94159,
    NPC_PRINCESS_TESS_GREYMANE_94138 = 94138,

    QUEST_CALL_OF_THE_UNCROWNED = 40832,
    QUEST_THE_FINAL_SHADOW = 40839,
    QUEST_A_WORTHY_BLADE = 40840,

    PLAYER_CHOICE_ROGE_SELECTION = 262,
};

struct npc_ravenholdt_courier_102018 : public ScriptedAI
{
    npc_ravenholdt_courier_102018(Creature* creature) : ScriptedAI(creature) { sayHello = false; }
    enum
    {
        SPELL_1 = 201212,
        SPELL_2 = 201234,
    };
    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
        if (!sayHello)
            DoCastSelf(SPELL_1);
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CALL_OF_THE_UNCROWNED)
        {
            me->RemoveAurasDueToSpell(SPELL_2);
            Talk(1, player);
            _scheduler.Schedule(Milliseconds(1000), [this](TaskContext context)
            {
                me->DisappearAndDie();
            });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld())
            return;

        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
        me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
        if (!sayHello)
        {
            sayHello = true;
            Talk(0, player);
        }
    }
private:
    TaskScheduler _scheduler;
    bool sayHello;
};
///251033
class go_facade_251033 : public GameObjectScript
{
public:
    go_facade_251033() : GameObjectScript("go_facade_251033") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        player->KilledMonsterCredit(102086);
        return false;
    }
};

struct npc_enter_the_hall_of_shadows_102086 : public ScriptedAI
{
    npc_enter_the_hall_of_shadows_102086(Creature* creature) : ScriptedAI(creature) { }


    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld())
            return;
        if (!me->IsWithinDist(who, 15.0f, false))
        {
            return;
        }
        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
        if ( player->HasQuest(QUEST_CALL_OF_THE_UNCROWNED) )
            player->KilledMonsterCredit(me->GetEntry());
        if (player->getClass() != CLASS_ROGUE)
        {
            ///TO DO TELE TO
        }
    }
};

struct npc_red_jack_findle_97004 : public ScriptedAI
{
    npc_red_jack_findle_97004(Creature* creature) : ScriptedAI(creature) { }

    void sGossipHello(Player* player)
    {
        ClearGossipMenuFor(player);
        player->PrepareGossipMenu(me, 19540);
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        if (player->HasQuest( QUEST_CALL_OF_THE_UNCROWNED)  )
        {
            if (gossipListId == 0)
            {
                Talk(0, player);
                player->KilledMonsterCredit(105907);
                CloseGossipMenuFor(player);
            }
        }
    }
};


///READ 201253
///SPELL_CONVERSATION_QUEST_CALL_OF_THE_UNCROWNED = 201254 (1101)
class spell_provided_for_201253 : public PlayerScript
{
public:
    spell_provided_for_201253() : PlayerScript("spell_provided_for_201253") { }

    void OnUpdate(Player* player, uint32 /*diff*/)
    {
        if(player->HasAura(201253)&& player->HasQuest(QUEST_CALL_OF_THE_UNCROWNED))
            player->KilledMonsterCredit(102041);
    }
};
///246387
class go_torch_246387 : public GameObjectScript
{
public:
    go_torch_246387() : GameObjectScript("go_torch_246387") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/) override
    {
        player->KilledMonsterCredit(102098);
        return false;
    }
};

enum
{
    ACTION_QUEST_THE_FINAL_SHADOW_TALK_1 = 1,
};

struct npc_lord_jorach_ravenholdt_101513 : public ScriptedAI
{
    npc_lord_jorach_ravenholdt_101513(Creature* creature) : ScriptedAI(creature) { }

    void DoAction(int32 param)
    {
        switch (param)
        {
        case ACTION_QUEST_THE_FINAL_SHADOW_TALK_1:
            
            if (Creature* valeera_sanguinar = me->FindNearestCreature(NPC_VALEERA_SANGUINAR_98102, 15.0f))
            {
                _scheduler.Schedule(Milliseconds(1000), [valeera_sanguinar](TaskContext context)
                {
                    valeera_sanguinar->Say(104737);
                });
            }
            _scheduler.Schedule(Milliseconds(3000), [this](TaskContext context)
            {
                me->Say(104757);
            });
            break;//NPC_FLEET_ADMIRAL_TETHYS_94159
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_FINAL_SHADOW)
        {
            ///SPELL_CONVERSATION_ 202389 创建对话  1191
            me->Say(104648, player);
        }
        if (quest->GetQuestId() == QUEST_A_WORTHY_BLADE)
        {
            me->Say(108892, player);
            if (Creature* fleet_admiral_tethys = me->FindNearestCreature(NPC_FLEET_ADMIRAL_TETHYS_94159, 25.0f))
            {
                _scheduler.Schedule(Milliseconds(2000), [fleet_admiral_tethys, player](TaskContext context)
                {
                    fleet_admiral_tethys->Say(108898, player);
                });
            }
            if (Creature* valeera_sanguinar = me->FindNearestCreature(NPC_VALEERA_SANGUINAR_98102, 15.0f))
            {
                _scheduler.Schedule(Milliseconds(4000), [valeera_sanguinar, player](TaskContext context)
                {
                    valeera_sanguinar->Say(108901, player);
                });
            }
        }
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/)  override
    {
        if (quest->GetQuestId() == QUEST_THE_FINAL_SHADOW)
        {

        }
    }

    void sGossipHello(Player* player)
    {
        ClearGossipMenuFor(player);
        if (player->HasQuest(QUEST_A_WORTHY_BLADE)&& !player->GetQuestObjectiveData(QUEST_A_WORTHY_BLADE, 3))
        {
            player->PrepareGossipMenu(me, 19545, false);
        }
        else
            player->PrepareGossipMenu(me, 19545, true);
        
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        if (player->HasQuest(QUEST_A_WORTHY_BLADE)&&player->GetQuestObjectiveData(QUEST_A_WORTHY_BLADE,0) && player->GetQuestObjectiveData(QUEST_A_WORTHY_BLADE, 1) && player->GetQuestObjectiveData(QUEST_A_WORTHY_BLADE, 2))
        {
            if (gossipListId == 0)
            {
                CloseGossipMenuFor(player);
                player->CastSpell(player, 201343, true);               
            }
        }
    }
private:
    TaskScheduler _scheduler;
};


class roge_playerchoice : public PlayerScript
{
public:
    roge_playerchoice() : PlayerScript("roge_playerchoice") { }

    void OnPlayerChoiceResponse(Player* player, uint32 choiceId, uint32 responseId)
    {
        if (choiceId != PLAYER_CHOICE_ROGE_SELECTION)
            return;

        TC_LOG_ERROR("server.worldserver", "roge_playerchoice %u, %u", choiceId, responseId);
        if (player->HasQuest(QUEST_A_WORTHY_BLADE))
        {
            player->KilledMonsterCredit(105953);
        }
    }
};

struct npc_valeera_sanguinar_98102 : public ScriptedAI
{
    npc_valeera_sanguinar_98102(Creature* creature) : ScriptedAI(creature) { }

    void DoAction(int32 param)
    {
        switch (param)
        {
        //case ACTION_RESPONSEID_1:
        //    break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        if (player->HasQuest(QUEST_A_WORTHY_BLADE))
        {
            if (gossipListId == 0)
            {
                player->KilledMonsterCredit(98102);
                CloseGossipMenuFor(player);
            }
        }
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/)  override
    {
    }
private:
    TaskScheduler _scheduler;
};


///SPELL_CONVERSATION_ 202417 1192
///246670 chair
class go_chair_246670 : public GameObjectScript
{
public:
    go_chair_246670() : GameObjectScript("go_chair_246670") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (player->HasQuest(QUEST_THE_FINAL_SHADOW) && (!player->GetQuestObjectiveData(QUEST_THE_FINAL_SHADOW, 0)))
        {
            player->KilledMonsterCredit(102498);
            if (Creature* lord_jorach = go->FindNearestCreature(NPC_LORD_JORACH_RAVENHOLDT_101513, 15.0f))
            {
                lord_jorach->AI()->DoAction(ACTION_QUEST_THE_FINAL_SHADOW_TALK_1);
            }
        }
        else if (player->HasQuest(QUEST_THE_FINAL_SHADOW) && (player->GetQuestObjectiveData(QUEST_THE_FINAL_SHADOW, 0)))
        {
            player->KilledMonsterCredit(102502);
        }      
        return false;
    }
};



struct npc_fleet_admiral_tethys_94159 : public ScriptedAI
{
    npc_fleet_admiral_tethys_94159(Creature* creature) : ScriptedAI(creature) {  }

    void DoAction(int32 param)
    {
        switch (param)
        {
        //case ACTION_RESPONSEID_1:
        //    break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        if (player->HasQuest(QUEST_A_WORTHY_BLADE))
        {
            if (gossipListId == 0)
            {
                player->KilledMonsterCredit(94159);
                CloseGossipMenuFor(player);
            }
        }
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {

    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/)  override
    {

    }
private:
    TaskScheduler _scheduler;
};



struct npc_princess_tess_greymane_94138 : public ScriptedAI
{
    npc_princess_tess_greymane_94138(Creature* creature) : ScriptedAI(creature) {  }

    void DoAction(int32 param)
    {
        switch (param)
        {
        //case ACTION_RESPONSEID_1:
        //    break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {

    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/)  override
    {

    }

    void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
    {
        TC_LOG_ERROR("server.worldserver", "sGossipSelect %u, %u", menuId, gossipListId);
        if (player->HasQuest(QUEST_A_WORTHY_BLADE))
        {
            if (gossipListId == 0)
            {
                player->KilledMonsterCredit(94138);
                CloseGossipMenuFor(player);
            }
        }
    }
private:

    TaskScheduler _scheduler;
};
///100762
///spell 49414????
void AddSC_class_hall_rogue()
{
    RegisterCreatureAI(npc_ravenholdt_courier_102018);
    new go_facade_251033();
    RegisterCreatureAI(npc_enter_the_hall_of_shadows_102086);
    RegisterCreatureAI(npc_red_jack_findle_97004);
    new spell_provided_for_201253();
    new go_torch_246387();
    RegisterCreatureAI(npc_lord_jorach_ravenholdt_101513);
    RegisterCreatureAI(npc_valeera_sanguinar_98102);
    new go_chair_246670();
    RegisterCreatureAI(npc_fleet_admiral_tethys_94159);
    RegisterCreatureAI(npc_princess_tess_greymane_94138);
    new roge_playerchoice();
}
