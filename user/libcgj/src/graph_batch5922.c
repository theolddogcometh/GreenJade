/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5922: shell command registry entry stride.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_reg_stride_5922(void);
 *     - Returns the soft byte stride of one shell command surface
 *       registry entry (64). Layout hint only; does not size a heap
 *       object.
 *   uint32_t __gj_shell_cmd_reg_stride_5922  (alias)
 *   __libcgj_batch5922_marker = "libcgj-batch5922"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_reg_stride_5922 surface only;
 * no multi-def. Distinct from gj_shell_cmd_reg_cap_5921. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5922_marker[] = "libcgj-batch5922";

/* Soft byte stride of one shell command surface registry entry. */
#define B5922_SHELL_CMD_REG_STRIDE  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5922_reg_stride(void)
{
	return B5922_SHELL_CMD_REG_STRIDE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_reg_stride_5922 - shell command registry entry stride.
 *
 * Always returns 64 (soft entry stride). Does not allocate and does
 * not call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_reg_stride_5922(void)
{
	(void)NULL;
	return b5922_reg_stride();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_reg_stride_5922(void)
    __attribute__((alias("gj_shell_cmd_reg_stride_5922")));
