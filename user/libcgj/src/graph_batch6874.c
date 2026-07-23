/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6874: fsconfig FSCONFIG_SET_PATH cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_set_path_6874(void);
 *     - Returns 3 (soft FSCONFIG_SET_PATH fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_set_path_6874  (alias)
 *   __libcgj_batch6874_marker = "libcgj-batch6874"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_set_path_6874 surface only; no
 * multi-def. Distinct from set_binary (batch6873) and set_path_empty
 * (batch6875). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6874_marker[] = "libcgj-batch6874";

/* Soft FSCONFIG_SET_PATH (set parameter, supplying an object by path). */
#define B6874_FSCONFIG_SET_PATH  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6874_set_path(void)
{
	return B6874_FSCONFIG_SET_PATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_set_path_6874 - soft FSCONFIG_SET_PATH cmd id.
 *
 * Always returns 3. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or touch superblock fds. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_set_path_6874(void)
{
	(void)NULL;
	return b6874_set_path();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_set_path_6874(void)
    __attribute__((alias("gj_fsconfig_cmd_set_path_6874")));
