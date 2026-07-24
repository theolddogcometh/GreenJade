/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40591: shell product green extract (wave 40600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_40600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       40600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_40600  (alias)
 *   __libcgj_batch40591_marker = "libcgj-batch40591"
 *
 * Milestone 40600 exclusive continuum CREATE-ONLY (40591-40600). Unique
 * gj_shell_green_40600 surface only; no multi-def. Distinct from
 * gj_shell_green_40500 / gj_shell_green_40400 / gj_shell_green_40300 and
 * sibling 40600 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40591–40600): host soft probes
 * (cgj_soft_milestone_40600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40591. */
const char __libcgj_batch40591_marker[] = "libcgj-batch40591";

/* Shell green lamp for wave 40600 (soft product status: ready). */
#define B40591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40591_green(void)
{
	return B40591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_40600 - report product shell green lamp for wave 40600.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_40600(void)
{
	(void)NULL;
	return b40591_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_40600(void)
    __attribute__((alias("gj_shell_green_40600")));
