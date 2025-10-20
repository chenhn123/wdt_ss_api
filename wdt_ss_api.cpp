/*
 * Copyright (C) 2025 Randy L.
 * Copyright (C) 2025 Weida Hi-Tech
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




#include "wdt_ss_api.h"

#include "wdt_ct/wdt_dev_api.h"
#include "wdt_ct/wdt_ct.h"

int wdt_ss_get_api_version(void)
{
	return WDT_SS_API_VERSION;
}


int wdt_ss_update_firmware(char* path)
{
	int ret = update_firmware_internal(path);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else 
		return ret;
}


int wdt_ss_verify_firmware(char* path)
{
	int ret = verify_firmware_internal(path);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else 
		return ret;

}


int wdt_ss_get_current_firmware_version(unsigned int* version)
{
	int ret = get_current_firmware_version_internal(version);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else 
		return ret;
}


int wdt_ss_get_device_name(char* name, size_t max_len)
{
	int ret = get_device_name_internal(name, max_len);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else 
		return ret;

}

int wdt_ss_get_vid_pid(unsigned int* vid, unsigned int* pid)
{
	int ret = get_vid_pid_internal(vid, pid);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else 
		return ret;
}


int wdt_ss_get_hardware_id(unsigned int* hardware_id)
{
	int ret = get_hardware_id_internal(hardware_id);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else 
		return ret;
}

int wdt_ss_get_device_info(unsigned int *vid, unsigned int *pid, unsigned int* hardware_id, unsigned int* version)
{
	int ret = get_device_info_internal(vid, pid, hardware_id, version);
	if(ret == 1)
		return 0;
	else if(ret == 0)
		return 1;
	else
		return ret;

}


