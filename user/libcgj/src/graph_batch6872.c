/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6872: fsconfig FSCONFIG_SET_STRING cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsconfig_cmd_set_string_6872(void);
 *     - Returns 1 (soft FSCONFIG_SET_STRING fsconfig(2) command id).
 *       Pure-data catalog stub; not a live fsopen/fsconfig syscall.
 *   uint32_t __gj_fsconfig_cmd_set_string_6872  (alias)
 *   __libcgj_batch6872_marker = "libcgj-batch6872"
 *
 * Exclusive continuum CREATE-ONLY (6871-6880: fsopen/fsconfig cmd
 * stubs). Unique gj_fsconfig_cmd_set_string_6872 surface only; no
 * multi-def. Distinct from set_flag (batch6871) and set_binary
 * (batch6873). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6872_marker[] = "libcgj-batch6872";

/* Soft FSCONFIG_SET_STRING (set parameter, supplying a string value). */
#define B6872_FSCONFIG_SET_STRING  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6872_set_string(void)
{
	return B6872_FSCONFIG_SET_STRING;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsconfig_cmd_set_string_6872 - soft FSCONFIG_SET_STRING cmd id.
 *
 * Always returns 1. Pure-data stub for product fsconfig-cmd catalogs;
 * does not issue fsopen/fsconfig or touch superblock fds. No parent
 * wires.
 */
uint32_t
gj_fsconfig_cmd_set_string_6872(void)
{
	(void)NULL;
	return b6872_set_string();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsconfig_cmd_set_string_6872(void)
    __attribute__((alias("gj_fsconfig_cmd_set_string_6872")));
