/*************************************************************************************************
 * LOCATOR CJSC
 *
 * CONFIDENTIAL AND PROPRIETARY
 * FOR USE BY AUTHORIZED PERSONS ONLY
 *
 * This is an unpublished work fully protected by the copyright
 * laws and is a trade secret belonging to the copyright holder.
 *
 * Copyright (c) 2007-2021 LOCATOR CJSC. All Rights Reserved
 *************************************************************************************************
 * pwr.c
 ************************************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "gpio.h"
#include "settings.h"
#include "pwr.h"

__IO bool is_coming_from_power_save = false;

static int power_save_lpsleep(void);
static int power_save_stop(void);

static int power_save_lpsleep(void)
{
    LL_PWR_ClearFlag_WU();

    LL_PWR_DisablePVD();

#if ULTRA_LOW_POWER_BIT_ENABLED == 1
    LL_PWR_EnableUltraLowPower();

#if defined(DEBUG)
    /*
     * From RM0377:
     *
     * Debug support for low-power modes:
     *
     * When one of the DBG_STANDBY, DBG_STOP and DBG_SLEEP bit is set and the
     * internal reference voltage is stopped in low-power mode (ULP bit set in
     * PWR_CR register), then the fast wakeup must be enabled (FWU bit set in
     * PWR_CR).
     */
    LL_PWR_EnableFastWakeUp();
#else
    LL_PWR_DisableFastWakeUp();
#endif
#endif /* ULTRA_LOW_POWER_BIT_ENABLED == 1 */

#if defined(DEBUG)
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_MAIN);
    LL_DBGMCU_EnableDBGSleepMode();
#else
    LL_DBGMCU_DisableDBGSleepMode();
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_LOW_POWER);
#endif

    LL_LPM_EnableSleep();

    LL_FLASH_EnableSleepPowerDown();

    LL_SYSTICK_DisableIT();

    deinit(POWER_SAVE_LPSLEEP);

#if LED_ERR_IS_COMBINED_WITH_WKUP1
    gpio_errled_disable();
#endif

#if WKUP_ENABLED == 1 && WKUP_WKUP1_ENABLED == 1
    LL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_PIN1);
#endif

    is_coming_from_power_save = true;

    if (!LL_RTC_IsActiveFlag_WUT(RTC)) {
        __WFI();
    }

    is_coming_from_power_save = false;

    reinit(POWER_SAVE_LPSLEEP);

    LL_SYSTICK_EnableIT();

    LL_FLASH_DisableSleepPowerDown();

    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_MAIN);

#if ULTRA_LOW_POWER_BIT_ENABLED == 1
#if defined(DEBUG)
    LL_PWR_DisableFastWakeUp();
#endif
    LL_PWR_DisableUltraLowPower();
#endif

    LL_PWR_SetPVDLevel(LL_PWR_PVDLEVEL_6);
    LL_PWR_EnablePVD();

    return 0;
}

static int power_save_stop(void)
{
    LL_PWR_ClearFlag_WU();

    LL_PWR_DisablePVD();

#if ULTRA_LOW_POWER_BIT_ENABLED == 1
    LL_PWR_EnableUltraLowPower();

#if defined(DEBUG)
    /*
     * From RM0377:
     *
     * Debug support for low-power modes:
     *
     * When one of the DBG_STANDBY, DBG_STOP and DBG_SLEEP bit is set and the
     * internal reference voltage is stopped in low-power mode (ULP bit set in
     * PWR_CR register), then the fast wakeup must be enabled (FWU bit set in
     * PWR_CR).
     */
    LL_PWR_EnableFastWakeUp();
#else
    LL_PWR_DisableFastWakeUp();
#endif
#endif /* ULTRA_LOW_POWER_BIT_ENABLED == 1 */

#if defined(DEBUG)
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_MAIN);
    LL_DBGMCU_EnableDBGStopMode();
#else
    LL_DBGMCU_DisableDBGStopMode();
    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_LOW_POWER);
#endif

    LL_LPM_EnableDeepSleep();

    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);

    LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_MSI);

    LL_SYSTICK_DisableIT();

    deinit(POWER_SAVE_STOP);

#if LED_ERR_IS_COMBINED_WITH_WKUP1
    gpio_errled_disable();
#endif

#if WKUP_ENABLED == 1 && WKUP_WKUP1_ENABLED == 1
    LL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_PIN1);
#endif

    is_coming_from_power_save = true;

    if (!LL_RTC_IsActiveFlag_WUT(RTC)) {
        __WFI();
    }

    is_coming_from_power_save = false;

    reinit(POWER_SAVE_STOP);

    LL_SYSTICK_EnableIT();

    LL_LPM_EnableSleep();

    LL_DBGMCU_DisableDBGStopMode();

    LL_PWR_SetRegulModeLP(LL_PWR_REGU_LPMODES_MAIN);

#if ULTRA_LOW_POWER_BIT_ENABLED == 1
#if defined(DEBUG)
    LL_PWR_DisableFastWakeUp();
#endif
    LL_PWR_DisableUltraLowPower();
#endif

    LL_PWR_SetPVDLevel(LL_PWR_PVDLEVEL_6);
    LL_PWR_EnablePVD();

    return 0;
}

int power_save(enum power_save_t t)
{
    switch (t) {
    case POWER_SAVE_LPSLEEP:
        return power_save_lpsleep();
    case POWER_SAVE_STOP:
        return power_save_stop();
    default:
        return 0;
    }
}
