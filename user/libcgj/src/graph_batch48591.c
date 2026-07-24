/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48591: shell product green extract (wave 48600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_48600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       48600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_48600  (alias)
 *   __libcgj_batch48591_marker = "libcgj-batch48591"
 *
 * Milestone 48600 exclusive continuum CREATE-ONLY (48591-48600). Unique
 * gj_shell_green_48600 surface only; no multi-def. Distinct from
 * gj_shell_green_48500 / gj_shell_green_48400 / gj_shell_green_48300 and
 * sibling 48600 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48591–48600): host soft probes
 * (cgj_soft_milestone_48600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=48600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48591. */
const char __libcgj_batch48591_marker[] = "libcgj-batch48591";

/* Shell green lamp for wave 48600 (soft product status: ready). */
#define B48591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48591_green(void)
{
	return B48591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_48600 - report product shell green lamp for wave 48600.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_48600(void)
{
	(void)NULL;
	return b48591_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_48600(void)
    __attribute__((alias("gj_shell_green_48600")));
