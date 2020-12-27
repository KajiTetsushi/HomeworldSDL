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
     * @deprecated Use ammunition.reloadDelay
     */
    real32 missileRegenerateTime;
    /**
     * The delay (in seconds) between every weapon fire while the volley fire special ability is active.
     * @deprecated Use volleyFire.fireDelay
     */
    real32 missileVolleyTime;
    /**
     * The delay (in seconds) before the volley fire special ability can be used again after its last activation.
     * @deprecated Use volleyFire.abilityCooldown
     */
    real32 missileLagVolleyTime;
} MissileDestroyerStat;

extern CustShipHeader MissileDestroyerHeader;

#endif
