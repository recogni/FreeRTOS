/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "RV64M1-ARIANE"

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART_BAUDRATE 115200U
#define BOARD_DEBUG_UART_BASEADDR (uint64_t) LPUART0
#define BOARD_DEBUG_UART_INSTANCE    0U
#define BOARD_DEBUG_UART_CLK_FREQ  150000     /* LATER FIXME BRETT */
#define BOARD_UART_IRQ_HANDLER LPUART0_IRQHandler
#ifdef LATER
#define BOARD_UART_IRQ LPUART0_IRQn
#endif //LATER

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

void BOARD_InitDebugConsole(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */

