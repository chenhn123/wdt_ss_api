/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 8/22/2009
 Linux Version - 6/2/2009

 Copyright 2009, All Rights Reserved.

 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU General Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        http://github.com/signal11/hidapi .
********************************************************/

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <errno.h>

/* Unix */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <poll.h>

/* Linux */
#include <linux/hidraw.h>
#include <linux/version.h>
#include <linux/input.h>

#include "wdt_dev_api.h"
#include "hidraw_api.h"
#include "wdt_ct.h"




/* Definitions from linux/hidraw.h. Since these are new, some distros
   may not have header files which contain them. */
#ifndef HIDIOCSFEATURE
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#endif
#ifndef HIDIOCGFEATURE
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif


/* USB HID device property names */
const char *device_string_names[] = {
	"manufacturer",
	"product",
	"serial",
};

/* Symbolic names for the properties above */
enum device_string_id {
	DEVICE_STRING_MANUFACTURER,
	DEVICE_STRING_PRODUCT,
	DEVICE_STRING_SERIAL,

	DEVICE_STRING_COUNT,
};

#if 0
struct hid_device_ {
	int device_handle;
	int blocking;
	int uses_numbered_reports;
};
#endif


static __u32 kernel_version = 0;

static hidraw_device *new_hid_device(void)
{
	hidraw_device *dev = calloc(1, sizeof(hidraw_device));
	dev->device_handle = -1;
	dev->blocking = 1;
	dev->uses_numbered_reports = 0;

	return dev;
}

#if 0
/* The caller must free the returned string with free(). */
static wchar_t *utf8_to_wchar_t(const char *utf8)
{
	wchar_t *ret = NULL;

	if (utf8) {
		size_t wlen = mbstowcs(NULL, utf8, 0);
		if ((size_t) -1 == wlen) {
			return wcsdup(L"");
		}
		ret = calloc(wlen+1, sizeof(wchar_t));
		mbstowcs(ret, utf8, wlen+1);
		ret[wlen] = 0x0000;
	}

	return ret;
}
#endif


/* uses_numbered_reports() returns 1 if report_descriptor describes a device
   which contains numbered reports. */
static int uses_numbered_reports(__u8 *report_descriptor, __u32 size) {
	unsigned int i = 0;
	int size_code;
	int data_len, key_size;

	while (i < size) {
		int key = report_descriptor[i];

		/* Check for the Report ID key */
		if (key == 0x85/*Report ID*/) {
			/* This device has a Report ID, which means it uses
			   numbered reports. */
			return 1;
		}

		//printf("key: %02hhx\n", key);

		if ((key & 0xf0) == 0xf0) {
			/* This is a Long Item. The next byte contains the
			   length of the data section (value) for this key.
			   See the HID specification, version 1.11, section
			   6.2.2.3, titled "Long Items." */
			if (i+1 < size)
				data_len = report_descriptor[i+1];
			else
				data_len = 0; /* malformed report */
			key_size = 3;
		}
		else {
			/* This is a Short Item. The bottom two bits of the
			   key contain the size code for the data section
			   (value) for this key.  Refer to the HID
			   specification, version 1.11, section 6.2.2.2,
			   titled "Short Items." */
			size_code = key & 0x3;
			switch (size_code) {
			case 0:
			case 1:
			case 2:
				data_len = size_code;
				break;
			case 3:
				data_len = 4;
				break;
			default:
				/* Can't ever happen since size_code is & 0x3 */
				data_len = 0;
				break;
			};
			key_size = 1;
		}

		/* Skip over this key and it's associated data */
		i += data_len + key_size;
	}

	/* Didn't find a Report ID key. Device doesn't use numbered reports. */
	return 0;
}

/*
 * The caller is responsible for free()ing the (newly-allocated) character
 * strings pointed to by serial_number_utf8 and product_name_utf8 after use.
 */
#if 0
static int
parse_uevent_info(const char *uevent, int *bus_type,
	unsigned short *vendor_id, unsigned short *product_id,
	char **serial_number_utf8, char **product_name_utf8)
{
	char *tmp = strdup(uevent);
	char *saveptr = NULL;
	char *line;
	char *key;
	char *value;

	int found_id = 0;
	int found_serial = 0;
	int found_name = 0;

	line = strtok_r(tmp, "\n", &saveptr);
	while (line != NULL) {
		/* line: "KEY=value" */
		key = line;
		value = strchr(line, '=');
		if (!value) {
			goto next_line;
		}
		*value = '\0';
		value++;

		if (strcmp(key, "HID_ID") == 0) {
			/**
			 *        type vendor   product
			 * HID_ID=0003:000005AC:00008242
			 **/
			int ret = sscanf(value, "%x:%hx:%hx", bus_type, vendor_id, product_id);
			if (ret == 3) {
				found_id = 1;
			}
		} else if (strcmp(key, "HID_NAME") == 0) {
			/* The caller has to free the product name */
			*product_name_utf8 = strdup(value);
			found_name = 1;
		} else if (strcmp(key, "HID_UNIQ") == 0) {
			/* The caller has to free the serial number */
			*serial_number_utf8 = strdup(value);
			found_serial = 1;
		}

next_line:
		line = strtok_r(NULL, "\n", &saveptr);
	}

	free(tmp);
	return (found_id && found_name && found_serial);
}
#endif

static int get_device_string(hidraw_device *dev, enum device_string_id key, wchar_t *string, size_t maxlen)
{
	int ret = 0;

	printf("etner %s\n", __func__);
	
	return ret;
}

int HID_API_EXPORT hidraw_init(void)
{
	const char *locale;

	/* Set the locale if it's not set. */
	locale = setlocale(LC_CTYPE, NULL);
	if (!locale)
		setlocale(LC_CTYPE, "");

	return 0;
}

int HID_API_EXPORT hidraw_exit(void)
{
	/* Nothing to do for this in the Linux/hidraw implementation. */
	return 0;
}


struct hidraw_device_info  HID_API_EXPORT *hidraw_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	struct hidraw_device_info *root = NULL; /* return object */
	
	return root;
}

void  HID_API_EXPORT hidraw_free_enumeration(struct hidraw_device_info *devs)
{
	struct hidraw_device_info *d = devs;
	while (d) {
		struct hidraw_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
}

hidraw_device * hidraw_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
	struct hidraw_device_info *devs, *cur_dev;
	const char *path_to_open = NULL;
	hidraw_device *handle = NULL;

	devs = hidraw_enumerate(vendor_id, product_id);
	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == vendor_id &&
		    cur_dev->product_id == product_id) {
			if (serial_number) {
				if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
					path_to_open = cur_dev->path;
					break;
				}
			}
			else {
				path_to_open = cur_dev->path;
				break;
			}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		/* Open the device */
		handle = hidraw_open_path(path_to_open);
	}

	hidraw_free_enumeration(devs);

	return handle;
}

hidraw_device * HID_API_EXPORT hidraw_open_path(const char *path)
{
	hidraw_device *dev = NULL;

	hidraw_init();

	dev = new_hid_device();

	if (kernel_version == 0) {
		struct utsname name;
		int major, minor, release;
		int ret;
		uname(&name);
		ret = sscanf(name.release, "%d.%d.%d", &major, &minor, &release);
		if (ret == 3) {
			kernel_version = major << 16 | minor << 8 | release;
			//printf("Kernel Version: %d\n", kernel_version);
		}
		else {
			printf("Couldn't sscanf() version string %s\n", name.release);
		}
	}

	/* OPEN HERE */
	dev->device_handle = open(path, O_RDWR);

	/* If we have a good handle, return it. */
	if (dev->device_handle > 0) {

		/* Get the report descriptor */
		int res, desc_size = 0;
		struct hidraw_report_descriptor rpt_desc;

		memset(&rpt_desc, 0x0, sizeof(rpt_desc));

		/* Get Report Descriptor Size */
		res = ioctl(dev->device_handle, HIDIOCGRDESCSIZE, &desc_size);
		if (res < 0)
			perror("HIDIOCGRDESCSIZE");


		/* Get Report Descriptor */
		rpt_desc.size = desc_size;
		res = ioctl(dev->device_handle, HIDIOCGRDESC, &rpt_desc);
		if (res < 0) {
			perror("HIDIOCGRDESC");
		} else {
			/* Determine if this device uses numbered reports. */
			dev->uses_numbered_reports =
				uses_numbered_reports(rpt_desc.value,
				                      rpt_desc.size);
		}

		return dev;
	}
	else {
		/* Unable to open any devices. */
		free(dev);
		return NULL;
	}
}


int HID_API_EXPORT hidraw_write(hidraw_device *dev, const unsigned char *data, size_t length)
{
	int bytes_written;

	bytes_written = write(dev->device_handle, data, length);

	return bytes_written;
}


int HID_API_EXPORT hidraw_read_timeout(hidraw_device *dev, unsigned char *data, size_t length, int milliseconds)
{
	int bytes_read;

	if (milliseconds >= 0) {
		/* Milliseconds is either 0 (non-blocking) or > 0 (contains
		   a valid timeout). In both cases we want to call poll()
		   and wait for data to arrive.  Don't rely on non-blocking
		   operation (O_NONBLOCK) since some kernels don't seem to
		   properly report device disconnection through read() when
		   in non-blocking mode.  */
		int ret;
		struct pollfd fds;

		fds.fd = dev->device_handle;
		fds.events = POLLIN;
		fds.revents = 0;
		ret = poll(&fds, 1, milliseconds);
		if (ret == -1 || ret == 0) {
			/* Error or timeout */
			return ret;
		}
		else {
			/* Check for errors on the file descriptor. This will
			   indicate a device disconnection. */
			if (fds.revents & (POLLERR | POLLHUP | POLLNVAL))
				return -1;
		}
	}

	bytes_read = read(dev->device_handle, data, length);
	if (bytes_read < 0 && (errno == EAGAIN || errno == EINPROGRESS))
		bytes_read = 0;

	if (bytes_read >= 0 &&
	    kernel_version < KERNEL_VERSION(2,6,34) &&
	    dev->uses_numbered_reports) {
		/* Work around a kernel bug. Chop off the first byte. */
		memmove(data, data+1, bytes_read);
		bytes_read--;
	}

	return bytes_read;
}

int HID_API_EXPORT hidraw_read(hidraw_device *dev, unsigned char *data, size_t length)
{
	return hidraw_read_timeout(dev, data, length, (dev->blocking)? -1: 0);
}

int HID_API_EXPORT hidraw_set_nonblocking(hidraw_device *dev, int nonblock)
{
	/* Do all non-blocking in userspace using poll(), since it looks
	   like there's a bug in the kernel in some versions where
	   read() will not return -1 on disconnection of the USB device */

	dev->blocking = !nonblock;
	return 0; /* Success */
}


int HID_API_EXPORT hidraw_send_feature_report(hidraw_device *dev, const unsigned char *data, size_t length) // int type)
{
	int res;

//	if (type)
		res = ioctl(dev->device_handle, HIDIOCSFEATURE(length), data);
/*
	else {
		if(length <= 8)
			res = ioctl(dev->device_handle, HIDIOCSFEATURE(8), data);	
		else
			res = ioctl(dev->device_handle, HIDIOCSFEATURE(64), data);
	}
*/	

	if (res < 0)
		printf("ioctl (SFEATURE): error (%d)\n", res);

	usleep(300);

	return (res >= 0);
}

int HID_API_EXPORT hidraw_get_feature_report(hidraw_device *dev, unsigned char *data, size_t length) //, int type)
{
	int res;
	
	//if (type)
		res = ioctl(dev->device_handle, HIDIOCGFEATURE(length), data);
/*	
	else {
		if(length <= 8)
			res = ioctl(dev->device_handle, HIDIOCGFEATURE(8), data);
		else
			res = ioctl(dev->device_handle, HIDIOCGFEATURE(64), data);
	}
	*/

	if (res < 0)
		printf("ioctl (GFEATURE): error (%d)\n", res);

	usleep(300);

	return (res >= 0);
}


void HID_API_EXPORT hidraw_close(hidraw_device *dev)
{
	if (!dev)
		return;
	close(dev->device_handle);
	free(dev);
}


int HID_API_EXPORT_CALL hidraw_get_manufacturer_string(hidraw_device *dev, wchar_t *string, size_t maxlen)
{
	return get_device_string(dev, DEVICE_STRING_MANUFACTURER, string, maxlen);
}

int HID_API_EXPORT_CALL hidraw_get_product_string(hidraw_device *dev, wchar_t *string, size_t maxlen)
{
	return get_device_string(dev, DEVICE_STRING_PRODUCT, string, maxlen);
}

int HID_API_EXPORT_CALL hidraw_get_serial_number_string(hidraw_device *dev, wchar_t *string, size_t maxlen)
{
	return get_device_string(dev, DEVICE_STRING_SERIAL, string, maxlen);
}

int HID_API_EXPORT_CALL hidraw_get_indexed_string(hidraw_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
	//return -1;
	return get_device_string(dev, string_index, string, maxlen);
}


HID_API_EXPORT const wchar_t * HID_API_CALL  hidraw_error(hidraw_device *dev)
{
	return NULL;
}
