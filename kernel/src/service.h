#ifndef __HELLBENDER_KERNEL_SERVICE_H__
#define __HELLBENDER_KERNEL_SERVICE_H__

#include "config.h"
#include <stdint.h>

struct service;

#define SERVICE_TABLE_BASE 0x1000
#define SERVICE_TABLE_SIZE 0x1000

/* Service calls allows a fast calls from user space process to
 * trusted system services. When executing a service call, the
 * service has memory access to the usermode process. This way
 * parameters and results can be read/written without copying.
 */

/* Adds the target address to the IDC table so that usermode processes can
 * call the function. The function is called using the returned value as
 * a pointer to function. Zero is returned if registration fails.
 */
uint64_t service_register(uint64_t target);

/* Modifies the thread state so that it will enter the service function
 * when returning to the usermode.
 */
void service_call(uint64_t handle);
void service_return();

#endif
