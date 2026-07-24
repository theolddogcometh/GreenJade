/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16591: shell product green extract (wave 16600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_16600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       16600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_16600  (alias)
 *   __libcgj_batch16591_marker = "libcgj-batch16591"
 *
 * Milestone 16600 exclusive continuum CREATE-ONLY (16591-16600). Unique
 * gj_shell_green_16600 surface only; no multi-def. Distinct from
 * gj_shell_green_16500 / gj_shell_green_16400 / gj_shell_green_16300 and
 * sibling 16600 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16591–16600): host soft probes
 * (cgj_soft_milestone_16600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16591. */
const char __libcgj_batch16591_marker[] = "libcgj-batch16591";

/* Shell green lamp for wave 16600 (soft product status: ready). */
#define B16591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16591_green(void)
{
	return B16591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_16600 - report product shell green lamp for wave 16600.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_16600(void)
{
	(void)NULL;
	return b16591_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_16600(void)
    __attribute__((alias("gj_shell_green_16600")));
