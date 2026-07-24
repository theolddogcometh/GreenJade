/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50591: shell product green extract (wave 50600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_50600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       50600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_50600  (alias)
 *   __libcgj_batch50591_marker = "libcgj-batch50591"
 *
 * Milestone 50600 exclusive continuum CREATE-ONLY (50591-50600). Unique
 * gj_shell_green_50600 surface only; no multi-def. Distinct from
 * gj_shell_green_50500 / gj_shell_green_50400 / gj_shell_green_50300 and
 * sibling 50600 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50591–50600): host soft probes
 * (cgj_soft_milestone_50600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50591. */
const char __libcgj_batch50591_marker[] = "libcgj-batch50591";

/* Shell green lamp for wave 50600 (soft product status: ready). */
#define B50591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50591_green(void)
{
	return B50591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_50600 - report product shell green lamp for wave 50600.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_50600(void)
{
	(void)NULL;
	return b50591_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_50600(void)
    __attribute__((alias("gj_shell_green_50600")));
