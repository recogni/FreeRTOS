/*
 * Copyright 2017 NXP
 * All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
#ifndef _CLOCK_CONFIG_H_
#define _CLOCK_CONFIG_H_

#include "fsl_common.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

void BOARD_InitBootClocks(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#define BOARD_BOOTCLOCKRUN_CORE_CLOCK              48000000U  /*!< Core clock frequency: 48000000Hz */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

void BOARD_BootClockRUN(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#define BOARD_BOOTCLOCKHSRUN_CORE_CLOCK            72000000U  /*!< Core clock frequency: 72000000Hz */


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

void BOARD_BootClockHSRUN(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#define BOARD_BOOTCLOCKVLPR_CORE_CLOCK              4000000U  /*!< Core clock frequency: 4000000Hz */


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

void BOARD_BootClockVLPR(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* _CLOCK_CONFIG_H_ */

