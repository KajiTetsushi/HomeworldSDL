//
//  Mission07.h
//
//  Finite state machine for "Mission07" mission
//
//  Copyright (C) 1998 Relic Entertainment Inc.
//  All rights reserved
//
//  This code was autogenerated from Mission07.kp by KAS2C Version 2.05sdl
//


#ifndef __Mission07_H
#define __Mission07_H


//
//  types and exposed game functions
//
#include <string.h>
#include "Types.h"
#include "Vector.h"
#include "AITeam.h"
#include "AIMoves.h"
#include "CommandWrap.h"
#include "Timer.h"
#include "Volume.h"
#include "Objectives.h"
#include "Attributes.h"
#include "TradeMgr.h"


//
//  level function pointer list
//
extern const void* Mission07_FunctionPointers[];
extern const unsigned int Mission07_FunctionPointerCount;


//
//  FSM prototypes
//
void Init_Mission07_HonourGuardSwarm(void);
void Watch_Mission07_HonourGuardSwarm(void);
void Init_Mission07_HonourGuardSwarm_Waiting(void);
void Watch_Mission07_HonourGuardSwarm_Waiting(void);
void Init_Mission07_HonourGuardSwarm_WaitingLeader(void);
void Watch_Mission07_HonourGuardSwarm_WaitingLeader(void);
void Init_Mission07_HonourGuardSwarm_Attack(void);
void Watch_Mission07_HonourGuardSwarm_Attack(void);
void Init_Mission07_HonourGuardSwarm_Dock(void);
void Watch_Mission07_HonourGuardSwarm_Dock(void);
void Init_Mission07_HonourGuardSwarm_NullState(void);
void Watch_Mission07_HonourGuardSwarm_NullState(void);
void Init_Mission07_GenericSwarmAttack(void);
void Watch_Mission07_GenericSwarmAttack(void);
void Init_Mission07_GenericSwarmAttack_NullState(void);
void Watch_Mission07_GenericSwarmAttack_NullState(void);
void Init_Mission07_GenericMothershipSwarm(void);
void Watch_Mission07_GenericMothershipSwarm(void);
void Init_Mission07_GenericMothershipSwarm_NullState(void);
void Watch_Mission07_GenericMothershipSwarm_NullState(void);
void Init_Mission07_GenericResourcerSwarm(void);
void Watch_Mission07_GenericResourcerSwarm(void);
void Init_Mission07_GenericResourcerSwarm_MoveToTarget(void);
void Watch_Mission07_GenericResourcerSwarm_MoveToTarget(void);
void Init_Mission07_GenericResourcerSwarm_NullState(void);
void Watch_Mission07_GenericResourcerSwarm_NullState(void);
void Init_Mission07_ResourceDefenderSwarm(void);
void Watch_Mission07_ResourceDefenderSwarm(void);
void Init_Mission07_ResourceDefenderSwarm_MoveToTarget(void);
void Watch_Mission07_ResourceDefenderSwarm_MoveToTarget(void);
void Init_Mission07_ResourceDefenderSwarm_NullState(void);
void Watch_Mission07_ResourceDefenderSwarm_NullState(void);
void Init_Mission07_DefendMothership(void);
void Watch_Mission07_DefendMothership(void);
void Init_Mission07_DefendMothership_NullState(void);
void Watch_Mission07_DefendMothership_NullState(void);
void Init_Mission07_IntroAttack(void);
void Watch_Mission07_IntroAttack(void);
void Init_Mission07_IntroAttack_GetStarted(void);
void Watch_Mission07_IntroAttack_GetStarted(void);
void Init_Mission07_IntroAttack_WaitForAttack(void);
void Watch_Mission07_IntroAttack_WaitForAttack(void);
void Init_Mission07_IntroAttack_NullState(void);
void Watch_Mission07_IntroAttack_NullState(void);
void Init_Mission07_PreAttackPod(void);
void Watch_Mission07_PreAttackPod(void);
void Init_Mission07_PreAttackPod_Dock(void);
void Watch_Mission07_PreAttackPod_Dock(void);
void Init_Mission07_PreAttackPod_WaitToLaunch(void);
void Watch_Mission07_PreAttackPod_WaitToLaunch(void);
void Init_Mission07_PreAttackPod_Launch(void);
void Watch_Mission07_PreAttackPod_Launch(void);
void Init_Mission07_PreAttackPod_GetOrders(void);
void Watch_Mission07_PreAttackPod_GetOrders(void);
void Init_Mission07_PreAttackPod_NullState(void);
void Watch_Mission07_PreAttackPod_NullState(void);
void Init_Mission07_PreAttackSwarm(void);
void Watch_Mission07_PreAttackSwarm(void);
void Init_Mission07_PreAttackSwarm_Dock(void);
void Watch_Mission07_PreAttackSwarm_Dock(void);
void Init_Mission07_PreAttackSwarm_WaitToLaunch(void);
void Watch_Mission07_PreAttackSwarm_WaitToLaunch(void);
void Init_Mission07_PreAttackSwarm_Launch(void);
void Watch_Mission07_PreAttackSwarm_Launch(void);
void Init_Mission07_PreAttackSwarm_DockAgain(void);
void Watch_Mission07_PreAttackSwarm_DockAgain(void);
void Init_Mission07_PreAttackSwarm_GetOrders(void);
void Watch_Mission07_PreAttackSwarm_GetOrders(void);
void Init_Mission07_PreAttackSwarm_NullState(void);
void Watch_Mission07_PreAttackSwarm_NullState(void);
void Init_Mission07_Events(void);
void Watch_Mission07_Events(void);
void Init_Mission07_Events_LocationCard(void);
void Watch_Mission07_Events_LocationCard(void);
void Init_Mission07_Events_Wait1(void);
void Watch_Mission07_Events_Wait1(void);
void Init_Mission07_Events_NebulaRich(void);
void Watch_Mission07_Events_NebulaRich(void);
void Init_Mission07_Events_UnknownContactsWait(void);
void Watch_Mission07_Events_UnknownContactsWait(void);
void Init_Mission07_Events_LBXIn1(void);
void Watch_Mission07_Events_LBXIn1(void);
void Init_Mission07_Events_UnknownContacts(void);
void Watch_Mission07_Events_UnknownContacts(void);
void Init_Mission07_Events_PreparingAmbassador(void);
void Watch_Mission07_Events_PreparingAmbassador(void);
void Init_Mission07_Events_P2IntroNIS(void);
void Watch_Mission07_Events_P2IntroNIS(void);
void Init_Mission07_Events_DefendMothership(void);
void Watch_Mission07_Events_DefendMothership(void);
void Init_Mission07_Events_HyperModuleCharging1(void);
void Watch_Mission07_Events_HyperModuleCharging1(void);
void Init_Mission07_Events_HyperModuleCharging2(void);
void Watch_Mission07_Events_HyperModuleCharging2(void);
void Init_Mission07_Events_LetsGetOutOfHere(void);
void Watch_Mission07_Events_LetsGetOutOfHere(void);
void Init_Mission07_Events_Engage(void);
void Watch_Mission07_Events_Engage(void);
void Init_Mission07_Events_HyperdriveFail(void);
void Watch_Mission07_Events_HyperdriveFail(void);
void Init_Mission07_Events_ContinueProtecting(void);
void Watch_Mission07_Events_ContinueProtecting(void);
void Init_Mission07_Events_AddedResearch(void);
void Watch_Mission07_Events_AddedResearch(void);
void Init_Mission07_Events_ProtectResourcers(void);
void Watch_Mission07_Events_ProtectResourcers(void);
void Init_Mission07_Events_P2MothershipEscapes(void);
void Watch_Mission07_Events_P2MothershipEscapes(void);
void Init_Mission07_Events_HyperspaceBack(void);
void Watch_Mission07_Events_HyperspaceBack(void);
void Init_Mission07_Events_JumpAway(void);
void Watch_Mission07_Events_JumpAway(void);
void Init_Mission07_Events_NullState(void);
void Watch_Mission07_Events_NullState(void);
void Init_Mission07_Events_FinalState(void);
void Watch_Mission07_Events_FinalState(void);
void Init_Mission07_P2Mothership(void);
void Watch_Mission07_P2Mothership(void);
void Init_Mission07_P2Mothership_Wait(void);
void Watch_Mission07_P2Mothership_Wait(void);
void Init_Mission07_P2Mothership_Reappear(void);
void Watch_Mission07_P2Mothership_Reappear(void);
void Init_Mission07_P2Mothership_InitialAttack(void);
void Watch_Mission07_P2Mothership_InitialAttack(void);
void Init_Mission07_P2Mothership_ReissueMove(void);
void Watch_Mission07_P2Mothership_ReissueMove(void);
void Init_Mission07_P2Mothership_HyperDodge(void);
void Watch_Mission07_P2Mothership_HyperDodge(void);
void Init_Mission07_P2Mothership_HypeDodgeIn(void);
void Watch_Mission07_P2Mothership_HypeDodgeIn(void);
void Init_Mission07_P2Mothership_MidBattle(void);
void Watch_Mission07_P2Mothership_MidBattle(void);
void Init_Mission07_P2Mothership_SecondAttack(void);
void Watch_Mission07_P2Mothership_SecondAttack(void);
void Init_Mission07_P2Mothership_DefendSelf(void);
void Watch_Mission07_P2Mothership_DefendSelf(void);
void Init_Mission07_P2Mothership_EndBattleDelay(void);
void Watch_Mission07_P2Mothership_EndBattleDelay(void);
void Init_Mission07_P2Mothership_EndBattle(void);
void Watch_Mission07_P2Mothership_EndBattle(void);
void Init_Mission07_P2Mothership_GetOuttaHere(void);
void Watch_Mission07_P2Mothership_GetOuttaHere(void);
void Init_Mission07_P2Mothership_NullState(void);
void Watch_Mission07_P2Mothership_NullState(void);
void Init_Mission07(void);
void Watch_Mission07(void);


#endif
