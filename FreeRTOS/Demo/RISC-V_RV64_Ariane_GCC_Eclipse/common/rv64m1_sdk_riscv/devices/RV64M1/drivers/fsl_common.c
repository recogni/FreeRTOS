/*
* Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
* All rights reserved.
*
* 
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_common.h"
#include "fsl_debug_console.h"

#ifndef NDEBUG
#if(defined(__GNUC__))
void __assert_func(const char *file, int line, const char *func, const char *failedExpr)
{
    PRINTF("ASSERT ERROR \" %s \": file \"%s\" Line \"%d\" function name \"%s\" \n", failedExpr, file, line, func);
    for (;;)
    {
        __BKPT(0);
    }
}
#endif /* (defined(__CC_ARM)) ||  (defined (__ICCARM__)) */
#endif /* NDEBUG */
