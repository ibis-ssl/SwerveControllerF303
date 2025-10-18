#ifndef SETTINGS_STORAGE_H
#define SETTINGS_STORAGE_H

#include <stddef.h>
#include <stdint.h>

#include "stm32f3xx_hal.h"

/* Linker-provided symbols for settings area */
extern uint8_t __settings_start__;
extern uint8_t __settings_end__;
extern uint8_t __settings_size__;

#define SETTINGS_AREA_START ((uint32_t)&__settings_start__)
#define SETTINGS_AREA_END ((uint32_t)&__settings_end__)
#define SETTINGS_AREA_SIZE ((uint32_t)&__settings_size__)

/* Erase entire reserved settings area */
HAL_StatusTypeDef SettingsStorage_EraseAll(void);

/* Read from settings area */
void SettingsStorage_Read(uint32_t offset, void * dst, size_t size);

/* Write arbitrary data into settings area (erased area required) */
HAL_StatusTypeDef SettingsStorage_Write(uint32_t offset, const void * src, size_t size);

#endif /* SETTINGS_STORAGE_H */
