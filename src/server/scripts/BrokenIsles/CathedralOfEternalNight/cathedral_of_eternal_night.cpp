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
#include "cathedral_of_eternal_night.h"

enum Spells
{
    //AGRONOX
    SPELL_TIMBER_SMASH = 235751,
    SPELL_SUCCULENT_LASHERS = 236639,//Only Mythic Mode
    SPELL_POISONOUS_SPORES = 236524,
    SPELL_FULMINATING_LASHERS = 236527,

    //THRASHBITE_THE_SCORNFUL
    SPELL_HEAVE_CUDGEL = 243124,
    SPELL_PULVERIZING_CUDGEL = 237276,
    SPELL_SCORNFUL_CHARGE = 238462,
    SPELL_SCORNFUL_GAZE = 237726,
    SPELL_TASTE_OF_BLOOD = 213738,
    //DOMATRAX
    //MEPHISTROTH


};

enum Events
{
    EVENT_NONE,

    EVENT_LVR_1,
    EVENT_LVR_2,
    EVENT_OUTRO_2,
    EVENT_OUTRO_3
};

void AddSC_cathedral_of_eternal_night()
{
    // NPC Scripts
}
