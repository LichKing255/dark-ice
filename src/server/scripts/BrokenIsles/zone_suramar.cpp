/*
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
enum SPELLS
{
    QUEST_MASQUERADE = 42079,
    LEARN_MASQUERADE = 211086,
    SPELL_MASQUERADE = 211135,
};
///苏拉玛伪装 http://www.wowhead.com/g00/quest=42079/masquerade?i10c.encReferrer=&i10c.ua=1&i10c.dv=14
class Player_Spell_Masquerade : public PlayerScript
{

public:
    Player_Spell_Masquerade() : PlayerScript("Player_Spell_Masquerade") { }

    void OnLogin(Player* player, bool /*firstLogin*/)
    {       
        UseSpellMasquerade(player, player->GetAreaId(), true);
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/)
    {
        UseSpellMasquerade(player, newArea, false);
    }

    void UseSpellMasquerade(Player* player, uint32 area, bool trigger)
    {        
        switch (area)
        {
        case 8149: ///暮光酒庄
        case 8148: ///苏拉玛城
        case 8150: ///群星庭院
        case 7963: ///大步道
        case 8351: ///动物园
        case 8352: ///苏伊什的巢穴
        case 8353: ///命运广场
        case 8355: ///以太大厅
        case 8378: ///月耀码头
        case 8379: ///鎏金集市
        case 8380: ///午夜庭院
        case 8381: ///宝石庄园
        case 8386: ///跃影花园
        case 8385: ///永月集市
        case 8384: ///海妖之祈
        case 8383: ///工匠大道
        case 8382: ///残月酒馆
        case 8395: ///阿斯塔瓦港
        case 8397: ///启迪平台
        case 8398: ///谕令平台
        case 8431: ///秩序大殿
        case 8436: ///日落公园
        case 8441: ///月光长堤
        case 8434: ///唤星者小屋
        case 8345: ///首席奥术师的庄园
        case 8461: ///启迪圣殿
        case 8487: ///永月平台
        case 8496: ///苏拉玛湾
            //if (!player->HasAura(LEARN_MASQUERADE) ||)
            //    return;
            //if (player->GetQuestStatus(QUEST_MASQUERADE) != QUEST_STATUS_REWARDED)
            //    return;
            if(trigger)
                player->RemoveAurasDueToSpell(SPELL_MASQUERADE);
            if (!player->HasAura(SPELL_MASQUERADE))
                player->CastSpell(player, SPELL_MASQUERADE, true);
            player->GetQuestObjectiveCriteriaMgr();
            return;
            break;
        default:
            return;
            break;
        }
    }
};

void AddSC_suramar()
{
    new Player_Spell_Masquerade();
}
