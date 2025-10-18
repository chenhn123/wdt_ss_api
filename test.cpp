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


#include "stdio.h"
#include "wdt_ss_api.h"


int main(int argc, char *argv[]) {
	int ret;
	int api_version = wdt_ss_get_api_version();
	int major = (api_version >> 16) & 0xFF;
	int minor = (api_version >> 8) & 0xFF;
	int patch = api_version & 0xFF;

	printf("API Version: %d.%d.%d\n", major, minor, patch);

	unsigned int vid = 0;
	unsigned int pid = 0;
	ret = wdt_ss_get_vid_pid(&vid, &pid);
	
	printf("vid,pid: 0x%x, 0x%x, ret:%d \n", vid, pid, ret);

	unsigned int fw_version = 0;
	ret = wdt_ss_get_current_firmware_version(&fw_version);
	printf("FW version:0x%x ret:%d \n", fw_version, ret);

	char name[9];
	ret = wdt_ss_get_device_name(name, 16);
	printf("device_name: %s \n", name);


	if(argc < 2) {
		printf("Usage: %s <path>\n", argv[0]);
	}else{
		ret = wdt_ss_verify_firmware(argv[1]);
		printf("verify_fw ret:%d \n", ret);
		if(ret == 0)
			printf("device fw and file fw are identical\n");
		else
			printf("device fw and file fw mismatch\n");

	}


	if(argc < 2) {
		printf("Usage: %s <path>\n", argv[0]);
	}else{
		ret = wdt_ss_update_firmware(argv[1]);
		printf("update_fw ret:%d \n", ret);
	}
	
	if(argc < 2) {
		printf("Usage: %s <path>\n", argv[0]);
	}else{
		ret = wdt_ss_verify_firmware(argv[1]);
		printf("verify_fw ret:%d \n", ret);
		if(ret == 0)
			printf("device fw and file fw are identical\n");
		else
			printf("device fw and file fw mismatch\n");

	}
	
	
	return ret;
}
