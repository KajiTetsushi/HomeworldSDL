// =============================================================================
//  MissileDestroyer.c
// =============================================================================
//  Copyright Relic Entertainment, Inc. All rights reserved.
//  Created 6/30/1997 by gshaw
// =============================================================================

#include "MissileDestroyer.h"

#include "AIShip.h"
#include "AITrack.h"
#include "Ammunition.h"
#include "Attack.h"
#include "Collision.h"
#include "Battle.h"
#include "DefaultShip.h"
#include "Gun.h"
#include "ObjTypes.h"
#include "StatScript.h"
#include "Universe.h"
#include "Vector.h"
#include "VolleyFire.h"

MissileDestroyerStat r1MissileDestroyerStat;
MissileDestroyerStat r2MissileDestroyerStat;

/**
 * @deprecated We should use the new namespaced API from here on out.
 */
scriptStructEntry MissileDestroyerScriptTable[] =
{
    { "MissileRegenerateTime",    scriptSetReal32CB,    &(r1MissileDestroyerStat.missileRegenerateTime),     &(r1MissileDestroyerStat) },
    { "MissileVolleyTime",        scriptSetReal32CB,    &(r1MissileDestroyerStat.missileVolleyTime),         &(r1MissileDestroyerStat) },
    { "MissileLagVolleyTime",     scriptSetReal32CB,    &(r1MissileDestroyerStat.missileLagVolleyTime),      &(r1MissileDestroyerStat) },

    END_SCRIPT_STRUCT_ENTRY
};

/**
 * @brief Initializes the ship using legacy attributes, if all of them were provided.
 * @deprecated Backwards compatibility
 * @param shipStaticInfo Ship static info.
 * @param stat Ship statics.
 */
void MissileDestroyerStatInitLegacy(struct ShipStaticInfo *shipStaticInfo, MissileDestroyerStat *stat)
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
    stat->volleyFire.abilityCooldown = stat->missileVolleyTime;
    stat->volleyFire.reloadCooldown = stat->missileLagVolleyTime;
    stat->ammunition.reloadCooldown = stat->missileRegenerateTime;

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

void MissileDestroyerStatInit(char *directory, char *filename, struct ShipStaticInfo *shipStaticInfo)
{
    MissileDestroyerStat *stat = (shipStaticInfo->shiprace == R1) ? &r1MissileDestroyerStat : &r2MissileDestroyerStat;

    scriptSetStruct(directory, filename, AmmunitionStatScriptTable, (ubyte *)&stat->ammunition);
    scriptSetStruct(directory, filename, VolleyFireStatScriptTable, (ubyte *)&stat->volleyFire);
    scriptSetStruct(directory, filename, MissileDestroyerScriptTable, (ubyte *)stat);

    MissileDestroyerStatInitLegacy(shipStaticInfo, stat);

    shipStaticInfo->custstatinfo = stat;

    udword tacticIndex;
    for (tacticIndex = 0; tacticIndex < NUM_TACTICS_TYPES; tacticIndex++)
    {
        stat->wpnRange[tacticIndex] = shipStaticInfo->bulletRange[tacticIndex];
        stat->wpnMinRange[tacticIndex] = shipStaticInfo->minBulletRange[tacticIndex] * 0.9f;
    }
}

void MissileDestroyerInit(Ship *ship)
{
    MissileDestroyerSpec *spec = (MissileDestroyerSpec *)ship->ShipSpecifics;

    volleyFireInit(&spec->volleyFire);
    ammunitionInit(&spec->ammunition);
}

void MissileDestroyerAttack(Ship *ship, SpaceObjRotImpTarg *target, real32 maxdist)
{
    ShipStaticInfo *shipstaticinfo = (ShipStaticInfo *)ship->staticinfo;
    MissileDestroyerStat *stat = (MissileDestroyerStat *)shipstaticinfo->custstatinfo;

    attackStraightForward(ship, target, stat->wpnRange[ship->tacticstype], stat->wpnMinRange[ship->tacticstype]);
}

void MissileDestroyerAttackPassive(Ship *ship, Ship *target, bool rotate)
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

void MissileDestroyerHousekeep(Ship *ship)
{
    ShipStaticInfo *shipStaticInfo = (ShipStaticInfo *)ship->staticinfo;
    MissileDestroyerSpec *spec = (MissileDestroyerSpec *)ship->ShipSpecifics;
    MissileDestroyerStat *stat = (MissileDestroyerStat *)shipStaticInfo->custstatinfo;

    bool canReload = volleyFireCanReloadNow(&spec->volleyFire, &stat->volleyFire);

    if (!canReload)
    {
        return;
    }

    GunInfo *gunInfo = ship->gunInfo;
    GunStaticInfo *gunStaticInfo = shipStaticInfo->gunStaticInfo;

    ammunitionReload(
        &spec->ammunition,
        &stat->ammunition,
        gunInfo,
        gunStaticInfo);
}

bool MissileDestroyerSpecialTargetAttack(Ship *ship, VolleyFireSpec *volleyFireSpec, VolleyFireStat *volleyFireStat, SelectAnyCommand *targets)
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
    bool hasAttemptedToFireAtLeastOnce = FALSE;

    // Ranging
    vector trajectory;
    real32 rangeToTarget;
    real32 maxRangeToTarget;

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

        // Calculate range to target.
        vecSub(trajectory, ship->collInfo.collPosition, target->collInfo.collPosition);
        rangeToTarget = RangeToTarget(ship, target, &trajectory);
        maxRangeToTarget = gun->gunstatic->bulletrange * 0.9f;

        // Break early if this weapon is out of range to target.
        if (rangeToTarget >= maxRangeToTarget)
        {
            continue;
        }

        hasAttemptedToFireAtLeastOnce = TRUE;

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
        aitrackZeroVelocity(ship);
        return FALSE;
    }
    // Stop here and reset everything if the weapons are out of ammunition.
    else if (hasAttemptedToFireAtLeastOnce)
    {
        volleyFireSetBattleChatterBusy(volleyFireSpec, FALSE);
        volleyFireSetCurrentTargetIndex(volleyFireSpec, 0);

        return TRUE;
    }
    // Stop here and attempt to approach first target in the selection list.
    else
    {
        Ship *nextTarget = (Ship *)targets->TargetPtr[0];
        udword aishipFlags = AISHIP_FastAsPossible | AISHIP_PointInDirectionFlying;
        real32 limitVelocity = 0.0f;
        aishipFlyToShipAvoidingObjs(ship, nextTarget, aishipFlags, limitVelocity);

        return FALSE;
    }
}

bool MissileDestroyerSpecialTarget(Ship *ship,void *custom)
{
    ShipStaticInfo *shipStaticInfo = (ShipStaticInfo *)ship->staticinfo;
    MissileDestroyerSpec *spec = (MissileDestroyerSpec *)ship->ShipSpecifics;
    MissileDestroyerStat *stat = (MissileDestroyerStat *)shipStaticInfo->custstatinfo;
    SelectAnyCommand *targets = (SelectAnyCommand *)custom;

    return volleyFireSpecialTarget(
        ship,
        targets,
        &spec->volleyFire,
        &stat->volleyFire,
        BCE_COMM_MissleDest_VolleyAttack,
        MissileDestroyerSpecialTargetAttack);
}

CustShipHeader MissileDestroyerHeader =
{
    MissileDestroyer,
    sizeof(MissileDestroyerSpec),
    MissileDestroyerStatInit,
    NULL,
    MissileDestroyerInit,
    NULL,
    MissileDestroyerAttack,
    DefaultShipFire,
    MissileDestroyerAttackPassive,
    NULL,
    MissileDestroyerSpecialTarget,
    MissileDestroyerHousekeep,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
