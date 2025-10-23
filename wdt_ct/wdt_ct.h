/*
 * Copyright (C) 2025 Randy L.
 * Copyright (C) 2017 Chen Hung-Nien
 * Copyright (C) 2017 Weida Hi-Tech
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef	__WDT_CT_H__
#define	__WDT_CT_H__
#include "stdint.h"

/* define for device control */
#define		OPTION_UPDATE			0x1
#define		OPTION_VERIFY			0x2
#define		OPTION_NO_FORCE			0x4
#define		OPTION_INFO			0x8


/* define for info from the device */


int		load_lib_func_address(WDT_DEV*, EXEC_PARAM*);
int		image_file_burn_data_verify(WDT_DEV *pdev, EXEC_PARAM *pParam);
int 		show_wif_info(WDT_DEV *pdev, EXEC_PARAM *pparam);
int		show_info(WDT_DEV *pdev, EXEC_PARAM *pParam);
int		image_file_check(WDT_DEV *pdev, EXEC_PARAM *pParam);

void 		wh_printf(const char *fmt, ...);
void 		wh_sleep(int ms);
void 		wh_udelay(int us);
unsigned long 	get_current_ms();

int update_firmware_internal(char *path);
int verify_firmware_internal(char *path);
int get_vid_pid_internal(unsigned int *vid, unsigned int *pid);
int get_current_firmware_version_internal(unsigned int *version);
int get_device_name_internal(char *name, size_t max_len);
int get_hardware_id_internal(unsigned int *hw_id);
int get_device_info_internal(unsigned int *vid, unsigned int *pid, unsigned int *hw_id, unsigned int *version);
int get_boot_mode_internal(unsigned int *mode);



/* __WDT_CT_H__ */
#endif
