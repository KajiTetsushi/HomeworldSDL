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
    { "ammo.reloadCooldown",    scriptSetReal32CB,    &(ammunitionStat.reloadCooldown),    &ammunitionStat },

    END_SCRIPT_STRUCT_ENTRY
};

bool findIndexOfLeastNoOfRoundsGun(GunInfo *gunInfo, GunStaticInfo *gunStaticInfo, AmmunitionSpec *spec, AmmunitionStat *stat);

//#region Setters
void ammunitionSetLastReloadedTimestamp(AmmunitionSpec *spec, real32 timestamp)
{
    spec->lastReloadedOn = timestamp;
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
 * @brief Determines if the ship's reloading is no longer in cooldown after a previous reload.
 * @param spec Ammunition instance.
 * @param stat Ammunition configuration.
 * @return TRUE if still in cooldown, FALSE if free from cooldown.
 */
bool ammunitionCanReloadNow(
    AmmunitionSpec *spec, // Ammunition instance.
    AmmunitionStat *stat) // Ammunition configuration.
{
    real32 durationSinceLastReload = universe.totaltimeelapsed - spec->lastReloadedOn;

    return durationSinceLastReload > stat->reloadCooldown;
}

/**
 * @brief Increments the weapon with the least number of rounds by 1.
 * @param spec Ammunition instance.
 * @param stat Ammunition configuration.
 * @param gunInfo Array of gun instances.
 * @param gunStaticInfo Array of gun configurations.
 */
void ammunitionReload(
    AmmunitionSpec *spec,         // Ammunition instance.
    AmmunitionStat *stat,         // Ammunition configuration.
    GunInfo *gunInfo,             // Array of gun instances.
    GunStaticInfo *gunStaticInfo) // Array of gun configurations.
{
    bool canReloadNow = ammunitionCanReloadNow(spec, stat);

    if (!canReloadNow)
    {
        return;
    }

    ammunitionSetLastReloadedTimestamp(spec, universe.totaltimeelapsed);

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
