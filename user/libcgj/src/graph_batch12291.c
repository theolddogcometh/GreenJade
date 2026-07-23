/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12291: shell product green extract (wave 12300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_12300(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       12300 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_12300  (alias)
 *   __libcgj_batch12291_marker = "libcgj-batch12291"
 *
 * Milestone 12300 exclusive continuum CREATE-ONLY (12291-12300). Unique
 * gj_shell_green_12300 surface only; no multi-def. Distinct from
 * gj_shell_green_12200 (batch12191 continuum form), gj_shell_green_12100
 * (batch12091), gj_shell_green_12000 (batch11991), and gj_libcgj_green_12300
 * (batch12292). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12291_marker[] = "libcgj-batch12291";

/* Product shell green lamp for wave 12300. */
#define B12291_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12291_green(void)
{
	return B12291_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_12300 - report product shell green lamp for wave 12300.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_12300(void)
{
	(void)NULL;
	return b12291_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_12300(void)
    __attribute__((alias("gj_shell_green_12300")));
