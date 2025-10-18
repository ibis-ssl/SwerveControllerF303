#ifndef CONFIG_MODE_H
#define CONFIG_MODE_H

#include <stdint.h>

/* 起動時の設定ロードとログ出力 */
void config_mode_init(void);

/* 起動直後の設定ウィンドウ（Enter連打検出→board_id入力/保存/再起動） */
void config_mode_run_window(void);

/* 現在ロード済みの board_id を取得 */
uint32_t config_mode_get_board_id(void);

#endif /* CONFIG_MODE_H */

