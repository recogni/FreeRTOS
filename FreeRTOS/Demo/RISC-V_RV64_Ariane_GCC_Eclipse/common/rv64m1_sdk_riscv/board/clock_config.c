/*
 * Copyright 2017 NXP
 * All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


// LATER #include "fsl_msmc.h"
#include "clock_config.h"

extern uint32_t SystemCoreClock;
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

void BOARD_BootClockRUN(void)
{
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
}

void BOARD_BootClockHSRUN(void)
{
    SystemCoreClock = BOARD_BOOTCLOCKHSRUN_CORE_CLOCK;
}

void BOARD_BootClockVLPR(void)
{
    SystemCoreClock = BOARD_BOOTCLOCKVLPR_CORE_CLOCK;
}

