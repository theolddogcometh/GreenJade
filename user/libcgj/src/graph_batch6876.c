/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6876: fsconfig FSCONFIG_SET_FD cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_set_fd_6876(void);
 *     - Returns 5 (soft FSCONFIG_SET_FD fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_set_fd_6876  (alias)
 *   __libcgj_batch6876_marker = "libcgj-batch6876"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_set_fd_6876 surface only; no
 * multi-def. Distinct from set_path_empty (batch6875) and cmd_create
 * (batch6877). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6876_marker[] = "libcgj-batch6876";

/* Soft FSCONFIG_SET_FD (set parameter, supplying an object by fd). */
#define B6876_FSCONFIG_SET_FD  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6876_set_fd(void)
{
	return B6876_FSCONFIG_SET_FD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_set_fd_6876 - soft FSCONFIG_SET_FD cmd catalog id.
 *
 * Always returns 5. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or touch superblock fds. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_set_fd_6876(void)
{
	(void)NULL;
	return b6876_set_fd();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_set_fd_6876(void)
    __attribute__((alias("gj_fsconfig_cmd_set_fd_6876")));
