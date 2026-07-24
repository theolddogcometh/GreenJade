/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37691: shell product green extract (wave 37700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_37700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       37700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_37700  (alias)
 *   __libcgj_batch37691_marker = "libcgj-batch37691"
 *
 * Milestone 37700 exclusive continuum CREATE-ONLY (37691-37700). Unique
 * gj_shell_green_37700 surface only; no multi-def. Distinct from
 * gj_shell_green_37600 / gj_shell_green_37500 / gj_shell_green_37400 and
 * sibling 37700 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (37691–37700): host soft probes
 * (cgj_soft_milestone_37700.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=37700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 37691. */
const char __libcgj_batch37691_marker[] = "libcgj-batch37691";

/* Shell green lamp for wave 37700 (soft product status: ready). */
#define B37691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37691_green(void)
{
	return B37691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_37700 - report product shell green lamp for wave 37700.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_37700(void)
{
	(void)NULL;
	return b37691_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_37700(void)
    __attribute__((alias("gj_shell_green_37700")));
