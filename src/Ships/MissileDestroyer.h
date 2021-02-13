// =============================================================================
//  MissileDestroyer.h
// =============================================================================
//  Copyright Relic Entertainment, Inc. All rights reserved.
//  Created 6/30/1997 by gshaw
// =============================================================================

#ifndef ___MISSILEDESTROYER_H
#define ___MISSILEDESTROYER_H

#include "Ammunition.h"
#include "Attack.h"
#include "SpaceObj.h"
#include "VolleyFire.h"

typedef struct
{
    VolleyFireSpec volleyFire;
    AmmunitionSpec ammunition;
} MissileDestroyerSpec;

typedef struct
{
    AttackSideStepParameters sidestepParameters;
    real32 wpnRange[NUM_TACTICS_TYPES];
    real32 wpnMinRange[NUM_TACTICS_TYPES];
    VolleyFireStat volleyFire;
    AmmunitionStat ammunition;

    /**
     * The waiting duration (in seconds) before incrementing the ship's total ammunition by 1.
     * @deprecated Use ammunition.reloadCooldown
     */
    real32 missileRegenerateTime;
    /**
     * The delay (in s) before the volley fire special ability can be activated again.
     * @deprecated Use volleyFire.abilityCooldown
     */
    real32 missileVolleyTime;
    /**
     * The delay (in s) before the ammunition can begin reloading after a successful volley fire attack.
     * @deprecated Use volleyFire.reloadCooldown
     */
    real32 missileLagVolleyTime;
} MissileDestroyerStat;

extern CustShipHeader MissileDestroyerHeader;

#endif
