/**
 *
 * @File : npc_teleport.cpp
 *
 * @Authors : Wilibald09
 *
 * @Date : 19/08/2008
 *
 * @Version : 1.2
 *
 **/



#include "Creature.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "World.h"

#include "WorldSession.h"
#include "Log.h"
#include "sc_npc_teleport.h"
#include <sstream>
#include "windows.h"
#define GOSSIP_SHOW_DEST        1000
#define GOSSIP_TELEPORT         1001
#define GOSSIP_NEXT_PAGEC       1002
#define GOSSIP_PREV_PAGEC       1003
#define GOSSIP_NEXT_PAGED       1004
#define GOSSIP_PREV_PAGED       1005
#define GOSSIP_MAIN_MENU        1006

#define SPELL_ID_PASSIVE_RESURRECTION_SICKNESS  15007
#define SPELL_VISUAL_TELEPORT   35517

#define NB_ITEM_PAGE            15
#define MSG_CAT                 9000000
#define MSG_DEST                9000001

using namespace nsNpcTel;


namespace 
{
    Page PageC, PageD;
    Page Cat;

    std::string next_page;
    std::string previous_page;
    std::string main_menu;
    std::string no_required_level;
    std::string no_enough_money;
    std::string in_combat;


    std::string string_To_UTF8(const std::string & str)
    {
        int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

        wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
        ZeroMemory(pwBuf, nwLen * 2 + 2);

        ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

        int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

        char * pBuf = new char[nLen + 1];
        ZeroMemory(pBuf, nLen + 1);

        ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

        std::string retStr(pBuf);

        delete[]pwBuf;
        delete[]pBuf;

        pwBuf = NULL;
        pBuf = NULL;

        return retStr;
    }

    std::string UTF8_To_string(const std::string & str)
    {
        int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

        wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
        memset(pwBuf, 0, nwLen * 2 + 2);

        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

        int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

        char * pBuf = new char[nLen + 1];
        memset(pBuf, 0, nLen + 1);

        WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

        std::string retStr = pBuf;

        delete[]pBuf;
        delete[]pwBuf;

        pBuf = NULL;
        pwBuf = NULL;

        return retStr;
    }

    void GetMenuStr() {
                next_page = string_To_UTF8("-> [下一页]");
                previous_page = string_To_UTF8("<- [上一页]");
                main_menu = string_To_UTF8("<= [主菜单]");
                no_required_level = string_To_UTF8("您没有达到等级要求. 需要等级达到");
                no_enough_money = string_To_UTF8("您没有足够的金币. 传送费用是");
                in_combat = string_To_UTF8("您还在战斗中.请稍后再来");
            //TC_LOG_INFO("server.loading", " menu locale ");
    }

    // Conversion function int->string
    std::string ConvertStr(const int64 &val)
    {
        std::ostringstream ostr;
        ostr << val;
        return ostr.str();
    }

    // Conversion function intMoney->stringMoney
    std::string ConvertMoney(const uint32 &Money)
    {
        std::string Str(ConvertStr(Money));
        uint32 SizeStr = Str.length();

        if (SizeStr > 4)
            Str = Str.insert(Str.length()-4, "g");
        if (SizeStr > 2)
            Str = Str.insert(Str.length()-2, "s");
        Str += "c";

        return Str;
    }



    // Teleport Player
    void Teleport(Player * const player, const uint16 &map,
                  const float &X, const float &Y, const float &Z, const float &orient)
    {
        player->CastSpell(player, SPELL_VISUAL_TELEPORT, true);
        player->TeleportTo(map, X, Y, Z, orient);
    }

    // Display categories
    void AffichCat(Player * const player, Creature * const creature)
    {
        if (PageC[player] > 0)
            AddGossipItemFor(player, 7, previous_page, GOSSIP_PREV_PAGEC, 0);

        VCatDest_t i (PageC[player] * NB_ITEM_PAGE);
        for ( ; i < TabCatDest.size() && i < (NB_ITEM_PAGE * (PageC[player] + 1)); ++i)
        {
            if (TabCatDest[i].IsAllowedToTeleport(player))
                AddGossipItemFor(player, 7, TabCatDest[i].GetName(player->IsGameMaster()).c_str(), GOSSIP_SHOW_DEST, i);
        }

        if (i < TabCatDest.size())
            AddGossipItemFor(player, 7, next_page, GOSSIP_NEXT_PAGEC, 0);

        SendGossipMenuFor(player, MSG_CAT, creature->GetGUID());
    }

    // Display destination categories
    void AffichDest(Player * const player, Creature * const creature)
    {
        if (PageD[player] > 0)
            AddGossipItemFor(player, 7, previous_page, GOSSIP_PREV_PAGED, 0);

        CatDest::VDest_t i (PageD[player] * NB_ITEM_PAGE);
        for ( ; i < TabCatDest[Cat[player]].size() && i < (NB_ITEM_PAGE * (PageD[player] + 1)); ++i)
        {
            AddGossipItemFor(player, 5, TabCatDest[Cat[player]].GetDest(i).m_name.c_str(), GOSSIP_TELEPORT, i);
        }

        if (i < TabCatDest[Cat[player]].size())
            AddGossipItemFor(player, 7, next_page, GOSSIP_NEXT_PAGED, 0);

        if (CatDest::CountOfCategoryAllowedBy(player) > 1)
            AddGossipItemFor(player, 7, main_menu, GOSSIP_MAIN_MENU, 0);

        SendGossipMenuFor(player, MSG_DEST, creature->GetGUID());
    }

    // Verification before teleportation
    void ActionTeleport(Player * const player, Creature * const creature, const uint32 &id)
    {
        Dest dest (TabCatDest[Cat[player]].GetDest(id));

        if (player->getLevel() < dest.m_level && !player->IsGameMaster())
        {
            std::string msg (no_required_level + ConvertStr(dest.m_level) + "."); // You do not have the required level. This destination requires level
            creature->Whisper(msg.c_str(), LANG_UNIVERSAL, player);
            return;
        }

        if (player->GetMoney() < dest.m_cost && !player->IsGameMaster())
        {
            std::string msg (no_enough_money + ConvertMoney(dest.m_cost) + ".");//You do not have enough money. The price for teleportation is 
            creature->Whisper(msg.c_str(), LANG_UNIVERSAL, player);
            return;
        }

        if (!player->IsGameMaster() && dest.m_cost)
            player->ModifyMoney(-1 * dest.m_cost);

        Teleport(player, dest.m_map, dest.m_X, dest.m_Y, dest.m_Z, dest.m_orient);
    }
}

class npc_teleport : public CreatureScript
{
public:
    npc_teleport() : CreatureScript("npc_teleport") {}

bool OnGossipHello(Player *player, Creature *creature) override
{
    if (next_page == "") {
        GetMenuStr();
    }
    
    
    PageC(player) = PageD(player) = Cat(player) = 0;

    if(player->IsInCombat())
    {
        CloseGossipMenuFor(player);
        creature->Whisper(in_combat, LANG_UNIVERSAL, player); //You are in combat. Come back later
        return true;
    }

    AffichCat(player, creature);
    return true;
}

bool OnGossipSelect(Player *player, Creature *creature, uint32 sender, uint32 param)
{
    player->PlayerTalkClass->ClearMenus();
    switch(sender)
    {
      // Display destinations
      case GOSSIP_SHOW_DEST:
        Cat(player) = param;
        AffichDest(player, creature);
        break;

      // Previous categories page
      case GOSSIP_PREV_PAGEC:
        --PageC(player);
        AffichCat(player, creature);
        break;

      // Next page categories
      case GOSSIP_NEXT_PAGEC:
        ++PageC(player);
        AffichCat(player, creature);
        break;

      // Previous destinations page
      case GOSSIP_PREV_PAGED:
        --PageD(player);
        AffichDest(player, creature);
        break;

      // Next destination page
      case GOSSIP_NEXT_PAGED:
        ++PageD(player);
        AffichDest(player, creature);
        break;

      // Display main menu
      case GOSSIP_MAIN_MENU:
        OnGossipHello(player, creature);
        break;

      // Teleportation
      case GOSSIP_TELEPORT:
        CloseGossipMenuFor(player);
        if(player->HasAura(SPELL_ID_PASSIVE_RESURRECTION_SICKNESS)) {
            creature->CastSpell(player,38588,false); // Healing effect
            player->RemoveAurasDueToSpell(SPELL_ID_PASSIVE_RESURRECTION_SICKNESS);
        }

        ActionTeleport(player, creature, param);
        break;
    }
    return true;
}
};

void AddSC_npc_teleport()
{
    new npc_teleport;
}
// End of TeleNPC2
