/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9291: shell product green extract (wave 9300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9300(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9300 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9300  (alias)
 *   __libcgj_batch9291_marker = "libcgj-batch9291"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_shell_green_9300 surface only; no multi-def. Distinct from
 * gj_shell_green_9200 (batch9191), gj_shell_green_9100 (batch9091),
 * gj_shell_green_9000 (batch8991), and gj_libcgj_green_9300
 * (batch9292). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9291_marker[] = "libcgj-batch9291";

/* Product shell green lamp for wave 9300. */
#define B9291_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9291_green(void)
{
	return B9291_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9300 - report product shell green lamp for wave 9300.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9300(void)
{
	(void)NULL;
	return b9291_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9300(void)
    __attribute__((alias("gj_shell_green_9300")));
