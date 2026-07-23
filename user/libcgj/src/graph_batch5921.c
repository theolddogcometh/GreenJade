/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5921: shell command surface registry capacity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_reg_cap_5921(void);
 *     - Returns the soft max slot capacity for the product shell command
 *       surface registry (256). Compile-time catalog bound only; does
 *       not allocate a table.
 *   uint32_t __gj_shell_cmd_reg_cap_5921  (alias)
 *   __libcgj_batch5921_marker = "libcgj-batch5921"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_reg_cap_5921 surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5921_marker[] = "libcgj-batch5921";

/* Soft max slots in the shell command surface registry. */
#define B5921_SHELL_CMD_REG_CAP  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5921_reg_cap(void)
{
	return B5921_SHELL_CMD_REG_CAP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_reg_cap_5921 - shell command surface registry capacity.
 *
 * Always returns 256 (soft catalog bound). Does not allocate registry
 * storage and does not call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_reg_cap_5921(void)
{
	(void)NULL;
	return b5921_reg_cap();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_reg_cap_5921(void)
    __attribute__((alias("gj_shell_cmd_reg_cap_5921")));
