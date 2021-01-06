#include "tos_k.h"
#include "core_cm0.h"

__PORT__ void port_cpu_reset(void)
{
    NVIC_SystemReset();
}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
    (void)SysTick_Config(cycle_per_tick);
}

__PORT__ void port_systick_priority_set(uint32_t prio)
{
    NVIC_SetPriority(SysTick_IRQn, prio);
}

#if TOS_CFG_TICKLESS_EN > 0u

__PORT__ k_time_t port_systick_max_delay_millisecond(void)
{
    k_time_t max_millisecond;
    uint32_t max_cycle;

    max_cycle = SysTick_LOAD_RELOAD_Msk; // 24 bit
    max_millisecond = (k_time_t)((uint64_t)max_cycle * K_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_CLOCK); // CLOCK: cycle per second
    return max_millisecond;
}

__PORT__ void port_systick_resume(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

__PORT__ void port_systick_suspend(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

__PORT__ void port_systick_reload(uint32_t cycle_per_tick)
{
    port_systick_config(cycle_per_tick);
}

__PORT__ void port_systick_pending_reset(void)
{
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
}

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__PORT__ void port_sleep_mode_enter(void)
{
#if 1
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#else
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#endif
}

__PORT__ void port_stop_mode_enter(void)
{
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

__PORT__ void port_standby_mode_enter(void)
{
    HAL_PWR_EnterSTANDBYMode();
}

#endif

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

__PORT__ void port_fault_diagnosis(void)
{
    k_fault_log_writer("fault diagnosis is not supported in CORTEX M0\n");
}

__PORT__ void __NAKED__ HardFault_Handler(void)
{
    __ASM__ __VOLATILE__ (
        "MOV     r0, lr\n\t"
        "MOVS    r1, #0x04\n\t"
        "TST     r0, r1\n\t"
        "BEQ     _LD_MSP\n\t"
        "MRS     r1, PSP\n\t"
        "B       _EXIT\n"
        "_LD_MSP:\n\t"
        "MRS     r1, MSP\n"
        "_EXIT:\n\t"
        "LDR     r2, =fault_backtrace\n\t"
        "BX      r2\n\t"
    );
}

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

