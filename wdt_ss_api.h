#ifndef __WDT_SS_API_H__
#define __WDT_SS_API_H__
 #include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief        Retrieves the current version of the Wedia Touch firmware API.
 * \details      This function returns the version number of the firmware API 
 *               supported by the Wedia Touch controller. The version is typically 
 *               encoded as an integer (e.g., 0x010203 for version 1.2.3).
 * \return       The firmware API version number.
 */
__attribute__((visibility("default")))
int wdt_ss_get_api_version(void);

/**
 * \brief        Updates the Wedia Touch controller firmware.
 * \details      This function loads the firmware binary from the specified file path and
 *               updates the firmware. It performs validation of the firmware and applies
 *               the update if valid.
 * \param path   Pointer to a null-terminated string containing the path to the firmware file.
 * \return       Zero on success, non-zero error code on failure.
 */
__attribute__((visibility("default")))
int wdt_ss_firmware_update(char* path);
/**
 * \brief        Verifies the Wedia Touch controller firmware.
 * \details      This function loads the firmware binary from the specified file path and
 *               performs validation checks to ensure the firmware integrity and compatibility.
 *               It does not apply the firmware update but confirms whether the firmware is valid.
 * \param path   Pointer to a null-terminated string containing the path to the firmware file.
 * \return       Zero if the firmware is valid, non-zero error code if verification fails.
 */
__attribute__((visibility("default")))
int wdt_ss_firmware_verification(char* path);
/**
 * \brief        Retrieves the current firmware version of the Wedia Touch controller.
 * \details      The version is returned as a 32-bit encoded integer, where the
 *               higher bytes typically represent the major, minor, and patch versions.
 *               The exact encoding scheme should be consistent with the API's versioning format.
 * \param[out] version Pointer to a 32-bit unsigned integer where the encoded firmware version will be stored.
 * \return       Zero on success, non-zero error code on failure.
 */
__attribute__((visibility("default")))
int wdt_ss_get_current_firmware_version(uint32_t* version);
/**
 * \brief        Retrieves the name of the connected Wedia Touch device.
 * \details      This function fills the provided buffer with the name of the connected device.
 *               The device name is returned as a null-terminated string. The function writes at most
 *               \p max_len bytes (including the null terminator) to the buffer.
 *               If the device name is longer than \p max_len - 1, it will be truncated.
 *
 * \param[out]   name     Pointer to a character buffer where the device name will be stored.
 * \param[in]    max_len  The maximum number of bytes that can be written to the buffer,
 *                        including the null terminator.
 *
 * \return       Zero on success, non-zero error code on failure.
 */
__attribute__((visibility("default")))
int wdt_ss_get_device_name(char* name, size_t max_len);

/**
 * \brief        Retrieves the Vendor ID (VID) and Product ID (PID) of the connected Wedia Touch device.
 * \details      This function fills the provided pointers with the USB Vendor ID and Product ID
 *               of the currently connected device. These identifiers are commonly used to distinguish
 *               between different USB devices.
 *
 * \param[out]   vid  Pointer to a 32-bit unsigned integer where the Vendor ID will be stored.
 * \param[out]   pid  Pointer to a 32-bit unsigned integer where the Product ID will be stored.
 *
 * \return       Zero on success, non-zero error code on failure.
 */

__attribute__((visibility("default")))
int wdt_ss_get_vid_pid(uint32_t* vid, uint32_t* pid);


#ifdef __cplusplus
}
#endif

#endif

