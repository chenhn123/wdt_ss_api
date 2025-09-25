#include "stdio.h"
#include "wdt_ss_api.h"


int main(int argc, char *argv[]) {
	int ret;
	int result = wdt_ss_get_api_version();
	printf("api version %d \n", result);

	char device_name[256];
	
	unsigned int vid = 0;
	unsigned int pid = 0;
	ret = wdt_ss_get_vid_pid(&vid, &pid);
	
	printf("0x%x, 0x%x \n", vid, pid);
	if(argc < 2) {
		printf("Usage: %s <path>\n,", argv[0]);
	}
	else{

		ret = wdt_ss_firmware_update(argv[1]);
	}

	
	return ret;
}
