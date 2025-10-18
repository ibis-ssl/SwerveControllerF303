#include "app_settings.h"
#include "settings_storage.h"
#include <string.h>

static void defaults(app_settings_t * s)
{
  memset(s, 0xFF, sizeof(*s));
  s->magic   = APP_SETTINGS_MAGIC;
  s->version = APP_SETTINGS_VER;
  s->board_id = 0u; /* 既定: 0 */
}

void app_settings_load(app_settings_t * out)
{
  if (out == NULL) return;
  app_settings_t tmp;
  SettingsStorage_Read(0, &tmp, sizeof(tmp));
  if (tmp.magic != APP_SETTINGS_MAGIC || tmp.version != APP_SETTINGS_VER) {
    defaults(out);
    return;
  }
  *out = tmp;
}

HAL_StatusTypeDef app_settings_save(const app_settings_t * in)
{
  if (in == NULL) return HAL_ERROR;
  HAL_StatusTypeDef st;
  st = SettingsStorage_EraseAll();
  if (st != HAL_OK) {
    return st;
  }
  return SettingsStorage_Write(0, in, sizeof(*in));
}

