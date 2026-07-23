/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6873: fsconfig FSCONFIG_SET_BINARY cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_set_binary_6873(void);
 *     - Returns 2 (soft FSCONFIG_SET_BINARY fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_set_binary_6873  (alias)
 *   __libcgj_batch6873_marker = "libcgj-batch6873"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_set_binary_6873 surface only; no
 * multi-def. Distinct from set_string (batch6872) and set_path
 * (batch6874). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6873_marker[] = "libcgj-batch6873";

/* Soft FSCONFIG_SET_BINARY (set parameter, supplying a binary blob). */
#define B6873_FSCONFIG_SET_BINARY  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6873_set_binary(void)
{
	return B6873_FSCONFIG_SET_BINARY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_set_binary_6873 - soft FSCONFIG_SET_BINARY cmd id.
 *
 * Always returns 2. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or touch superblock fds. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_set_binary_6873(void)
{
	(void)NULL;
	return b6873_set_binary();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_set_binary_6873(void)
    __attribute__((alias("gj_fsconfig_cmd_set_binary_6873")));
