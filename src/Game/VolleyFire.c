//
//  VolleyFire.c
//  Handles behavior involving the volley fire speciabl ability.
//
//  Created on 2020-12-13 by Kaji Tetsushi
//  Copyleft HomeworldSDL.
//

#include "VolleyFire.h"

#include "AIShip.h"
#include "AITrack.h"
#include "Battle.h"
#include "Gun.h"
#include "ShipSelect.h"
#include "SoundEvent.h"
#include "StatScript.h"
#include "Universe.h"

static VolleyFireStat volleyFireStat;

scriptStructEntry VolleyFireStatScriptTable[] =
{

    { "volleyFire.fireDelay",          scriptSetReal32CB,    &(volleyFireStat.fireDelay),          &volleyFireStat },
    { "volleyFire.abilityCooldown",    scriptSetReal32CB,    &(volleyFireStat.abilityCooldown),    &volleyFireStat },

    END_SCRIPT_STRUCT_ENTRY
};

//#region Setters
void volleyFireSetCurrentTargetIndex(VolleyFireSpec *spec, sdword index)
{
    spec->currentTargetIndex = index;
}

void volleyFireSetBattleChatterBusy(VolleyFireSpec *spec, bool busy)
{
    spec->battleChatterBusy = busy;
}

void volleyFireSetLastAttemptedTimestamp(VolleyFireSpec *spec, real32 timestamp)
{
    spec->lastAttemptedOn = timestamp;
}

void volleyFireSetLastActivatedTimestamp(VolleyFireSpec *spec, real32 timestamp)
{
    spec->lastActivatedOn = timestamp;
}
//#endregion

sdword volleyFireGetNextTargetIndex(sdword currentTargetIndex, sdword noOfTargets)
{
    sdword nextTargetIndex = currentTargetIndex + 1;

    return nextTargetIndex < noOfTargets ? nextTargetIndex : 0;
}

void volleyFireBattleChatterAttempt(Ship *ship, sdword event)
{
    if (ship->playerowner->playerIndex == universe.curPlayerIndex)
    {
        if (battleCanChatterAtThisTime(event, ship))
        {
            battleChatterAttempt(SOUND_EVENT_DEFAULT, event, ship, SOUND_EVENT_DEFAULT);
        }
    }
}

/**
 * @brief Determines if the ship's volley fire special ability is still in cooldown.
 * @param ship Volley fire instance.
 * @return TRUE if still in cooldown, FALSE if free from cooldown.
 */
bool volleyFireIsReady(VolleyFireSpec *spec, VolleyFireStat *stat)
{
    real32 durationSinceLastAttempted = universe.totaltimeelapsed - spec->lastAttemptedOn;

    return durationSinceLastAttempted > stat->abilityCooldown;
}

/**
 * @brief Initializes the volley fire special ability.
 * @param spec Volley fire instance.
 */
void volleyFireInit(VolleyFireSpec *spec)
{
    volleyFireSetCurrentTargetIndex(spec, 0);
    volleyFireSetBattleChatterBusy(spec, FALSE);
    volleyFireSetLastAttemptedTimestamp(spec, -100000.0f);
    volleyFireSetLastActivatedTimestamp(spec, 0.0f);
}

/**
 * @brief Invokes the volley fire special ability with a user-defined weapons behavior.
 * @param ship Ship reference.
 * @param custom Targets selected to attack.
 * @param spec Volley fire instance.
 * @param stat Volley fire configuration.
 * @param shootTargets Ship behavior to use when attacking all selected targets.
 * @param shoot Weapon behavior to use to shoot each target at least once.
 * @param battleChatter Battle chatter event to use when the ability has been activated.
 * @return Indication of whether the ship's volley fire sequence is complete.
 */
bool volleyFireSpecialTarget(
    Ship *ship,                        // Ship.
    void *custom,                      // Targets selected to attack.
    VolleyFireSpec *spec,              // Volley fire instance.
    VolleyFireStat *stat,              // Volley fire configuration.
    ShootTargetsCallback shootTargets, // Ship behavior to use when attacking all selected targets.
    ShootCallback shoot,               // Weapon behavior to use to shoot each target at least once.
    sdword battleChatterEvent)         // Battle chatter event to use when the ability has been activated.
{
    // Stop here if the user-defined callbacks aren't provided, since the volley fire won't work, anyway.
    if (shootTargets == NULL || shoot == NULL)
    {
        return TRUE;
    }

    volleyFireSetLastAttemptedTimestamp(spec, universe.totaltimeelapsed);

    // Stop here if the "volley reload time" has passed
    if ((universe.totaltimeelapsed - spec->lastActivatedOn) <= stat->fireDelay)
    {
        return FALSE;
    }

    volleyFireSetLastActivatedTimestamp(spec, universe.totaltimeelapsed);

    SpaceObjRotImpTarg *target;
    SelectAnyCommand *targets = (SelectAnyCommand *)custom;
    sdword noOfShipsToTarget = targets->numTargets;

    // Stop here and reset everything if no target was selected.
    if (noOfShipsToTarget == 0)
    {
        volleyFireSetBattleChatterBusy(spec, FALSE);
        volleyFireSetCurrentTargetIndex(spec, 0);

        return TRUE;
    }

    // Attempt voice feedback if the ship's not busy.
    if (!(spec->battleChatterBusy))
    {
        volleyFireSetBattleChatterBusy(spec, TRUE);

        if (battleChatterEvent != NULL)
        {
            volleyFireBattleChatterAttempt(ship, battleChatterEvent);
        }
    }

    return shootTargets(ship, spec, targets, shoot);
}

/**
 * @brief Sets the ship to volley fire without changing its velocity. Use for ships with hit-and-run capabilities such as fighters and corvettes.
 * @param ship Ship.
 * @param spec Volley fire instance.
 * @param targets All targets selected when the volley fire special ability as activated.
 * @param shoot Weapon behavior to use to shoot each target at least once.
 * @return Indication of whether the ship's volley fire sequence is complete.
 */
bool volleyFireShootAllTargetsDrifting(
    Ship *ship,                // Ship.
    VolleyFireSpec *spec,      // Volley fire instance.
    SelectAnyCommand *targets, // All targets selected when the volley fire special ability as activated.
    ShootCallback shoot)       // Weapon behavior to use to shoot each target at least once.
{
    // Stop here if the shoot callback isn't provided, since the volley fire won't work, anyway.
    if (shoot == NULL)
    {
        return TRUE;
    }

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
    if (spec->currentTargetIndex >= noOfShipsToTarget)
    {
        volleyFireSetCurrentTargetIndex(spec, 0);
    }

    // Attempt to attack each target.
    for (gunIndex = 0; gunIndex < noOfGuns; gunIndex++)
    {
        target = targets->TargetPtr[spec->currentTargetIndex];
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
        shoot(ship, gun, target);

        sdword nextTargetIndex = volleyFireGetNextTargetIndex(spec->currentTargetIndex, noOfShipsToTarget);
        volleyFireSetCurrentTargetIndex(spec, nextTargetIndex);
    }

    if (hasFiredAtLeastOnce)
    {
        return FALSE;
    }
    // Stop here and reset everything if the weapons are out of ammunition.
    else
    {
        volleyFireSetBattleChatterBusy(spec, FALSE);
        volleyFireSetCurrentTargetIndex(spec, 0);

        return TRUE;
    }
}

/**
 * @brief Sets the ship to volley fire in place, flying towards its first target if necessary. Use for slow ships such as frigates and capital ships.
 * @param ship Ship.
 * @param spec Volley fire instance.
 * @param targets All targets selected when the volley fire special ability as activated.
 * @param shoot Weapon behavior to use to shoot each target at least once.
 * @return Indication of whether the ship's volley fire sequence is complete.
 */
bool volleyFireShootAllTargetsLumbering(
    Ship *ship,                // Ship.
    VolleyFireSpec *spec,      // Volley fire instance.
    SelectAnyCommand *targets, // All targets selected when the volley fire special ability as activated.
    ShootCallback shoot)       // Weapon behavior to use to shoot each target at least once.
{
    // Stop here if the shoot callback isn't provided, since the volley fire won't work, anyway.
    if (shoot == NULL)
    {
        return TRUE;
    }

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
    if (spec->currentTargetIndex >= noOfShipsToTarget)
    {
        volleyFireSetCurrentTargetIndex(spec, 0);
    }

    // Attempt to attack each target.
    for (gunIndex = 0; gunIndex < noOfGuns; gunIndex++)
    {
        target = targets->TargetPtr[spec->currentTargetIndex];
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
        shoot(ship, gun, target);

        sdword nextTargetIndex = volleyFireGetNextTargetIndex(spec->currentTargetIndex, noOfShipsToTarget);
        volleyFireSetCurrentTargetIndex(spec, nextTargetIndex);
    }

    if (hasFiredAtLeastOnce)
    {
        aitrackZeroVelocity(ship);
        return FALSE;
    }
    // Stop here and reset everything if the weapons are out of ammunition.
    else if (hasAttemptedToFireAtLeastOnce)
    {
        volleyFireSetBattleChatterBusy(spec, FALSE);
        volleyFireSetCurrentTargetIndex(spec, 0);

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
