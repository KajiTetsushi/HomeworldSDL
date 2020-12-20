//
//  VolleyFire.h
//
//  Created on 2020-12-13 by Kaji Tetsushi
//  Copyleft HomeworldSDL.
//

#ifndef ___VOLLEYFIRE_H
#define ___VOLLEYFIRE_H

#include "ShipSelect.h"
#include "SpaceObj.h"
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

typedef void (*ShootCallback)(Ship *ship, Gun *gun, SpaceObjRotImpTarg *target);
typedef bool (*ShootTargetsCallback)(struct Ship *ship, VolleyFireSpec *spec, SelectAnyCommand *targets, ShootCallback shoot);

void volleyFireInit(VolleyFireSpec *spec);
bool volleyFireIsReady(VolleyFireSpec *spec, VolleyFireStat *stat);

bool volleyFireShootAllTargetsDrifting(Ship *ship, VolleyFireSpec *spec, SelectAnyCommand *targets, ShootCallback shoot);
bool volleyFireShootAllTargetsLumbering(Ship *ship, VolleyFireSpec *spec, SelectAnyCommand *targets, ShootCallback shoot);

bool volleyFireSpecialTarget(Ship *ship, void *custom, VolleyFireSpec *spec, VolleyFireStat *stat, ShootTargetsCallback shootTargets, ShootCallback shoot, sdword battleChatterEvent);

#endif
