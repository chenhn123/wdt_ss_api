#include "stdio.h"
#include "wdt_ss_api.h"


int main() {
	int ret;
	int result = wdt_ss_get_api_version();
	printf("api version %d \n", result);

	char device_name[256];
	
	uint32_t vid = 0;
	uint32_t pid = 0;
	ret = wdt_ss_get_vid_pid(&vid, &pid);
	
	printf("0x%x, 0x%x \n", vid, pid);
	
	return 0;
}
