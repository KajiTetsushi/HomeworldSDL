//
//  VolleyFire.h
//
//  Created on 2020-12-13 by Kaji Tetsushi
//  Copyleft HomeworldSDL.
//

#ifndef ___VOLLEYFIRE_H
#define ___VOLLEYFIRE_H

#include "ShipSelect.h"
#include "StatScript.h"
#include "Types.h"

typedef struct VolleyFireSpec
{
    real32 lastAttemptedOn;
    real32 lastActivatedOn;
    sdword currentTargetIndex;
    bool battleChatterBusy;
} VolleyFireSpec;

typedef struct VolleyFireStat
{
    /**
     * The delay (in seconds) between every weapon fire while the volley fire special ability is active.
     */
    real32 fireDelay;
    /**
     * The delay (in seconds) before the volley fire special ability can be used again after its last activation.
     */
    real32 abilityCooldown;
} VolleyFireStat;

extern scriptStructEntry VolleyFireStatScriptTable[];

typedef bool (*AttackTargetsCallback)(struct Ship *ship, VolleyFireSpec *spec, VolleyFireStat *stat, SelectAnyCommand *targets);

void volleyFireSetCurrentTargetIndex(VolleyFireSpec *spec, sdword index);
void volleyFireSetBattleChatterBusy(VolleyFireSpec *spec, bool busy);
void volleyFireSetLastAttemptedTimestamp(VolleyFireSpec *spec, real32 timestamp);
void volleyFireSetLastActivatedTimestamp(VolleyFireSpec *spec, real32 timestamp);
sdword volleyFireGetNextTargetIndex(sdword currentTargetIndex, sdword noOfTargets);
void volleyFireInit(VolleyFireSpec *spec);
bool volleyFireIsReady(VolleyFireSpec *spec, VolleyFireStat *stat);
bool volleyFireSpecialTarget(Ship *ship, SelectAnyCommand *targets, VolleyFireSpec *spec, VolleyFireStat *stat, sdword battleChatterEvent, AttackTargetsCallback attackTargets);

#endif
