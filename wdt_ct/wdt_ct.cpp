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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include <stdarg.h>

#include "wdt_dev_api.h"
#include "wdt_ct.h"

static int g_show_extra_info = 0;



void wh_printf(const char *fmt, ...)
{
	if (!g_show_extra_info)
		return;
	
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void wh_sleep(int ms)
{
	usleep(ms * 1000);
}

void wh_udelay(int us)
{
	usleep(us);
}

unsigned long get_current_ms()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int check_privilege()
{
	uid_t uid = getuid();
	uid_t euid = geteuid();

	if (uid == 0 || euid == 0)
		return 1;

	return 0;
}

int check_file(char* filename)
{
	FILE*	p_file = 0;

	p_file = fopen(filename, "r");

	if (p_file) {
		
		/* file existed */
		fclose(p_file);
		return 1;		
	}

	return 0;
}


int update_firmware_internal(char* path)
{
	
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);
	
	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));

	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_UPDATE;
	exec_param.image_file = path;


	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;
	if (exec_param.argus & OPTION_UPDATE)
		LPFUNC_execution = image_file_burn_data_verify; 
	
	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);

	return ret;

}



int verify_firmware_internal(char* path)
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);
	
	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));

	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_VERIFY;

	exec_param.image_file = path;

	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;
	if (exec_param.argus & OPTION_VERIFY)
		LPFUNC_execution = image_file_check;
	
	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);

	return ret;

}


int get_device_name_internal(char *name, size_t max_len)
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);

	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));
	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_INFO;


	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;
	if (exec_param.argus & OPTION_INFO)
		LPFUNC_execution = show_info; 
	
	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);
	
	if (name == NULL) {
        	return -1; // handle error safely
	}
	memcpy(name, wdt_dev.board_info.device_name, max_len);


	
	return ret;



}

int get_current_firmware_version_internal(unsigned int *version)
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);

	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));
	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_INFO;


	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;
	if (exec_param.argus & OPTION_INFO)
		LPFUNC_execution = show_info; 
	
	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);
	
	if (version == NULL) {
        	return -1; // handle error safely
	}


	*version = wdt_dev.board_info.serial_no;
	
	return ret;

}

int get_vid_pid_internal(unsigned int *vid, unsigned int *pid)
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);

	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));
	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_INFO;


	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;
	if (exec_param.argus & OPTION_INFO)
		LPFUNC_execution = show_info; 

	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);
	
	if (vid == NULL|| pid == NULL) {
        	return -1; // handle error safely
	}


	*vid = wdt_dev.board_info.vid;
	*pid = wdt_dev.board_info.pid;
	
	return ret;

}




int get_hardware_id_internal(unsigned int *hw_id)
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);

	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));
	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_INFO;


	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;

	if (exec_param.argus & OPTION_INFO)
		LPFUNC_execution = show_info; 

	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);
	
	if (hw_id == NULL) {
        	return -1; // handle error safely
	}

	*hw_id = wdt_dev.board_info.hardware_id;
	
	return ret;

}


int get_device_info_internal(unsigned int *vid, unsigned int *pid, unsigned int *hw_id, unsigned int *version)
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);

	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));
	exec_param.interface_num = INTERFACE_HIDRAW;
	exec_param.argus |= OPTION_INFO;


	if (!check_privilege()) {
		printf("Must be a root to run this program!\n");
		return 0;
	}

	memset(&wdt_dev, 0, sizeof(WDT_DEV));
	
	if (!load_lib_func_address(&wdt_dev, &exec_param)) {
		printf("Load function table failed !\n");
		return 0;
	}

	wdt_dev.pparam = &exec_param;

	LPFUNC_execution = NULL;

	if (exec_param.argus & OPTION_INFO)
		LPFUNC_execution = show_info; 

	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);
	
	if (hw_id == NULL) {
        	return -1; // handle error safely
	}

	*hw_id = wdt_dev.board_info.hardware_id;
	*vid = wdt_dev.board_info.vid;
	*pid = wdt_dev.board_info.pid;
	*version = wdt_dev.board_info.serial_no;
	
	
	return ret;

}



