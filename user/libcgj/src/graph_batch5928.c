/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5928: shell command surface registry ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_reg_ready_5928(void);
 *     - Returns 1 (shell command surface registry ready). Soft product
 *       status tag for the registry continuum; not a runtime probe of
 *       greenjade-shell.
 *   uint32_t __gj_shell_cmd_reg_ready_5928  (alias)
 *   __libcgj_batch5928_marker = "libcgj-batch5928"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_reg_ready_5928 surface only; no
 * multi-def. Distinct from gj_shell_green_5900 (batch5895). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5928_marker[] = "libcgj-batch5928";

/* Shell command surface registry ready lamp. */
#define B5928_SHELL_CMD_REG_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5928_reg_ready(void)
{
	return B5928_SHELL_CMD_REG_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_reg_ready_5928 - shell command surface registry ready.
 *
 * Always returns 1 (registry ready). Does not probe greenjade-shell
 * and does not call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_reg_ready_5928(void)
{
	(void)NULL;
	return b5928_reg_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_reg_ready_5928(void)
    __attribute__((alias("gj_shell_cmd_reg_ready_5928")));
