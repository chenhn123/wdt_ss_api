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

#define		TOOL_TITLE_STR			"wdt_ct_oss"
#define		TOOL_VERSION_STR		"1.0.5"


#define WDT_UTIL_GETOPTS	"hdv:fbrsw:vci:j:"

static struct option long_options[] = {
	{"help", 0, NULL, 'h'},
	{"debug",0, NULL, 'd'},		
	{"update", 1, NULL, 'u'},
	{"info", 0, NULL, 's'},	
	{"cksum", 0, NULL, 'j'},
	{"intf", 1, NULL, 'i'},	
	{0, 0, 0, 0},
};

static int g_show_extra_info = 0;

void print_version()
{
	printf("%s %s\n", TOOL_TITLE_STR, TOOL_VERSION_STR);
}

void print_help(const char *prog_name)
{
	print_version();
	
	printf("\nUsage: %s [OPTIONS] [FW_FILE|SUB-ARGU]\n", prog_name);
	printf("\t-h, --help\tPrint this message\n");
	printf("\t-d, --debug\n");	
	printf("\t-v, --update\tUpdate firmware with verification\n");
	printf("\t-s, --info\tPrint the info associated with the devcie\n");
	printf("\t-j, --verify device checksum with fw\n");
}

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

int parse_args(int argc, char* argv[], EXEC_PARAM* pparam)
{
	int 	opt;
	int 	index;

	/* only support HIDRAW here */
	pparam->interface_num = INTERFACE_HIDRAW;

	while ((opt = getopt_long(argc, argv, WDT_UTIL_GETOPTS, long_options, &index)) != -1) {
		switch (opt) {
			case 'h':
				print_help(argv[0]);
				return 0;
			case 'd':
				g_show_extra_info = 1;
				break;
			case 'v':
				pparam->argus |= OPTION_UPDATE;
				pparam->image_file = optarg;
				break;
			case 'j':
				pparam->argus |= OPTION_VERIFY;
				pparam->image_file = optarg;
				break;
			case 's':
				pparam->argus |= OPTION_INFO;
				break;				
			case 'i':
				break;
			default:
				break;

		}
	}

	if (optind != argc) {
		int i=0; 
		while (i<argc) {
			printf("%s ", argv[i]);
			i++;
		}
		printf("\n");
		
		print_help(argv[0]);
		return 0;
	}

	return pparam->argus;
}


int main(int argc, char * argv[]) 
{
	int		ret = 0;
	EXEC_PARAM	exec_param;
	WDT_DEV		wdt_dev;
	unsigned long	start_tick;
	int (*LPFUNC_execution)(WDT_DEV*, EXEC_PARAM*);

	memset((void*) &exec_param, 0, sizeof(EXEC_PARAM));
	if(!parse_args(argc, argv, &exec_param)) {
		printf("parse arg fail!\n");
		return 0;
	}

	if (!exec_param.argus )
		print_version();

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
	start_tick = get_current_ms();

	LPFUNC_execution = NULL;
	if (exec_param.argus & OPTION_UPDATE)
		LPFUNC_execution = image_file_burn_data_verify; 
	else if (exec_param.argus & OPTION_INFO)
		LPFUNC_execution = show_info; 
	else if (exec_param.argus & OPTION_VERIFY)
		LPFUNC_execution = image_file_check; 
	
	if (LPFUNC_execution)
		ret = LPFUNC_execution(&wdt_dev, &exec_param);

	if (exec_param.argus & OPTION_UPDATE) {
		printf("It takes %ums\n", (unsigned int) (get_current_ms() - start_tick));
	}

		/*
		*
		*
			The Spec from 4POS
			Success -> 0
			Touch Controller is correct, but FW Updated failed -> 1
			Touch Panel match fails(this means that it is the old touch controller OR a completely other touch controller for example from eGalaxy) -> 2
			Unkown -> Others
		 */

	wh_printf("ret: %d\n", ret);

	if (exec_param.argus & OPTION_UPDATE) {

		if (ret == 1)
			return 0;
		else if (ret < 0) {
			return -ret;
		}
		else if (ret == 0)
			return 1;
		else
			return ret;

	}


	if (ret == 1)
		return 0;
	else
		return 1;
	
}
