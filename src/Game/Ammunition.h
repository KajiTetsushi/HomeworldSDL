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
     * The elapsed time (in s) since the last time the ammunition has been reloaded.
     */
    real32 lastReloadedOn;
} AmmunitionSpec;

typedef struct AmmunitionStat
{
    /**
     * The delay (in s) before the ship's total stored ammunition is incremented by 1.
     */
    real32 reloadCooldown;
} AmmunitionStat;

extern scriptStructEntry AmmunitionStatScriptTable[];

void ammunitionInit(AmmunitionSpec *spec);
bool ammunitionCanReloadNow(AmmunitionSpec *spec, AmmunitionStat *stat);
void ammunitionReload(AmmunitionSpec *spec, AmmunitionStat *stat, GunInfo *gunInfo, GunStaticInfo *gunStaticInfo);

#endif
