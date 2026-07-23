/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6875: fsconfig FSCONFIG_SET_PATH_EMPTY cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_set_path_empty_6875(void);
 *     - Returns 4 (soft FSCONFIG_SET_PATH_EMPTY fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_set_path_empty_6875  (alias)
 *   __libcgj_batch6875_marker = "libcgj-batch6875"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_set_path_empty_6875 surface only; no
 * multi-def. Distinct from set_path (batch6874) and set_fd
 * (batch6876). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6875_marker[] = "libcgj-batch6875";

/* Soft FSCONFIG_SET_PATH_EMPTY (object by empty path). */
#define B6875_FSCONFIG_SET_PATH_EMPTY  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6875_set_path_empty(void)
{
	return B6875_FSCONFIG_SET_PATH_EMPTY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_set_path_empty_6875 - soft FSCONFIG_SET_PATH_EMPTY.
 *
 * Always returns 4. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or touch superblock fds. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_set_path_empty_6875(void)
{
	(void)NULL;
	return b6875_set_path_empty();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_set_path_empty_6875(void)
    __attribute__((alias("gj_fsconfig_cmd_set_path_empty_6875")));
