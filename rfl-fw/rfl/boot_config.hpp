/*
 * boot_config.hpp
 *
 *  Created on: Jun 14, 2026
 *      Author: hannes
 */

#ifndef BOOT_CONFIG_HPP_
#define BOOT_CONFIG_HPP_

#include "stm32g0xx_hal_flash.h"
#include "stm32g0xx_ll_rcc.h"

uint32_t _reboot_cookie __attribute__((section(".noinit")));

bool _is_sw_boot_configured(FLASH_OBProgramInitTypeDef const& ob)
{
    return (ob.USERConfig & OB_nBOOT0_SET) && !(ob.USERConfig & OB_BOOT0_FROM_PIN);
}

void _write_config()
{
    FLASH_OBProgramInitTypeDef OptionsBytesStruct;
    while (HAL_FLASH_Unlock() != HAL_OK)
        ;
    while (HAL_FLASH_OB_Unlock() != HAL_OK)
        ;
    OptionsBytesStruct.OptionType = OPTIONBYTE_USER | OPTIONBYTE_PCROP;
    OptionsBytesStruct.USERType = OB_USER_nBOOT0 | OB_USER_nBOOT_SEL;
    OptionsBytesStruct.USERConfig = OB_nBOOT0_SET | OB_USER_nBOOT_SEL;
    OptionsBytesStruct.PCROPConfig = OB_PCROP_RDP_ERASE;
    while (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
        ;
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
        ;
    FLASH->CR |= FLASH_CR_OBL_LAUNCH;
}

void _set_boot_to_bootloader()
{
    FLASH_OBProgramInitTypeDef OptionsBytesStruct;
    while (HAL_FLASH_Unlock() != HAL_OK)
        ;
    while (HAL_FLASH_OB_Unlock() != HAL_OK)
        ;
    OptionsBytesStruct.OptionType = OPTIONBYTE_USER; // Configure USER and RDP
    OptionsBytesStruct.USERType = OB_USER_nBOOT0 | OB_USER_nBOOT_SEL;
    OptionsBytesStruct.USERConfig = OB_nBOOT0_RESET | OB_USER_nBOOT_SEL; // Set to boot from pin (UART)
    while (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
        ;
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
        ;
    FLASH->CR |= FLASH_CR_OBL_LAUNCH;
}

void early_configure_option_bytes()
{
    FLASH_OBProgramInitTypeDef ob;
    HAL_FLASHEx_OBGetConfig(&ob);

    if (_reboot_cookie == 0xCD23ABD7) {
        _reboot_cookie = 0;
        _set_boot_to_bootloader();
    }

    if (!_is_sw_boot_configured(ob)) {
        // wait a bit for voltage stabilization
        for (uint32_t i = 0; i < 0x7FFFFF; i++) {
            asm("NOP");
        } // couple of seconds
        _write_config();
    }
}

void enter_bootloader(int key)
{
    if (key != 1234) {
        return;
    }
    _reboot_cookie = 0xCD23ABD7;
    NVIC_SystemReset();
}

#endif /* BOOT_CONFIG_HPP_ */
