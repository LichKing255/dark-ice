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

#include "ScriptMgr.h"
#include "Player.h"
#include "InstanceScript.h"
#include "seat_of_triumvirate.h"

DoorData const doorData[] =
{
    { GO_DOOR_ROOM_ZURAAL_THE_ASCENDED,    DATA_ZURAAL_THE_ASCENDED,   DOOR_TYPE_PASSAGE },
    { GO_DOOR_ROOM_VICEROY_NEZHAR,         DATA_VICEROY_NEZHAR,        DOOR_TYPE_PASSAGE }

};

struct instance_seat_of_triumvirate : public InstanceScript
{
    instance_seat_of_triumvirate(InstanceMap* map) : InstanceScript(map)
    {
        SetHeaders(DataHeader);
        SetBossNumber(EncounterCount);
        LoadDoorData(doorData);
    }

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);

        if (instance->IsHeroic())
            creature->SetBaseHealth(creature->GetMaxHealth() * 2.f);
        if (instance->IsMythic())
            creature->SetBaseHealth(creature->GetMaxHealth() * 1.33f);
    }

    void  OnPlayerEnter(Player* player) override
    {
        Conversation::CreateConversation(5632, player, player->GetPosition(), { player->GetGUID() });
    }

    bool SetBossState(uint32 type, EncounterState state) override
    {
        if (!InstanceScript::SetBossState(type, state))
            return false;

        switch (type)
        {
        case DATA_VICEROY_NEZHAR:
        {
            if (state == DONE)
            {
                if (Creature* lura_door = GetCreature(NPC_LURA_DOOR))
                    lura_door->DespawnOrUnsummon();
            }
            //    DoCastSpellOnPlayers(SPELL_CONVERSATION_AFTER_FIRST_BOSS);
            break;
        default:
            break;
        }
        }
    }
};

void AddSC_instance_seat_of_triumvirate()
{
    RegisterInstanceScript(instance_seat_of_triumvirate, 1753);
}
