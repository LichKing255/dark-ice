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
///������αװ http://www.wowhead.com/g00/quest=42079/masquerade?i10c.encReferrer=&i10c.ua=1&i10c.dv=14
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
        case 8149: ///ĺ���ׯ
        case 8148: ///�������
        case 8150: ///Ⱥ��ͥԺ
        case 7963: ///�󲽵�
        case 8351: ///����԰
        case 8352: ///����ʲ�ĳ�Ѩ
        case 8353: ///���˹㳡
        case 8355: ///��̫����
        case 8378: ///��ҫ��ͷ
        case 8379: ///�̽���
        case 8380: ///��ҹͥԺ
        case 8381: ///��ʯׯ԰
        case 8386: ///ԾӰ��԰
        case 8385: ///���¼���
        case 8384: ///����֮��
        case 8383: ///�������
        case 8382: ///���¾ƹ�
        case 8395: ///��˹���߸�
        case 8397: ///����ƽ̨
        case 8398: ///����ƽ̨
        case 8431: ///������
        case 8436: ///���乫԰
        case 8441: ///�¹ⳤ��
        case 8434: ///������С��
        case 8345: ///��ϯ����ʦ��ׯ԰
        case 8461: ///����ʥ��
        case 8487: ///����ƽ̨
        case 8496: ///��������
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
