//
//  Ammunition.c
//  Manages ammunition logic such as reloading behavior.
//
//  Created on 2020-12-21 by KajiTetsushi
//  Copyleft HomeworldSDL.
//

#include "Ammunition.h"

#include "SpaceObj.h"
#include "StatScript.h"
#include "Types.h"
#include "Universe.h"

static AmmunitionStat ammunitionStat;

scriptStructEntry AmmunitionStatScriptTable[] =
{
    { "ammo.reloadDelay",    scriptSetReal32CB,    &(ammunitionStat.reloadDelay),    &ammunitionStat },

    END_SCRIPT_STRUCT_ENTRY
};

bool findIndexOfLeastNoOfRoundsGun(GunInfo *gunInfo, GunStaticInfo *gunStaticInfo, AmmunitionSpec *spec, AmmunitionStat *stat);

//#region Setters
void ammunitionSetLastReloadedTimestamp(AmmunitionSpec *spec, real32 timestamp)
{
    spec->lastReloadedTimestamp = timestamp;
}
//#endregion

/**
 * @brief Initializes the ammunition manager.
 * @param spec Ammunition instance.
 */
void ammunitionInit(AmmunitionSpec *spec)
{
    ammunitionSetLastReloadedTimestamp(spec, 0.0f);
}

/**
 * @brief Reloads one weapon, i.e. the weapon with the least number of rounds, with one round of ammunition.
 * @param spec Ammunition instance.
 * @param stat Ammunition configuration.
 * @param gunInfo Array of gun instances.
 * @param gunStaticInfo Array of gun configurations.
 */
void ammunitionReloadLeastNoOfRoundsGun(
    AmmunitionSpec *spec,
    AmmunitionStat *stat,
    GunInfo *gunInfo,
    GunStaticInfo *gunStaticInfo)
{
    if ((universe.totaltimeelapsed - spec->lastReloadedTimestamp) <= stat->reloadDelay)
    {
        return;
    }

    spec->lastReloadedTimestamp = universe.totaltimeelapsed;

    sdword leastNoOfRoundsGunIndex = findIndexOfLeastNoOfRoundsGun(gunInfo, gunStaticInfo, spec, stat);

    if (leastNoOfRoundsGunIndex < 0)
    {
        return;
    }

    gunInfo->guns[leastNoOfRoundsGunIndex].numMissiles++;
}

bool findIndexOfLeastNoOfRoundsGun(
    GunInfo *gunInfo,
    GunStaticInfo *gunStaticInfo,
    AmmunitionSpec *spec,
    AmmunitionStat *stat)
{
    sdword noOfGuns = gunInfo->numGuns;
    sdword gunIndex;
    sdword leastNoOfRoundsGunIndex = -1; // Index of the gun with the least number of rounds.
    sdword leastNoOfRounds = -1;         // Stores the eventual value of the least number of rounds through iteration.

    Gun *gun;
    GunStatic *gunStat;

    for (gunIndex = 0; gunIndex < noOfGuns; gunIndex++)
    {
        gunStat = &gunStaticInfo->gunstatics[gunIndex];

        // Stop here if the weapon doesn't have ammunition capacity as an attribute.
        if (gunStat->maxMissiles == 0)
        {
            continue;
        }

        // Attempt to find the gun with the smallest number of rounds.
        gun = &gunInfo->guns[gunIndex];

        leastNoOfRounds = leastNoOfRounds <= 0
            ? gunStat->maxMissiles
            : leastNoOfRounds;

        if (gun->numMissiles >= gunStat->maxMissiles)
        {
            continue;
        }

        if (gun->numMissiles >= leastNoOfRounds)
        {
            continue;
        }

        leastNoOfRounds = gun->numMissiles;
        leastNoOfRoundsGunIndex = gunIndex;
    }

    return leastNoOfRoundsGunIndex;
}
