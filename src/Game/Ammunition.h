//
//  Ammunition.h
//  Manages ammunition logic such as reloading behavior.
//
//  Created on 2020-12-21 by KajiTetsushi
//  Copyleft HomeworldSDL.
//

#ifndef ___AMMUNITION_H
#define ___AMMUNITION_H

#include "SpaceObj.h"
#include "StatScript.h"
#include "Types.h"

typedef struct AmmunitionSpec
{
    /**
     * The elapsed time (in seconds) since the last time the ammunition has been reloaded.
     */
    real32 lastReloadedTimestamp;
} AmmunitionSpec;

typedef struct AmmunitionStat
{
    /**
     * The waiting duration (in seconds) before incrementing the ship's total ammunition by 1.
     */
    real32 reloadDelay;
} AmmunitionStat;

extern scriptStructEntry AmmunitionStatScriptTable[];

void ammunitionInit(AmmunitionSpec *spec);
void ammunitionReloadLeastNoOfRoundsGun(AmmunitionSpec *spec, AmmunitionStat *stat, GunInfo *gunInfo, GunStaticInfo *gunStaticInfo);

#endif
