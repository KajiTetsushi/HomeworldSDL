//
//  VolleyFire.c
//  Handles behavior involving the volley fire speciabl ability.
//
//  Created on 2020-12-13 by Kaji Tetsushi
//  Copyleft HomeworldSDL.
//

#include "VolleyFire.h"

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
    volleyFireSetBattleChatterBusy(spec, FALSE);
    volleyFireSetCurrentTargetIndex(spec, 0);
    volleyFireSetLastAttemptedTimestamp(spec, -100000.0f);
    volleyFireSetLastActivatedTimestamp(spec, 0.0f);
}

/**
 * @brief Invokes the volley fire special ability with a user-defined weapons behavior.
 * @param ship Ship reference.
 * @param custom Targets selected to attack.
 * @param spec Volley fire instance.
 * @param stat Volley fire configuration.
 * @param battleChatter Battle chatter event to use when the ability has been activated.
 * @param attackTargets Ship behavior to use when attacking all selected targets.
 * @return Indication of whether the ship's volley fire sequence is complete.
 */
bool volleyFireSpecialTarget(
    Ship *ship,                          // Ship.
    SelectAnyCommand *targets,           // Targets selected to attack.
    VolleyFireSpec *spec,                // Volley fire instance.
    VolleyFireStat *stat,                // Volley fire configuration.
    sdword battleChatterEvent,           // Battle chatter event to use when the ability has been activated.
    AttackTargetsCallback attackTargets) // Ship behavior to use when attacking all selected targets.
{
    dbgAssertOrIgnore(attackTargets != NULL);

    volleyFireSetLastAttemptedTimestamp(spec, universe.totaltimeelapsed);

    // Stop here if the "volley reload time" has passed
    real32 durationSinceLastActivated = universe.totaltimeelapsed - spec->lastActivatedOn;
    if (durationSinceLastActivated <= stat->fireDelay)
    {
        return FALSE;
    }

    volleyFireSetLastActivatedTimestamp(spec, universe.totaltimeelapsed);

    if (targets->numTargets == 0)
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

    return attackTargets(ship, spec, stat, targets);
}
