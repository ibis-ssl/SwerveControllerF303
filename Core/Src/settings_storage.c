#include "settings_storage.h"
#include <string.h>

HAL_StatusTypeDef SettingsStorage_EraseAll(void)
{
    HAL_StatusTypeDef st;
    FLASH_EraseInitTypeDef ei = {0};
    uint32_t page_error = 0;

    uint32_t nb_pages = SETTINGS_AREA_SIZE / FLASH_PAGE_SIZE;

    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

    ei.TypeErase   = FLASH_TYPEERASE_PAGES;
    ei.PageAddress = SETTINGS_AREA_START;
    ei.NbPages     = nb_pages;

    st = HAL_FLASHEx_Erase(&ei, &page_error);

    HAL_FLASH_Lock();
    return st;
}

void SettingsStorage_Read(uint32_t offset, void *dst, size_t size)
{
    if (offset + size > SETTINGS_AREA_SIZE) {
        size = (offset < SETTINGS_AREA_SIZE) ? (SETTINGS_AREA_SIZE - offset) : 0;
    }
    memcpy(dst, (const void *)(SETTINGS_AREA_START + offset), size);
}

HAL_StatusTypeDef SettingsStorage_Write(uint32_t offset, const void *src, size_t size)
{
    if (offset + size > SETTINGS_AREA_SIZE) {
        return HAL_ERROR;
    }

    const uint8_t *p = (const uint8_t *)src;
    uint32_t addr = SETTINGS_AREA_START + offset;
    HAL_StatusTypeDef st = HAL_OK;

    HAL_FLASH_Unlock();

    /* Program in half-words to avoid alignment pitfalls */
    while (size >= 2U) {
        uint16_t hw = (uint16_t)p[0] | ((uint16_t)p[1] << 8);
        st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, hw);
        if (st != HAL_OK) {
            break;
        }
        addr += 2U;
        p    += 2U;
        size -= 2U;
    }

    if (st == HAL_OK && size == 1U) {
        /* Pad high byte with 0xFF */
        uint16_t hw = (uint16_t)p[0] | (0xFFU << 8);
        st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, hw);
    }

    HAL_FLASH_Lock();
    return st;
}

