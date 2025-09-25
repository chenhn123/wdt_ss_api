
#include "wdt_ss_api.h"

#include "wdt_ct/wdt_dev_api.h"
#include "wdt_ct/wdt_ct.h"
#define WDT_SS_API_VERSION_MAJOR 1
#define WDT_SS_API_VERSION_MINOR 0
#define WDT_SS_API_VERSION_PATCH 0

#define WDT_SS_API_VERSION \
    ((WDT_SS_API_VERSION_MAJOR << 16) | (WDT_SS_API_VERSION_MINOR << 8) | WDT_SS_API_VERSION_PATCH)

int wdt_ss_get_api_version(void)
{
	return WDT_SS_API_VERSION;
}


int wdt_ss_firmware_update(char* path)
{
	char* argv[2];
	argv[0] = (char*)"-u";
	argv[1] = path;
	int argc = 2;	

	return run_from_lib(argc, argv);

}




int wdt_ss_firmware_verification(char* path)
{
	return 1;
}


int wdt_ss_get_current_firmware_version(unsigned int* version)
{
	return 1;
}


int wdt_ss_get_device_name(char* name, size_t max_len)
{

	return 1;
}

int wdt_ss_get_vid_pid(unsigned int* vid, unsigned int* pid)
{
    	int status = get_vid_pid_internal(vid, pid);
	return status;
}


