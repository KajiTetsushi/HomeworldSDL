// =============================================================================
//  MissileDestroyer.c
// =============================================================================
//  Copyright Relic Entertainment, Inc. All rights reserved.
//  Created 6/30/1997 by gshaw
// =============================================================================

#include "MissileDestroyer.h"

#include "Ammunition.h"
#include "Attack.h"
#include "Collision.h"
#include "Battle.h"
#include "DefaultShip.h"
#include "Gun.h"
#include "ObjTypes.h"
#include "SoundEvent.h"
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
        bool canVolleyFire = gunStatic->guntype == GUN_MissileLauncher ? TRUE : gunStatic->canVolleyFire;
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

bool MissileDestroyerSpecialTarget(Ship *ship, void *custom)
{
    ShipStaticInfo *shipStaticInfo = (ShipStaticInfo *)ship->staticinfo;
    MissileDestroyerSpec *spec = (MissileDestroyerSpec *)ship->ShipSpecifics;
    MissileDestroyerStat *stat = (MissileDestroyerStat *)shipStaticInfo->custstatinfo;

    return volleyFireSpecialTarget(
        ship,
        custom,
        &spec->volleyFire,
        &stat->volleyFire,
        volleyFireShootAllTargetsLumbering,
        missileShoot,
        BCE_COMM_MissleDest_VolleyAttack);
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
