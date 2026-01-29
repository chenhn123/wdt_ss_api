/*
 * Copyright (C) 2025 Randy Lai
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#include "wdt_dev_api.h"
#include "dev_def.h"
#include "w8755_funcs.h"
#include "w8760_funcs.h"
#include "w8790_funcs.h"
#include "wdt_ct.h"
#include "wif2.h"
#include "hidraw_api.h"


int wh_get_device_access_func(int interfaceIndex, FUNC_PTR_STRUCT_DEV_ACCESS*  pFuncs)
{
	if (!pFuncs)
		return 0;

	if (interfaceIndex == INTERFACE_I2C) {
		pFuncs->p_wh_scan_device = (LPFUNC_wh_scan_device) wh_i2c_scan_device;
		pFuncs->p_wh_get_device = (LPFUNC_wh_get_device) wh_i2c_get_device;
		pFuncs->p_wh_open_device = (LPFUNC_wh_open_device) wh_i2c_open_device;
		pFuncs->p_wh_close_device = (LPFUNC_wh_close_device) wh_i2c_close_device;
		pFuncs->p_wh_prepare_data = (LPFUNC_wh_prepare_data) wh_i2c_prepare_data;			
		return 1;
	}
	else if(interfaceIndex == INTERFACE_HIDRAW) {
		pFuncs->p_wh_scan_device = (LPFUNC_wh_scan_device) wh_hidraw_scan_device;
		pFuncs->p_wh_get_device = (LPFUNC_wh_get_device) wh_hidraw_get_device;
		pFuncs->p_wh_open_device = (LPFUNC_wh_open_device) wh_hidraw_open_device;
		pFuncs->p_wh_close_device = (LPFUNC_wh_close_device) wh_hidraw_close_device;
		pFuncs->p_wh_prepare_data = (LPFUNC_wh_prepare_data) wh_hidraw_prepare_data;
		return 1;
	}

	return 0;
}

int wh_get_device_private_access_func(WDT_DEV* pdev,  FUNC_PTR_STRUCT_DEV_OPERATION*  pFuncs)
{
	if (!pFuncs || !pdev)
		return 0;

	if (pdev->board_info.dev_type & FW_WDT8755) {
		pFuncs->p_wh_program_chunk = (LPFUNC_wh_program_chunk) wh_w8755_dev_program_chunk;
		pFuncs->p_wh_verify_chunk = (LPFUNC_wh_verify_chunk) wh_w8755_dev_verify_chunk;
		pFuncs->p_wh_flash_write_data = (LPFUNC_wh_flash_write_data) wh_w8755_dev_flash_write_data;
		pFuncs->p_wh_flash_get_checksum = (LPFUNC_wh_flash_get_checksum) wh_w8755_dev_flash_get_checksum;
 		pFuncs->p_wh_send_commands = (LPFUNC_wh_send_commands) wh_w8755_dev_send_commands;

		return wh_w8755_dev_set_basic_op(pdev);
	}

	if (pdev->board_info.dev_type & FW_WDT8760_2) {
		pFuncs->p_wh_program_chunk = (LPFUNC_wh_program_chunk) wh_w8760_dev_program_chunk;
		pFuncs->p_wh_verify_chunk = (LPFUNC_wh_verify_chunk) wh_w8760_dev_verify_chunk;
		pFuncs->p_wh_flash_write_data = (LPFUNC_wh_flash_write_data) wh_w8760_dev_flash_write_data;
		pFuncs->p_wh_flash_get_checksum = (LPFUNC_wh_flash_get_checksum) wh_w8760_dev_flash_get_checksum;
 		pFuncs->p_wh_send_commands = (LPFUNC_wh_send_commands) wh_w8760_dev_send_commands;

		return wh_w8760_dev_set_basic_op(pdev);
	}
	if (pdev->board_info.dev_type & FW_WDT8790) {
		pFuncs->p_wh_flash_write_data = (LPFUNC_wh_flash_write_data)wh_w8790_dev_flash_write_data;
		pFuncs->p_wh_flash_get_checksum = (LPFUNC_wh_flash_get_checksum)wh_w8790_dev_flash_get_checksum;
		pFuncs->p_wh_send_commands = (LPFUNC_wh_send_commands)wh_w8790_dev_send_commands;
		pFuncs->p_wh_flash_erase = (LPFUNC_wh_flash_erase)wh_w8790_dev_flash_erase;


		return wh_w8790_dev_set_basic_op(pdev);
	}


	return 0;

}

int wh_get_device_basic_access_func(WDT_DEV* pdev,  FUNC_PTR_STRUCT_DEV_BASIC*  pFuncs)
{
	if (!pFuncs || !pdev)
		return 0;


	if (pdev->board_info.dev_type & FW_WDT8755) {
		pFuncs->p_wh_get_feature = (LPFUNC_wh_get_feature) wh_w8755_dev_get_feature;
		pFuncs->p_wh_set_feature = (LPFUNC_wh_set_feature) wh_w8755_dev_set_feature;

		return 1;
	}
	
	if (pdev->board_info.dev_type & FW_WDT8760_2) {
		pFuncs->p_wh_get_feature = (LPFUNC_wh_get_feature) wh_w8760_dev_get_feature;
		pFuncs->p_wh_set_feature = (LPFUNC_wh_set_feature) wh_w8760_dev_set_feature;
		return 1;
	}

	if (pdev->board_info.dev_type & FW_WDT8790) {
		pFuncs->p_wh_get_feature = (LPFUNC_wh_get_feature) wh_w8790_dev_get_feature;
		pFuncs->p_wh_set_feature = (LPFUNC_wh_set_feature) wh_w8790_dev_set_feature;
		return 1;
        }



	return 0;
}



int check_firmware_id(WDT_DEV *pdev, UINT32 fwid)
{
	if ((fwid & 0xF0000000) == 0x30000000) {
		if (pdev->pparam->argus & OPTION_INFO)		
			printf("It is CI3.0 or SR2.0 !\n");	
		return FW_WDT8755;
	}

	if ((fwid & 0xFFFF0000) == 0xFFFF0000) {
		if (pdev->pparam->argus & OPTION_INFO)		
			wh_printf("It is CI3.0 or SR2.0 recovery fw !\n");	
		return FW_WDT8755;
	}	

	if ((fwid & 0xF0000000) == 0x40000000) {
		if (pdev->pparam->argus & OPTION_INFO)	
			printf("It is CI4.0 or TM4.0 !\n");	
		return FW_WDT8760;
	}

	if ((fwid & 0xFF000000) == 0x51000000) {
                if(pdev->pparam->argus & OPTION_INFO)
                        printf("It is SR3.0 !\n");
                return FW_NOT_SUPPORT;

        }

	//TODO: We will add new devices here and should before FW_WDT8970 for compatibility.

	if ((fwid & 0xFF000000) == 0x50000000) {
		if(pdev->pparam->argus & OPTION_INFO)
			printf("It is CI5.0 !\n");
		return FW_WDT8790;

	}
	if ((fwid & 0xFFFFF000) == 0x13308000) {
		if(pdev->pparam->argus)
			printf("It is TM1.8, TM1.75 or TM1.7 (Not suppport) !\n");
		return FW_NOT_SUPPORT;
	}
	return FW_NOT_SUPPORT;
	

}

UINT16 get_unaligned_le16(const void *p)
{
	const BYTE *_p = (BYTE *) p;
	return _p[0] | _p[1] << 8;
}

UINT32 get_unaligned_le32(const void *p)
{
	const BYTE *_p = (BYTE *) p;
	return _p[0] | _p[1] << 8 | _p[2] << 16 | _p[3] << 24;
}

void put_unaligned_le16(UINT16 val, BYTE *p)
{
	*p++ = (BYTE) val;
	*p++ = (BYTE) (val >> 8);
}

void put_unaligned_le32(UINT32 val, BYTE *p)
{
	put_unaligned_le16(val >> 16, p + 2);
	put_unaligned_le16(val, p);
}


int check_is_all_ff(BYTE* data, int length)
{
	if (data == 0)
		return 0;

	int idx;

	for (idx = 0; idx < length; idx++)
		if (data[idx] != 0xFF)
			return 0;
	return 1;
}

int count_ff_bytes(BYTE* data, int start, int size)
{
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		if (data[start + i] == 0xFF)
			count++;
	}
	return count;
}


int load_lib_func_address(WDT_DEV *pdev, EXEC_PARAM *pparam)
{
	if (!pdev)
		return 0;

	pdev->func_wh_open_whiff = (LPFUNC_wh_open_whiff) wh_open_whiff_file;
	if (!pdev->func_wh_open_whiff) {
		printf("Not found: wh_open_whiff \n");
		return 0;
	}

	pdev->func_wh_close_whiff = (LPFUNC_wh_close_whiff) wh_close_whiff_file;
	if (!pdev->func_wh_close_whiff)	{
		printf("Not found: wh_close_whiff \n");
		return 0;
	}

	pdev->func_wh_get_chunk_info = (LPFUNC_wh_get_chunk_info) wh_get_chunk_info;
	if (!pdev->func_wh_get_chunk_info) {
		printf("Not found: wh_get_chunk_info \n");
		return 0;
	}

	pdev->func_wh_get_device_access_func = (LPFUNC_wh_get_device_access_func) wh_get_device_access_func;
	if (!pdev->func_wh_get_device_access_func) {
		printf("Not found: wh_get_device_access_func \n");
		return 0;
	}

	pdev->func_wh_get_device_private_access_func = (LPFUNC_wh_get_device_private_access_func) wh_get_device_private_access_func;
	if (!pdev->func_wh_get_device_private_access_func) {
		printf("Not found: wh_get_device_private_access_func \n");
		return 0;
	}

	pdev->func_wh_get_device_basic_access_func = (LPFUNC_wh_get_device_basic_access_func) wh_get_device_basic_access_func;
	if (!pdev->func_wh_get_device_basic_access_func) {
		printf("Not found: wh_get_device_basic_access_func \n");
		return 0;
	}
	
	return 1;
}

int load_wif(WDT_DEV *pdev, char *path)
{
	if (!pdev || !path)
		return 0;

	strcpy(pdev->wif_access.wif_path, path);

	pdev->wif_access.wif_handle = pdev->func_wh_open_whiff(pdev->wif_access.wif_path);

	if (!pdev->wif_access.wif_handle) {
		wh_printf("Parse input file error\n");
		return 0;
	}

	memset(&pdev->wif_access.wif_chunk_info, 0, sizeof(CHUNK_INFO_EX));

	return 1;
}

int close_wif(WDT_DEV *pdev)
{
	if (!pdev->func_wh_close_whiff(pdev->wif_access.wif_handle)) {
		wh_printf("Close input file error\n");
		return 0;
	}
	return 1;
}

int init_n_scan_device(WDT_DEV *pdev, EXEC_PARAM *pparam, unsigned int flag)
{
	WDT_DEVICE_INFO	wdtDevInfo;

	if (!pdev || !pparam)
		return 0;

	pdev->intf_index = pparam->interface_num;
	pdev->board_info.is_ss_boot_mode = 0;

	if (!pdev->func_wh_get_device_access_func(pdev->intf_index, &pdev->funcs_device)) {
		wh_printf("Get device funcs error \n");
		return 0;		
	}

	if (!strlen(pparam->dev_path)) {
		memset(&wdtDevInfo, 0, sizeof(WDT_DEVICE_INFO));	
		
		/* num of devices */
		int num = pdev->funcs_device.p_wh_scan_device(pdev);
		
		if (num == 0) {
			printf("error: device not found \n");
			return 0;
		}
		if (num > 1) {
			printf("\n");
                        printf("Multiple devices found.\n");
                        printf("This tool currently supports only one device.\n");
                        printf("If you need support for multiple devices, please submit feedback on the GitHub community or email Weida.\n");
			return 0;

		}

		/* just use index 0 to get the device info */
		if (!pdev->funcs_device.p_wh_get_device(pdev, &wdtDevInfo, 0))
			return 0;
			
		strcpy(pdev->dev_path, wdtDevInfo.path);
	}else{
        	struct dirent *dir;
        	pdev->board_info.i2c_address = DEFAULT_I2C_ADDR;
        	wh_printf("%s\n", pparam->dev_path);
        	char i2c_sysfs_path[64] = "/sys/bus/i2c/devices";	
        	strcat(i2c_sysfs_path, strdup(&pparam->dev_path[4]));
        	DIR* d = opendir(i2c_sysfs_path);
        	wh_printf("%s\n", i2c_sysfs_path);
		if (d) {	
			while ((dir = readdir(d)) != NULL) {
				if (memcmp(dir->d_name, ACPI_NAME_HID, strlen(ACPI_NAME_HID)) == 0) {
					char* reg_gen_hid  = strdup(&dir->d_name[4]);
					wh_printf("current reg_gen_hid:%s\n", reg_gen_hid);
					pdev->board_info.i2c_address = get_i2c_address_map(reg_gen_hid);
					wh_printf("i2c_address %x\n", pdev->board_info.i2c_address);
					break;	
				}				
			}
            		closedir(d);
		}
        
		strcpy(pdev->dev_path, pparam->dev_path);
    	}
	if (pdev->funcs_device.p_wh_open_device(pdev)) {
		if (pdev->funcs_device.p_wh_prepare_data(pdev, &pdev->board_info)) {		
			
			if (!pdev->func_wh_get_device_private_access_func(pdev, &pdev->funcs_device_private)) {
				wh_printf("Get device private funcs error\n");
				return 0;		
			}
			return 1;
		}else
			wh_printf("Get system info error\n");
	}
	return 0;
}

void close_device(WDT_DEV *pdev)
{
	if (pdev)
		pdev->funcs_device.p_wh_close_device(pdev);
}





int program_one_chunk(WDT_DEV *pdev, const char *chunk_name, UINT32 chunk_id, UINT32 option, CHUNK_INFO_EX *pinfo)
{
	CHUNK_INFO_EX	chunk_info;
	CHUNK_INFO_EX	*pchunk_info = NULL;

	if (!pdev)
		return 0;

	if (pinfo == NULL) {
		memset(&chunk_info, 0, sizeof(CHUNK_INFO_EX));
		if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, chunk_id, &chunk_info))
			pchunk_info = &chunk_info;
		else
			return 1;
	} else
		pchunk_info = pinfo;

	if (!pchunk_info)
		return 0;
		
	printf("\n[%s] programming ....\n", chunk_name);
				
	if (!pdev->funcs_device_private.p_wh_program_chunk(pdev, pchunk_info, option ))	{
		char	msg[64];

		sprintf(msg, "[%s] Write chunk error", chunk_name);
		wh_printf(msg);
		return 0;	
	}

	printf("[%s] chunk program : pass...\n", chunk_name);
	return 1;
}

int image_file_check(WDT_DEV *pdev, EXEC_PARAM *pparam)
{
	int err = 1;
	if (!pdev)
		return -1;

	if (!init_n_scan_device(pdev, pparam, 0))
		return 0;


	if (pdev->board_info.dev_type & (FW_WDT8790)) {
		return check_fw_by_wif2(pdev, (char*)pparam->image_file);
	}

	if (!load_wif(pdev, (char*)pparam->image_file)){
		printf("Load WIF failed !\n");
		goto exit_func;
	}


	CHUNK_INFO_EX chunkInfoEx;

	memset(&chunkInfoEx, 0, sizeof(CHUNK_INFO_EX));

	printf("version checking ....\n");		// version & body check
	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_FRWR, &chunkInfoEx)){

		printf("fw1 main fw checking ....\n");

		if (!pdev->funcs_device_private.p_wh_verify_chunk(pdev, &chunkInfoEx)) {
			printf("verification fw1 : fail !\n");
			err = 0;
		}
		else
			printf("verification : pass !\n");
	}


	memset(&chunkInfoEx, 0, sizeof(CHUNK_INFO_EX));
	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFG, &chunkInfoEx)){
		printf("config data checking ....\n");

		if (!pdev->funcs_device_private.p_wh_verify_chunk(pdev, &chunkInfoEx)) {
			printf("verification config : fail !\n");
			err = 0;
		}
		else
			printf("verification : pass !\n");
	}

	memset(&chunkInfoEx, 0, sizeof(CHUNK_INFO_EX));
	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFG, &chunkInfoEx)) {
		printf("config1 data checking ....\n");

		if (!pdev->funcs_device_private.p_wh_verify_chunk(pdev, &chunkInfoEx)) {
			printf("verification config1 : fail !\n");
			err = 0;
		}
		else
			printf("verification : pass !\n");
	}

	memset(&chunkInfoEx, 0, sizeof(CHUNK_INFO_EX));
	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_FRWD, &chunkInfoEx)) {
		printf("dual firmware secondary main firmware data checking ....\n");

		if (!pdev->funcs_device_private.p_wh_verify_chunk(pdev, &chunkInfoEx)) {
			err = 0;
			printf("verification dual firmware secondary main : fail !\n");
			goto exit_func;
		}
		else
			printf("verification : pass !\n");
	}

	memset(&chunkInfoEx, 0, sizeof(CHUNK_INFO_EX));
	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFD, &chunkInfoEx)) {
		printf("dual firmware secondary config data checking ....\n");

		if (!pdev->funcs_device_private.p_wh_verify_chunk(pdev, &chunkInfoEx)) {
			printf("verification dual firmware secondary config : fail !\n");
			err = 0;
		}
		else
			printf("verification : pass !\n");
	}



exit_func:
	close_device(pdev);


	if (!close_wif(pdev))
	{
		printf("Close fw file fail !\n");
		return 0;

	}

	printf("Operation done!\n");

	return err;
}



int image_file_burn_data_verify(WDT_DEV *pdev, EXEC_PARAM *pparam)
{
	BOARD_INFO	*pinfo = &pdev->board_info;
	CHUNK_INFO_EX	chunk_info_fw;
	CHUNK_INFO_EX	chunk_info_cfg;
	int		is_fw_update = 1;
	int		is_cfg_update = 1;
	int 	err = 0;


	if (!pdev || !pparam)
		return 0;

	if (pinfo->dev_type & FW_MAYBE_ISP)
		return 0;	

        if (!init_n_scan_device(pdev, pparam, 0)) {
		printf("Wdt controller not found !\n");
                return -2;
        }

	if (pdev->board_info.dev_type & FW_WDT8790) {
		return update_fw_by_wif2(pdev, (char*)pparam->image_file);
	}


	if (!load_wif(pdev, (char*)pparam->image_file)){
		printf("Load WIF failed !\n");
		goto exit_burn;
	}

	
	memset(&chunk_info_fw, 0, sizeof(CHUNK_INFO_EX));
	memset(&chunk_info_cfg, 0, sizeof(CHUNK_INFO_EX));
	
	if (pdev->board_info.dev_type & FW_WDT8755) {
		if (pinfo->dev_info.w8755_dev_info.boot_partition == W8755_BP_SECONDARY) {
			if (!pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_FRWD, &chunk_info_fw)) {
				wh_printf("Not found sec fw chunk !\n");
				goto exit_burn;
			}
			if (!pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFD, &chunk_info_cfg)) {
				wh_printf("Not found sec cfg chunk !\n");
				goto exit_burn;
			}
			if (!chunk_info_fw.chuckInfo.versionNumber) {
				wh_printf("Wif fw version is null!\n");
				goto exit_burn;
			}
		}else{
			if (!pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_FRWR, &chunk_info_fw)) {
				wh_printf("Not found fw chunk !\n");
				goto exit_burn;
			}
			if (!pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFG, &chunk_info_cfg)) {
				wh_printf("Not found cfg chunk !\n");
				goto exit_burn;
			}
			if (!chunk_info_fw.chuckInfo.versionNumber) {
				wh_printf("Wif fw version is null!\n");
				goto exit_burn;
			}
		
		}
	}else{
		if (!pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_FRWR, &chunk_info_fw)) {
			wh_printf("Not found fw chunk !\n");
			goto exit_burn;
		}
		if (!pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFG, &chunk_info_cfg)) {
			wh_printf("Not found cfg chunk !\n");
			goto exit_burn;
		}
		if (!chunk_info_fw.chuckInfo.versionNumber) {
			wh_printf("Wif fw version is null!\n");
			goto exit_burn;
		}
	}

	if (is_cfg_update && chunk_info_cfg.chuckInfo.targetStartAddr) {
		UINT32 tempStartAddr = chunk_info_cfg.chuckInfo.targetStartAddr;

		if (pdev->board_info.dev_type & FW_WDT8755) {
			if (pinfo->dev_info.w8755_dev_info.boot_partition == W8755_BP_SECONDARY) {
				chunk_info_cfg.chuckInfo.targetStartAddr = pdev->board_info.sec_header.w8755_sec_header.secondary_param_clone_addr;
				err = program_one_chunk(pdev, "r_config", CHUNK_ID_CNFD, OPTION_4K_VERIFY, &chunk_info_cfg);
			}else{
				chunk_info_cfg.chuckInfo.targetStartAddr = pdev->board_info.sec_header.w8755_sec_header.param_clone_addr;
				err = program_one_chunk(pdev, "r_config", CHUNK_ID_CNFG, OPTION_4K_VERIFY, &chunk_info_cfg);
			}

			if (!err)
				goto exit_burn;
		}

		chunk_info_cfg.chuckInfo.targetStartAddr = tempStartAddr;

		/* fw should be reprogramed when parameters have been programmed */
		chunk_info_cfg.chuckInfo.temp = chunk_info_fw.chuckInfo.targetStartAddr;
		err = program_one_chunk(pdev, "config", CHUNK_ID_CNFG, OPTION_4K_VERIFY | OPTION_ERASE_TEMP,
								&chunk_info_cfg);

		if (!err)
			goto exit_burn;
	
		if (!program_one_chunk(pdev, "ext bin", CHUNK_ID_EXTB, OPTION_4K_VERIFY, NULL))
			goto exit_burn;

		if (!err)
			goto exit_burn;

		is_fw_update = 1;
	}

	if (pdev->board_info.dev_type & FW_WDT8755) {
		if (chunk_info_fw.chuckInfo.targetStartAddr == 0) {
			printf("Address can not be %d \n", chunk_info_fw.chuckInfo.targetStartAddr);
			goto exit_burn;
		}			
	} else if (pdev->board_info.dev_type & FW_WDT8760_2) {
		if (chunk_info_fw.chuckInfo.targetStartAddr >= 0x60000) {
			printf("Address can not be %d \n", chunk_info_fw.chuckInfo.targetStartAddr);
			goto exit_burn;
		}			
	}

	if (is_fw_update && (chunk_info_fw.chuckInfo.versionNumber)) {
		err = program_one_chunk(pdev, "firmware", CHUNK_ID_FRWR, OPTION_4K_VERIFY, &chunk_info_fw);

		if (!err)
			goto exit_burn;
	}
	
	if (is_fw_update || is_cfg_update) {
		pdev->funcs_device_private.p_wh_send_commands(pdev, WH_CMD_RESET, 0);
		printf("Reset device ... \n");

		wh_sleep(2000);
	}




exit_burn:


	close_device(pdev);	
	close_wif(pdev);
	
	return err;
}


int show_info(WDT_DEV *pdev, EXEC_PARAM *pparam)
{
	BOARD_INFO *pinfo = &pdev->board_info;	
	int ret = 0;
	char str[32];

	if (!pdev || !pparam)
		return 0;

	if (!init_n_scan_device(pdev, pparam, 0))
		return 0;

	ret = !(pinfo->dev_type & FW_MAYBE_ISP);

	if (!ret)
		goto info_exit;


	if (ret) {
		memset(str, '\0', sizeof(str));
                memcpy(str, pinfo->dev_info.w8760_feature_devinfo.platform_id, 8);
                printf("It is %s\n", str);

		printf("Vendor_ID: 0x%04x\n", pinfo->vid);
		printf("Product_ID: 0x%04x\n", pinfo->pid);
		printf("Firmware_ID: 0x%X\n", pinfo->firmware_id);
		printf("Hardware_ID: 0x%X\n", pinfo->hardware_id);		
		printf("Serial_No: 0x%X\n", pinfo->serial_no);
		printf("Platform_ID: 0x%x\n", 0x0);
		printf("XmlId1: %X   XmlId2: %X\n", pinfo->sys_param.xmls_id1, pinfo->sys_param.xmls_id2);
		printf("Param: phy_x %d, phy_y %d\n", pinfo->sys_param.Phy_Frmbuf_W, pinfo->sys_param.Phy_Frmbuf_H);
	} 
info_exit:	
	close_device(pdev);
	
	return ret;
}

int show_wif_info(WDT_DEV *pdev, EXEC_PARAM *pparam)
{
	if (!pdev || !pparam)
		return 0;
	if(show_wif2_info((char*)pparam->image_file))
		return 1;

	if (!load_wif(pdev, (char*)pparam->image_file))
		return 0;

	CHUNK_INFO_EX chunk_info_ex;
	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_FRWR, &chunk_info_ex)) 
		wh_printf("FW id: 0x%08x\n", chunk_info_ex.chuckInfo.versionNumber);

	if (pdev->func_wh_get_chunk_info(pdev->wif_access.wif_handle, CHUNK_ID_CNFG, &chunk_info_ex)) {
		UINT32	config_id = 0;
		
		if (chunk_info_ex.chuckInfo.attribute & 0x2)
			config_id = chunk_info_ex.chuckInfo.versionNumber;
		wh_printf("Config id: 0x%x\n", config_id);		
	}
	
	if (!close_wif(pdev))
		return 0; 
	
	return 1;
}

int find_hid_dev_name(int bus, int vid, int pid, char *device_name)
{
	int ret = 0;
	struct dirent * dev_dir_entry;
	DIR * dev_dir;
	char device_ids[32];

	snprintf(device_ids, 15, "%04X:%04X:%04X", bus, vid, pid);

	dev_dir = opendir("/sys/bus/hid/devices");
	if (!dev_dir) {
		printf("open dev dir failed !\n");
		return 0;
	}

	while ((dev_dir_entry = readdir(dev_dir)) != NULL) {
		if (!strncmp(dev_dir_entry->d_name, device_ids, 14)) {
			strcpy(device_name, dev_dir_entry->d_name);
			ret = 1;
			break;
		}
	}
	closedir(dev_dir);

	return ret;
}

int find_device_name(char *hid_dev_name, char *driver_name, char *driver_path)
{
	char dev_path[] = "/sys/bus/i2c/devices/";

	struct dirent * devs_dir_entry;
	DIR * devs_dir;
	struct dirent * dev_dir_entry;
	DIR * dev_dir;
	int 	device_found = 0;
	ssize_t sz;
	char	tmp_buf[256];
	char	tmp_path[277];

	devs_dir = opendir(dev_path);
	if (!devs_dir) {
		printf("can not open device path: %s\n", dev_path);
		return 0;
	}


	while((devs_dir_entry = readdir(devs_dir)) != NULL) {
		if (devs_dir_entry->d_type != DT_LNK)
			continue;

		sz = readlinkat(dirfd(devs_dir), devs_dir_entry->d_name, tmp_buf, 256);
		if (sz < 0)
			continue;

		tmp_buf[sz] = 0;

		sprintf(tmp_path, "%s%s", dev_path, tmp_buf);

		dev_dir = opendir(tmp_path);
		if (!dev_dir) 
			continue;

		while ((dev_dir_entry = readdir(dev_dir)) != NULL) {
			if (!strcmp(dev_dir_entry->d_name, hid_dev_name)) {
				strcpy(driver_name, devs_dir_entry->d_name);
				device_found = 1;
				break;
			}
		}
		closedir(dev_dir);

		if (device_found){
			strcat(tmp_path, "/uevent");
			FILE *stream;
                        char line[64];

                	stream = fopen(tmp_path, "r");
                	if (stream == NULL) {
            	        	printf("can not open driver path: %s\n", tmp_path);
		        	return 0;
               		}

                	if(fgets (line, 64, stream)!=NULL) {
            			line[strcspn(line, "\n")] = 0;
                        	char *modulename;
                		modulename = strchr(line, '=') +1;
                        	strcat(driver_path, modulename);
                        	strcat(driver_path, "/");
                	}
                	fclose(stream);
                	break;
		}
	}
	closedir(devs_dir);
	return device_found;
}

int write_devname_to_sys_attr(const char *attr, const char *action)
{
	int fd;
	ssize_t size;

	fd = open(attr, O_WRONLY);
	if (fd < 0) {
		wh_printf("%s: open file error !", __func__);
		return 0;
	}

	for (;;) {
		size = write(fd, action, strlen(action));
		if (size < 0) {
			if (errno == EINTR)
				continue;

			return 0;
		}
		break;
	}

	close(fd);

	return (size == (ssize_t) strlen(action));
}



int rebind_driver(WDT_DEV *pdev)
{
	int 	bus = 0x18;
	int 	vendor = pdev->board_info.vid;
	int 	product = pdev->board_info.pid;
	char	hid_dev_name[64];
	char	driver_path[64];
	char	i2c_dev_name[64];
	char	attr_str[70];

	printf("Start to rebind driver !\n");

	if (!find_hid_dev_name(bus, vendor, product, hid_dev_name)) {
		wh_printf("Not found hid device: 0x%x:0x%x:0x%x\n", bus, vendor, product);
		return 0;
	}



	strcpy(driver_path, "/sys/bus/i2c/drivers/");

	if (!find_device_name(hid_dev_name, i2c_dev_name, driver_path)) {
		wh_printf("find device name failed %s\n", hid_dev_name);
		return 0;
	}


	sprintf(attr_str, "%s%s", driver_path, "unbind");


	if (!write_devname_to_sys_attr(attr_str, i2c_dev_name)) {
		printf("failed to unbind HID device %s %s\n", attr_str, i2c_dev_name);
		return 0;
	}
	
	wh_sleep(300);

	sprintf(attr_str, "%s%s", driver_path, "bind");

	if (!write_devname_to_sys_attr(attr_str, i2c_dev_name)) {
		printf("failed to bind HID device %s %s\n", attr_str, i2c_dev_name);
		return 0;
	}

	wh_sleep(300);

	printf("Rebind driver is done !\n");

	return 1;
}


UINT32 get_chunk_fourcc(UINT32 chunk_index)
{
	switch (chunk_index) {
		case	CHUNK_ID_FRMT:
			return FOURCC_ID_FRMT;
		case	CHUNK_ID_FRWR:
			return FOURCC_ID_FRWR;
		case	CHUNK_ID_CNFG:
			return FOURCC_ID_CNFG;
		case	CHUNK_ID_HDRS:
			return FOURCC_ID_HDRS;
		case	CHUNK_ID_FSBT:
			return FOURCC_ID_FSBT;
		case	CHUNK_ID_BINF:
			return FOURCC_ID_BINF;
		case	CHUNK_ID_RCVY:
			return FOURCC_ID_RCVY;
		case	CHUNK_ID_TSTB:
			return FOURCC_ID_TSTB;
		case	CHUNK_ID_EXTB:
			return FOURCC_ID_EXTB;
		case	CHUNK_ID_FRWD:
			return FOURCC_ID_FRWD;
		case 	CHUNK_ID_CNFD:
			return FOURCC_ID_CNFD;
		default:
			return 0;
	}
	return 0;
}

int process_whiff_file(WIF_FILE *pcur_wif)
{
	UINT32*	pstruct = NULL;

	if (!pcur_wif)
		return 0;

	pstruct = (UINT32*) pcur_wif->pdata;

	if (pstruct[0] == FOURCC_ID_RIFF && pstruct[2] == FOURCC_ID_WHIF) {
		
		/* lengths should be the same */
		if (pcur_wif->data_len == pstruct[1]) {
			pcur_wif->pformat_chunk = (FORMAT_CHUNK*) &pstruct[3];
			return 1;
		}
	}

	return 0;
}

int wh_close_whiff_file(WH_HANDLE handle)
{
	WIF_FILE *pcur_wif = (WIF_FILE*) handle;

	if (pcur_wif) {
		if (pcur_wif->pdata)
			free(pcur_wif->pdata);

		free(pcur_wif);
		return 1;
	}

	return 0;
}

WH_HANDLE wh_open_whiff_file(char* path)
{
	FILE*			pfile = NULL;
	WIF_FILE 		*pcur_wif = NULL;

	pfile = fopen(path, "rb");
	if (!pfile)
		return NULL;

	pcur_wif = (WIF_FILE*) malloc(sizeof(WIF_FILE));
	if (!pcur_wif) {
		fclose(pfile);
		return NULL;
	}

	/* set file ptr to the end */
	fseek(pfile, 0, SEEK_END);
	pcur_wif->data_len = ftell(pfile);

	/* set the file ptr the beginning */
	rewind(pfile);		

	pcur_wif->pdata = (BYTE*) malloc(pcur_wif->data_len + 32);

	if (!pcur_wif->pdata)
		goto failed;

	if (fread(pcur_wif->pdata, 1, pcur_wif->data_len, pfile) == pcur_wif->data_len)	{	
		process_whiff_file(pcur_wif);
		fclose(pfile);

		return (WH_HANDLE) pcur_wif;
	}

failed:
	printf("open wif file fail \n");
	wh_close_whiff_file((WH_HANDLE) pcur_wif);
		
	return NULL;
}


int wh_get_chunk_info(WH_HANDLE handle, UINT32 chunk_index, CHUNK_INFO_EX* pchunk_info_ex)
{
	WIF_FILE	*pcur_wif = (WIF_FILE*) handle;
	if (!pcur_wif){
		return 0;
	}

	UINT32	chunk_four_cc = get_chunk_fourcc (chunk_index);

	if (!chunk_four_cc){
		return 0;

	}

	if (!pcur_wif->pformat_chunk){
		return 0;
	}

	/* check if the chunk is existed */
	if (pcur_wif->pformat_chunk->enableFlag | chunk_index) {
		
		/* 12 is the header size */
		UINT32	chunk_start_pos = 12 + sizeof (FORMAT_CHUNK);
		CHUNK_DATA*	pchunk_data = NULL;

		while (chunk_start_pos < pcur_wif->data_len) {
			pchunk_data = (CHUNK_DATA*) & pcur_wif->pdata[chunk_start_pos];

			/* we got it */
			if (pchunk_data->ckID == chunk_four_cc)	{
				memcpy((void*) &pchunk_info_ex->chuckInfo, (void*) &pchunk_data->chunkInfo, sizeof(CHUNK_INFO));
				pchunk_info_ex->length = pchunk_data->chunkInfo.length;
				pchunk_info_ex->pData = (BYTE*) &pcur_wif->pdata[chunk_start_pos + 8 + sizeof(CHUNK_INFO)];
				
				return 1;
			} else
				/* 8 is the header size */
				chunk_start_pos = chunk_start_pos + pchunk_data->ckSize + 8;
		}
	}

	return 0;
}

/* 
 * 	the checksum functions
 */
UINT16 misr_16b(UINT16 current_value, UINT16 new_value)
{
	unsigned int a, b;
	unsigned int bit0;
	UINT16 y;

	a = current_value;
	b = new_value;
	bit0 = a^(b&1);
	bit0 ^= a>>1;
	bit0 ^= a>>2;
	bit0 ^= a>>4;
	bit0 ^= a>>5;
	bit0 ^= a>>7;
	bit0 ^= a>>11;
	bit0 ^= a>>15;
	y = (a<<1)^b;
	y = (y&~1) | (bit0&1);

	return y;
}

UINT16 misr_32b(UINT16 current_value, UINT32 new_word)
{
	UINT16 checksum = misr_16b(current_value, (UINT16)new_word);
	checksum = misr_16b(checksum, (UINT16)(new_word >> 16));
	return checksum;
}




UINT16 misr_for_halfwords(UINT16 current_value, BYTE *buf, int start, int hword_count)
{
	int i;
	UINT32 checksum = current_value;
	UINT16 *p_hword = (UINT16 *)&buf[start];
	
	for (i = 0; i < hword_count; i++)
		checksum = misr_16b(checksum, *p_hword++);

	return checksum;
}

UINT16 misr_for_bytes(UINT16 current_value, BYTE *bytes, int start, int size)
{
	UINT32 checksum = current_value;

	if (size / 2 > 0)
		checksum = misr_for_halfwords(checksum, bytes, start, size / 2);

	if ((size % 2) != 0)
		checksum = misr_16b(checksum, bytes[start + size - 1]);
	
	return checksum;
}

