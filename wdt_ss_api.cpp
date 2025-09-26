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
	return update_firmware_internal(path);
}


int wdt_ss_verify_firmware(char* path)
{
	return verify_firmware_internal(path);
}


int wdt_ss_get_current_firmware_version(unsigned int* version)
{
	return get_current_firmware_version_internal(version);
}


int wdt_ss_get_device_name(char* name, size_t max_len)
{
	return get_device_name_internal(name, max_len);
}

int wdt_ss_get_vid_pid(unsigned int* vid, unsigned int* pid)
{
    	return  get_vid_pid_internal(vid, pid);
}


