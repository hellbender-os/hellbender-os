#ifndef _KERNEL_IDC_H
#define _KERNEL_IDC_H

// inter domain calls.

/**
 * NOTE: At the moment, all IDC declarations has to be on a single line!
 *
 * All inter-domain callable interfaces must have the following declaration:
 *
 * __IDC__ r_type mysrv_myfunc(IDC_PTR, p_type p1, etc);
 *
 * where "r_type" is your return value type, and
 * "p_type p1, etc" are your input parameters.
 *
 * All IDC implementations must have the following declaration:
 *
 * __IDCIMPL__ r_type somesrv_somefunc(IDC_PTR, p_type p1, etc);
 */
#define __IDC__ extern
#define IDC_PTR __attribute__((unused))uintptr_t __idc_ptr
#define __IDCIMPL__

/**
 * makes a pointer that you use to call the implementation.
 *
 * first, create and store the pointer e.g. as:
 *   coresrv_vfs_read_ptr read_ptr =
 *     MAKE_IDC_PTR(coresrv_vfs_read, coresrv_fat_read);
 *
 * later, call the implementation:
 *   IDC(coresrv_vfs_read, read_ptr, data_buffer, buffer_size);
 */
#define MAKE_IDC_PTR(IFACE, IMPL) ((IFACE ## _ptr) &__IDE__ ## IMPL)

/**
 * calls a specific implementation of an IDC interface, see MAKE_IDC_PTR.
 */
#define IDC(IFACE, PTR, ...) __IDC__ ## IFACE (PTR, ## __VA_ARGS__)

// Use this instead of IDC pointer when making in-domain calls.
#define NO_IDC ((uintptr_t)0)

#endif
