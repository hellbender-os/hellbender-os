#ifndef __HELLBENDER_KERNEL_SERVICE_H__
#define __HELLBENDER_KERNEL_SERVICE_H__

#include "config.h"
#include "process.h"
#include <stdint.h>

struct service;

/* Service calls allows a fast calls from user space process to
 * trusted system services. When executing a service call, the
 * service has memory access to the usermode process. This way
 * parameters and results can be read/written without copying.
 */

void service_init();

/* Allocates a new service virtual memory area and modifies the
 * process descriptor so that the process will be created in the
 * allocated virtual memory. The binary MUST be PIC!
 * Returns the virtual memory base address.
 */
uintptr_t service_relocate(struct process_descriptor* pd);

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
