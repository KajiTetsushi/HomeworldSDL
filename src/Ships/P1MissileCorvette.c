// =============================================================================
//  P1MissileCorvette.c
// =============================================================================
//  Copyright Relic Entertainment, Inc. All rights reserved.
//  Created 5/06/1998 by ddunlop
// =============================================================================

#include "P1MissileCorvette.h"

#include "Ammunition.h"
#include "Attack.h"
#include "Battle.h"
#include "DefaultShip.h"
#include "Gun.h"
#include "ObjTypes.h"
#include "StatScript.h"
#include "VolleyFire.h"

P1MissileCorvetteStat p1MissileCorvetteStat;

/**
 * @deprecated We should use the new namespaced API from here on out.
 */
scriptStructEntry P1MissileCorvetteScriptTable[] =
{
    { "MissileRegenerateTime",    scriptSetReal32CB,    &(p1MissileCorvetteStat.missileRegenerateTime),    &(p1MissileCorvetteStat) },
    { "MissileVolleyTime",        scriptSetReal32CB,    &(p1MissileCorvetteStat.missileVolleyTime),        &(p1MissileCorvetteStat) },
    { "MissileLagVolleyTime",     scriptSetReal32CB,    &(p1MissileCorvetteStat.missileLagVolleyTime),     &(p1MissileCorvetteStat) },

    END_SCRIPT_STRUCT_ENTRY
};

/**
 * @brief Initializes the ship using legacy attributes, if all of them were provided.
 * @deprecated Backwards compatibility
 * @param shipStaticInfo Ship static info.
 * @param stat Ship statics.
 */
void P1MissileCorvetteStatInitLegacy(struct ShipStaticInfo *shipStaticInfo, P1MissileCorvetteStat *stat)
{
    // Stop here if none of these were declared in the *.shp file.
    // - MissileVolleyTime
    // - MissileLagVolleyTime
    // - MissileRegenerateTime
    if (!(stat->missileVolleyTime && stat->missileLagVolleyTime && stat->missileRegenerateTime))
    {
        return;
    }

    // Assign fallbacks for the essential simple values.
    shipStaticInfo->canVolleyFire = TRUE;
    stat->volleyFire.fireDelay = stat->missileVolleyTime;
    stat->volleyFire.abilityCooldown = stat->missileLagVolleyTime;
    stat->ammunition.reloadDelay = stat->missileRegenerateTime;

    // Override only the missile launchers to allow volley fire.
    sdword noOfWeapons = shipStaticInfo->gunStaticInfo->numGuns;
    sdword weaponIndex;
    GunStatic *gunStatic;

    for (weaponIndex = 0; weaponIndex < noOfWeapons; weaponIndex++)
    {
        gunStatic = &shipStaticInfo->gunStaticInfo->gunstatics[weaponIndex];
        bool8 canVolleyFire = gunStatic->guntype == GUN_MissileLauncher ? TRUE : gunStatic->canVolleyFire;
        gunStatic->canVolleyFire = canVolleyFire;
    }
}

void P1MissileCorvetteStatInit(char *directory, char *filename, struct ShipStaticInfo *shipStaticInfo)
{
    P1MissileCorvetteStat *stat = &p1MissileCorvetteStat;

    memset(stat, 0, sizeof(P1MissileCorvetteStat));

    scriptSetStruct(directory, filename, AttackSideStepParametersScriptTable, (ubyte *)&stat->sidestepParameters);
    scriptSetStruct(directory, filename, AmmunitionStatScriptTable, (ubyte *)&stat->ammunition);
    scriptSetStruct(directory, filename, VolleyFireStatScriptTable, (ubyte *)&stat->volleyFire);
    scriptSetStruct(directory, filename, P1MissileCorvetteScriptTable, (ubyte *)stat);

    P1MissileCorvetteStatInitLegacy(shipStaticInfo, stat);

    shipStaticInfo->custstatinfo = stat;

    udword tacticIndex;
    for (tacticIndex = 0; tacticIndex < NUM_TACTICS_TYPES; tacticIndex++)
    {
        stat->wpnRange[tacticIndex] = shipStaticInfo->bulletRange[tacticIndex];
        stat->wpnMinRange[tacticIndex] = shipStaticInfo->minBulletRange[tacticIndex] * 0.9f;
    }
}

void P1MissileCorvetteInit(Ship *ship)
{
    P1MissileCorvetteSpec *spec = (P1MissileCorvetteSpec *)ship->ShipSpecifics;

    attackSideStepInit(&spec->attacksidestep);
    volleyFireInit(&spec->volleyFire);
    ammunitionInit(&spec->ammunition);
}

void P1MissileCorvetteAttack(Ship *ship, SpaceObjRotImpTarg *target, real32 maxdist)
{
    P1MissileCorvetteSpec *spec = (P1MissileCorvetteSpec *)ship->ShipSpecifics;
    P1MissileCorvetteStat *stat = (P1MissileCorvetteStat *)((ShipStaticInfo *)ship->staticinfo)->custstatinfo;

    attackSideStep(ship, target, &spec->attacksidestep, &stat->sidestepParameters);
}

void P1MissileCorvetteAttackPassive(Ship *ship, Ship *target, bool rotate)
{
    if ((rotate) & ((bool)((ShipStaticInfo *)(ship->staticinfo))->rotateToRetaliate))
    {
        attackPassiveRotate(ship, target);
    }
    else
    {
        attackPassive(ship, target);
    }
}

void P1MissileCorvetteHousekeep(Ship *ship)
{
    ShipStaticInfo *shipStaticInfo = (ShipStaticInfo *)ship->staticinfo;
    P1MissileCorvetteSpec *spec = (P1MissileCorvetteSpec *)ship->ShipSpecifics;
    P1MissileCorvetteStat *stat = (P1MissileCorvetteStat *)shipStaticInfo->custstatinfo;
    bool isVolleyFireReady = volleyFireIsReady(&spec->volleyFire, &stat->volleyFire);

    if (!isVolleyFireReady)
    {
        return;
    }

    GunInfo *gunInfo = ship->gunInfo;
    GunStaticInfo *gunStaticInfo = shipStaticInfo->gunStaticInfo;

    ammunitionReloadLeastNoOfRoundsGun(
        &spec->ammunition,
        &stat->ammunition,
        gunInfo,
        gunStaticInfo);
}

bool P1MissileCorvetteSpecialTargetAttack(Ship *ship, VolleyFireSpec *volleyFireSpec, VolleyFireStat *volleyFireStat, SelectAnyCommand *targets)
{
    ShipStaticInfo *shipStaticInfo = (ShipStaticInfo *)ship->staticinfo;

    // Targets
    SpaceObjRotImpTarg *target;
    sdword noOfShipsToTarget = targets->numTargets;

    // Weapons
    Gun *gun;
    GunStatic *gunStatic;
    sdword gunIndex;
    sdword noOfGuns = ship->gunInfo->numGuns;
    bool hasFiredAtLeastOnce = FALSE;

    // Correct the current target index if it's out of bounds from the last attempt.
    if (volleyFireSpec->currentTargetIndex >= noOfShipsToTarget)
    {
        volleyFireSetCurrentTargetIndex(volleyFireSpec, 0);
    }

    // Attempt to attack each target.
    for (gunIndex = 0; gunIndex < noOfGuns; gunIndex++)
    {
        target = targets->TargetPtr[volleyFireSpec->currentTargetIndex];
        gunStatic = &shipStaticInfo->gunStaticInfo->gunstatics[gunIndex];
        gun = &ship->gunInfo->guns[gunIndex];

        // Break early if this weapon cannot volley fire.
        if (!(gunStatic->canVolleyFire))
        {
            continue;
        }

        // Break early if this weapon has no ammunition remaining.
        if (!gunHasAmmunition(gun))
        {
            continue;
        }

        hasFiredAtLeastOnce = TRUE;
        missileShoot(ship, gun, target);

        sdword nextTargetIndex = volleyFireGetNextTargetIndex(volleyFireSpec->currentTargetIndex, noOfShipsToTarget);
        volleyFireSetCurrentTargetIndex(volleyFireSpec, nextTargetIndex);
    }

    if (hasFiredAtLeastOnce)
    {
        return FALSE;
    }
    // Stop here and reset everything if the weapons are out of ammunition.
    else
    {
        volleyFireSetBattleChatterBusy(volleyFireSpec, FALSE);
        volleyFireSetCurrentTargetIndex(volleyFireSpec, 0);

        return TRUE;
    }
}

bool P1MissileCorvetteSpecialTarget(Ship *ship, void *custom)
{
    ShipStaticInfo *shipStaticInfo = (ShipStaticInfo *)ship->staticinfo;
    P1MissileCorvetteSpec *spec = (P1MissileCorvetteSpec *)ship->ShipSpecifics;
    P1MissileCorvetteStat *stat = (P1MissileCorvetteStat *)shipStaticInfo->custstatinfo;
    SelectAnyCommand *targets = (SelectAnyCommand *)custom;

    return volleyFireSpecialTarget(
        ship,
        targets,
        &spec->volleyFire,
        &stat->volleyFire,
        BCE_COMM_MissleDest_VolleyAttack,
        P1MissileCorvetteSpecialTargetAttack);
}

CustShipHeader P1MissileCorvetteHeader =
{
    P1MissileCorvette,
    sizeof(P1MissileCorvetteSpec),
    P1MissileCorvetteStatInit,
    NULL,
    P1MissileCorvetteInit,
    NULL,
    P1MissileCorvetteAttack,
    DefaultShipFire,
    P1MissileCorvetteAttackPassive,
    NULL,
    P1MissileCorvetteSpecialTarget,
    P1MissileCorvetteHousekeep,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
