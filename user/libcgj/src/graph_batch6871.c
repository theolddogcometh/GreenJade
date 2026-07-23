/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6871: fsconfig FSCONFIG_SET_FLAG cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_set_flag_6871(void);
 *     - Returns 0 (soft FSCONFIG_SET_FLAG fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_set_flag_6871  (alias)
 *   __libcgj_batch6871_marker = "libcgj-batch6871"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs — set_flag_6871, set_string_6872, set_binary_6873,
 * set_path_6874, set_path_empty_6875, set_fd_6876, cmd_create_6877,
 * cmd_reconfigure_6878, cmd_ok_6879, continuum + batch_id_6880).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6871_marker[] = "libcgj-batch6871";

/* Soft FSCONFIG_SET_FLAG (set parameter, supplying no value). */
#define B6871_FSCONFIG_SET_FLAG  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6871_set_flag(void)
{
	return B6871_FSCONFIG_SET_FLAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_set_flag_6871 - soft FSCONFIG_SET_FLAG cmd catalog id.
 *
 * Always returns 0. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or touch superblock fds. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_set_flag_6871(void)
{
	(void)NULL;
	return b6871_set_flag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_set_flag_6871(void)
    __attribute__((alias("gj_fsconfig_cmd_set_flag_6871")));
