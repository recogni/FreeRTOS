/*
** ###################################################################
**     Processors:          RV32M1_ri5cy
**                          RV32M1_ri5cy
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          MCUXpresso Compiler
**
**     Reference manual:    RV32M1 Series Reference Manual, Rev. 1 , 8/10/2018
**     Version:             rev. 1.0, 2018-10-02
**     Build:               b180926
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2018-10-02)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file RV32M1_ri5cy
 * @version 1.0
 * @date 2018-10-02
 * @brief Device specific configuration file for RV32M1_ri5cy
 *        (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "fsl_device_registers.h"
#include "fsl_common.h"

typedef void (*irq_handler_t)(void);

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
uint32_t foo = 0x33;

extern uint32_t __etext;
extern uint32_t __data_load;
extern uint32_t __data_start;
extern uint32_t __data_end;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

extern uint32_t __sdata_load;
extern uint32_t __sdata_start;
extern uint32_t __sdata_end;

extern uint32_t __sbss_start;
extern uint32_t __sbss_end;

//#ifdef LATER
static void copy_section(uint32_t * p_load, uint32_t * p_vma, uint32_t * p_vma_end)
{
    while(p_vma <= p_vma_end)
    {
        *p_vma = *p_load;
        ++p_load;
        ++p_vma;
    }
}

static void zero_section(uint32_t * start, uint32_t * end)
{
    uint32_t * p_zero = start;

    while(p_zero <= end)
    {
        *p_zero = 0;
        ++p_zero;
    }
}
//#endif

#define DEFINE_IRQ_HANDLER(irq_handler, driver_irq_handler) \
    void __attribute__((weak)) irq_handler(void) { driver_irq_handler();}

#define DEFINE_DEFAULT_IRQ_HANDLER(irq_handler) void irq_handler() __attribute__((weak, alias("DefaultIRQHandler")))

#ifdef LATER
DEFINE_DEFAULT_IRQ_HANDLER(RF0_0_IRQHandler);
DEFINE_DEFAULT_IRQ_HANDLER(RF0_1_IRQHandler);

__attribute__((section("user_vectors"))) const irq_handler_t isrTable[] =
{
    RF0_0_IRQHandler,
    RF0_1_IRQHandler,
};

extern uint32_t __VECTOR_TABLE[];
#endif //LATER


static uint32_t irqNesting = 0;

#ifdef LATER
static void DefaultIRQHandler(void)
{
    for (;;)
    {
    }
}
#endif

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

#ifdef OUTER_LATER
void SystemInit (void) {
#if (DISABLE_WDOG)
  WDOG0->CNT = 0xD928C520U;
  WDOG0->TOVAL = 0xFFFF;
  WDOG0->CS = (uint32_t) ((WDOG0->CS) & ~WDOG_CS_EN_MASK) | WDOG_CS_UPDATE_MASK;
#endif /* (DISABLE_WDOG) */

  SystemInitHook();

#ifdef LATER
  copy_section(&__etext, &__data_start__, &__data_end__);
  zero_section(&__bss_start__, &__bss_end__);
#endif  //LATER

  /* Setup the vector table address. */
  irqNesting = 0;

#ifdef LATER
  __ASM volatile("csrw 0x305, %0" :: "r"((uint64_t)__VECTOR_TABLE)); /* MTVEC */
  __ASM volatile("csrw 0x005, %0" :: "r"((uint64_t)__VECTOR_TABLE)); /* UTVEC */

  /* Clear all pending flags. */
  EVENT_UNIT->INTPTPENDCLEAR = 0xFFFFFFFF;
  EVENT_UNIT->EVTPENDCLEAR = 0xFFFFFFFF;
  /* Set all interrupt as secure interrupt. */
  EVENT_UNIT->INTPTSECURE = 0xFFFFFFFF;
#endif
}
#endif //OUTER_LATER

void _init(void)
{
    extern int main(int, char**);
    const char *argv0 = "hello";
    char *argv[] = {(char *)argv0, NULL, NULL};

    foo = 0x55;
    copy_section(&__sdata_load, &__sdata_start, &__sdata_end);
    foo = 0x66;
    copy_section(&__data_load, &__data_start, &__data_end);
    foo = 0x77;
    zero_section(&__sbss_start, &__sbss_end);
    foo = 0x88;
    zero_section(&__bss_start, &__bss_end);
    foo = 0x99;

    main(1, argv);
}


/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {

  uint32_t SCGOUTClock;                                 /* Variable to store output clock frequency of the SCG module */
  uint16_t Divider;
  Divider = ((SCG->CSR & SCG_CSR_DIVCORE_MASK) >> SCG_CSR_DIVCORE_SHIFT) + 1;

  switch ((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) {
    case 0x1:
      /* System OSC */
      SCGOUTClock = CPU_XTAL_CLK_HZ;
      break;
    case 0x2:
      /* Slow IRC */
      SCGOUTClock = (((SCG->SIRCCFG & SCG_SIRCCFG_RANGE_MASK) >> SCG_SIRCCFG_RANGE_SHIFT) ? 8000000 : 2000000);
      break;
    case 0x3:
      /* Fast IRC */
      SCGOUTClock = 48000000 + ((SCG->FIRCCFG & SCG_FIRCCFG_RANGE_MASK) >> SCG_FIRCCFG_RANGE_SHIFT) * 4000000;
      break;
    case 0x5:
      /* Low Power FLL */
      SCGOUTClock = 48000000 + ((SCG->LPFLLCFG & SCG_LPFLLCFG_FSEL_MASK) >> SCG_LPFLLCFG_FSEL_SHIFT) * 24000000;
      break;
    default:
      return;
  }
  SystemCoreClock = (SCGOUTClock / Divider);
}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInitHook (void) {
  /* Void implementation of the weak function. */
}

#if defined(__IAR_SYSTEMS_ICC__)
#pragma weak SystemIrqHandler
void SystemIrqHandler(uint32_t mcause) {
#elif defined(__GNUC__)
__attribute__((weak)) void SystemIrqHandler(uint32_t mcause) {
#else
  #error Not supported compiler type
#endif
    uint32_t intNum;

    if (mcause & 0x80000000) /* For external interrupt. */
    {
        intNum = mcause & 0x1FUL;

        irqNesting++;

        /* Clear pending flag in EVENT unit .*/
        EVENT_UNIT->INTPTPENDCLEAR = (1U << intNum);

        /* Read back to make sure write finished. */
        (void)(EVENT_UNIT->INTPTPENDCLEAR);

        __enable_irq();      /* Support nesting interrupt */

#ifdef LATER
        /* Now call the real irq handler for intNum */
        isrTable[intNum]();
#endif //LATER

        __disable_irq();

        irqNesting--;
    }
}

/* Use LIPT0 channel 0 for systick. */
#define SYSTICK_LPIT LPIT0
#define SYSTICK_LPIT_CH 0
#define SYSTICK_LPIT_IRQn LPIT0_IRQn

/* Leverage LPIT0 to provide Systick */
void SystemSetupSystick(uint32_t tickRateHz, uint32_t intPriority)
{
    /* Init pit module */
}

uint32_t SystemGetIRQNestingLevel(void)
{
    return irqNesting;
}

void SystemClearSystickFlag(void)
{
}

void EVENT_SetIRQPriority(IRQn_Type IRQn, uint8_t intPriority)
{
}

uint8_t EVENT_GetIRQPriority(IRQn_Type IRQn)
{
    uint8_t regIdx;
    uint8_t regOffset;
    int32_t intPriority;

    if ((IRQn < 32))
    {
        /*
         * 4 priority control registers, each register controls 8 interrupts.
         * Bit 0-2: interrupt 0
         * Bit 4-7: interrupt 1
         * ...
         * Bit 28-30: interrupt 7
         */
        regIdx = IRQn >> 3U;
        regOffset = (IRQn & 0x07U) << 2U;

        intPriority = (EVENT_UNIT->INTPTPRI[regIdx] >> regOffset) & 0xF;
        return (uint8_t)intPriority;
    }
    return 0;
}

bool SystemInISR(void)
{
    return ((EVENT_UNIT->INTPTENACTIVE) != 0);;
}

void EVENT_SystemReset(void)
{
    EVENT_UNIT->SLPCTRL |= EVENT_SLPCTRL_SYSRSTREQST_MASK;
}
