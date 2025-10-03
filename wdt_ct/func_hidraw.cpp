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


#include "wdt_dev_api.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>


#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <errno.h>

#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>


#include "hidraw_api.h"

#include "w8755_funcs.h"
#include "w8760_funcs.h"
#include "w8790_funcs.h"
#include "wdt_ct.h"



#define		MAX_DEV			16
#define 	VID_WEIDA 		0x2575


static char		g_dev_path[24] = "/dev/hidraw";
static int		g_dev_count = 0;



static	WDT_DEVICE_INFO	g_dev_info[MAX_DEV];
static 	int		g_cur_dev_index = -1;



/*
 * End with input1 
 */
int ends_with_t1(const char* str) {
    // Get the length of the string
    size_t len = strlen(str);
    // Check if the string is long enough to end with ".1"
    if (len < 2) {
        return 0; // Not long enough
    }
    // Compare the last two characters
    return strcmp(str + len - 2, "t1") == 0;
}






// Device Access Functions
int get_device_info(int handle, struct hidraw_devinfo *pinfo)
{
	int rc = 1;

	if (handle < 0 || !pinfo)
		return -1;

	memset(pinfo, 0, sizeof(struct hidraw_devinfo));

	rc = ioctl(handle, HIDIOCGRAWINFO, pinfo);
	if (rc < 0)
		goto error;

	wh_printf("vendor: %04x, bustype: 0x%x\n", pinfo->vendor, pinfo->bustype);

	return 1;

error:
	close(handle);
	
	return rc;
}


int  wh_hidraw_scan_device(WDT_DEV* pdev)
{
	char deviceFile[PATH_MAX];
	int count = 0;
	int handle;
	struct hidraw_devinfo 	hidraw_info;
	g_dev_count = 0;

	memset(g_dev_info, 0, sizeof(WDT_DEVICE_INFO)*MAX_DEV);

	char buf[256] ;
	memset(buf, 0x0, sizeof(buf));

	while (count < MAX_DEV) {
		snprintf(deviceFile, PATH_MAX, "/dev/hidraw%d", count);

		wh_printf("device: %s\n", deviceFile);
		handle = open(deviceFile, O_RDWR|O_NONBLOCK);

		count ++;
		if (handle < 0) {
			continue;
		} else if (get_device_info(handle, &hidraw_info) > 0) {
			if (hidraw_info.vendor == VID_WEIDA)	{
				int res = ioctl(handle, HIDIOCGRAWPHYS(256), buf);
				if (res < 0){
					 perror("HIDIOCGRAWPHYS");
					 close(handle);
					 continue;
				}else{
					//printf("Raw Phys: %s\n", buf);
					//skip the w8730 mouse device. (fw defines interface 1 as mouse device)
					if(ends_with_t1(buf) == 1){
						close(handle);
						continue;
					}
				}

				
				
 				
				g_cur_dev_index = g_dev_count;
			
				strcpy(g_dev_path, deviceFile);
				close(handle);
				//pdev->bustype = hidraw_info.bustype;

				g_dev_info[g_cur_dev_index].vid = hidraw_info.vendor;
				g_dev_info[g_cur_dev_index].pid = hidraw_info.product;

				strcpy(g_dev_info[g_cur_dev_index].path, g_dev_path);

                                   				    
			        wh_printf("path%d=%s\n", g_cur_dev_index, g_dev_path);
				g_dev_count ++;

			}
		} else {
			close(handle);
			continue;
		}
               		
	}

	return g_dev_count;
	

}

int wh_hidraw_get_device(WDT_DEV* pdev, WDT_DEVICE_INFO *pDevInfo, int flag)
{
	int index = flag & 0xFF;

	int option = flag & 0xFFFFFF00;


	if (!option && index < g_dev_count)
	{
		strcpy(pDevInfo->path, g_dev_path);
		g_cur_dev_index = index;
		return 1;
	} else {
		index = 0;

		while (index < g_dev_count) {
			index ++;
		}
	}
	return 0;

}

int wh_hidraw_open_device(WDT_DEV* pdev)
{
	if (!pdev)
		return 0;
	else { 
		pdev->dev_handle = hidraw_open_path(pdev->dev_path);
		return 1;
	}

	return 0;
}

int wh_hidraw_close_device(WDT_DEV* pdev)
{
	if (pdev && pdev->dev_handle) {
		hidraw_close((hidraw_device *) pdev->dev_handle);
		hidraw_exit();
		return 1;
	}
	return 0;
}

int wh_w8755_hidraw_prepare_data(WDT_DEV* pdev, BOARD_INFO* p_out_board_info, int maybe_isp)
{
	maybe_isp = 0;
	/*
	// initialize the basic function for handling the following operations
	wh_w8755_dev_set_basic_op(pdev);	
	if (!wh_w8755_dev_identify_platform(pdev)) {
		printf("Inconsistant data!\n");
		return 0;
	}

	if (!wh_w8755_dev_parse_new_dev_info(pdev, &p_out_board_info->dev_info.w8755_dev_info)) {
		printf("Can't get new device info!\n");
		return 0;
	}


	memcpy(&pdev->board_info.dev_info.w8755_dev_info, &p_out_board_info->dev_info.w8755_dev_info, sizeof(W8755_DEV_INFO_NEW));

	if (!wh_w8755_dev_read_flash_map(pdev, p_out_board_info)) {
		printf("Can't get address table!\n");
		return 0;		
	}


	wh_w8755_dev_set_device_mode(pdev, W8755_DM_SENSING);

*/
	return 1;
}

int wh_hidraw_prepare_data(WDT_DEV *pdev, BOARD_INFO* p_out_board_info)
{
	BYTE	buf[70];
	hidraw_device	*phid;
	BOARD_INFO		board_info;
	struct hidraw_devinfo 	hidraw_info;
	int 	ret = 0;
	int 	id;

	if (!pdev || !pdev->dev_handle) {
		printf("device ptr is null !\n");
		return 0;
	}

	memset(&board_info, 0, sizeof(BOARD_INFO));

	phid = (hidraw_device *) pdev->dev_handle;
	if (get_device_info(phid->device_handle, &hidraw_info) < 0) {
		printf("Can't get device info !\n");
		return 0;		
	}

	board_info.vid = (UINT16) hidraw_info.vendor;
	board_info.pid = (UINT16) hidraw_info.product;
	//pdev->bustype = hidraw_info.bustype;

	buf[0] = VND_REQ_DEV_INFO;
	ret = wh_hidraw_get_feature(pdev, buf, 64);
	id = get_unaligned_le32(&buf[1]);

	if (ret <= 0 || id == 0) {	
		board_info.dev_type = FW_MAYBE_ISP;	
		printf("Can't get fw id, should be in ISP mode !\n");

        if ((buf[0x26] == 0x49 && buf[0x27] == 0x53 && buf[0x28] == 0x50)) {
			printf("It is maybe WDT8760 ISP\n");
			board_info.dev_type = FW_WDT8760_2_ISP;
		}
	}		
		
	if (buf[0] != VND_REQ_DEV_INFO)	{
		board_info.dev_type = FW_MAYBE_ISP;	
		printf("Firmware id packet error !\n");
	}

	// we don't need this check here or it will get failed
	if (!(board_info.dev_type & ( FW_WDT8760_2_ISP))) {
		board_info.firmware_id = id;
		board_info.hardware_id = get_unaligned_le32(buf + 5);
		board_info.serial_no = get_unaligned_le32(buf + 9);
		board_info.dev_type = check_firmware_id(pdev, board_info.firmware_id);
	}
	

	if (board_info.dev_type & (FW_WDT8760)) {
		wh_w8760_get_feature_devinfo(&board_info.dev_info.w8760_feature_devinfo, buf);	
		
		memcpy(&pdev->board_info.dev_info.w8760_feature_devinfo, &board_info.dev_info.w8760_feature_devinfo, 
			sizeof(W8760_REPORT_FEATURE_DEVINFO));
		
		if (wh_w8760_prepare_data(pdev, &board_info))
			ret = 1;
		goto exit_prepare_data;
	}

	if (board_info.dev_type & FW_WDT8790) {
		wh_w8790_parse_device_info(&board_info.dev_info.w8790_feature_devinfo, buf);

		memcpy(&pdev->board_info.dev_info.w8790_feature_devinfo, &board_info.dev_info.w8790_feature_devinfo,
			sizeof(W8790_DEV_INFO));
		if (wh_w8790_prepare_data(pdev, &board_info))
			ret = 1;
		goto exit_prepare_data;
	}



	if (board_info.dev_type & FW_WDT8755) { 
		// set this value before we go into other w8755 functions
		pdev->board_info.dev_type = FW_WDT8755;	
		
		buf[0] = 0xf4;		
		if (!wh_hidraw_get_feature(pdev, buf, 56)) 
			printf("failed to get i2c cfg\n");
		else 
			if (buf[0] != 0xf4)
				printf("wrong id[0xf4] of fw response: 0x%x\n", buf[0]);
			else
				board_info.i2c_dummy = buf[1];

		if (buf[0] == 0xf4 && (get_unaligned_le16(buf + 2) == 0x154f)) { 	
			board_info.dev_type |= FW_WDT8755;

			memcpy(&board_info.sys_param, &buf[10], get_unaligned_le16(buf + 12));

			if (wh_w8755_hidraw_prepare_data(pdev, &board_info, 0)) 
				ret = 1;
		} else
			printf("error in parse 0xf4 packet\n");

		goto exit_prepare_data;
	}	

	printf("*** string feature is not supported in hidraw,\n skip to get parameters\n");

exit_prepare_data:
	if (p_out_board_info) {
		memcpy(p_out_board_info, &board_info, sizeof(BOARD_INFO));
		return ret;
	}
	
	return 0;
}

int wh_hidraw_set_feature(WDT_DEV *pdev, BYTE* buf, UINT32 buf_size)
{
	int retval;

	if (buf_size > 64)
		buf_size = 64;

	if (!buf || !pdev) {
		wh_printf("%s : pointer is null\n", __FUNCTION__);
		return 0;
	}


	retval = hidraw_send_feature_report((hidraw_device*) pdev->dev_handle, buf, buf_size);		
	
#if	DELAY_BY_CMD
	if (retval && (pdev->board_info.dev_type & FW_WDT8755))
		w8755_delay_by_cmd(pdev, buf);
#endif	
	
	return retval;
}

int wh_hidraw_get_feature(WDT_DEV *pdev, BYTE* buf, UINT32 buf_size)
{
	if (buf_size > 64)
		buf_size = 64;

	if (!buf || !pdev) {
		wh_printf("%s : pointer is null\n", __FUNCTION__);
		return 0;
	}

	return hidraw_get_feature_report((hidraw_device*) pdev->dev_handle, buf, buf_size);
}

int wh_hidraw_get_indexed_string(WDT_DEV *pdev, UINT32 index, BYTE* buf, UINT32 buf_size)
{
	return hidraw_get_indexed_string((hidraw_device*) pdev->dev_handle, index, (wchar_t*) buf, buf_size);	
}

int wh_hidraw_read(WDT_DEV *pdev, BYTE* buf, UINT32 buf_size)
{
	return hidraw_read_timeout((hidraw_device*) pdev->dev_handle, buf, buf_size, 10);
}


