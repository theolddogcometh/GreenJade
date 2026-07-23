/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4145: shell product green status (wave 4150).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4150(void);
 *     - Returns the product shell green/ready status flag for the
 *       milestone 4150 continuum (always 1 = green). Soft compile-time
 *       product status lamp; not a runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4150  (alias)
 *   __libcgj_batch4145_marker = "libcgj-batch4145"
 *
 * Milestone 4150 exclusive continuum CREATE-ONLY (4141-4150). Unique
 * gj_shell_green_4150 surface only; no multi-def. Distinct from
 * gj_shell_green_4100 (batch4095), gj_shell_green_4050 (batch4045),
 * and gj_libcgj_green_4150 (batch4146). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4145_marker[] = "libcgj-batch4145";

/* Product shell green status for wave 4150 (1 = green). */
#define B4145_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4145_green(void)
{
	return B4145_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4150 - report product shell green status for wave 4150.
 *
 * Always returns 1 (shell green for milestone 4150 continuum). Does
 * not call libc or probe greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4150(void)
{
	(void)NULL;
	return b4145_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4150(void)
    __attribute__((alias("gj_shell_green_4150")));
