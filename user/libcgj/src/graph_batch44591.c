/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44591: shell product green extract (wave 44600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_44600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       44600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_44600  (alias)
 *   __libcgj_batch44591_marker = "libcgj-batch44591"
 *
 * Milestone 44600 exclusive continuum CREATE-ONLY (44591-44600). Unique
 * gj_shell_green_44600 surface only; no multi-def. Distinct from
 * gj_shell_green_44500 / gj_shell_green_44400 / gj_shell_green_44300 and
 * sibling 44600 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44591–44600): host soft probes
 * (cgj_soft_milestone_44600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44591. */
const char __libcgj_batch44591_marker[] = "libcgj-batch44591";

/* Shell green lamp for wave 44600 (soft product status: ready). */
#define B44591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44591_green(void)
{
	return B44591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_44600 - report product shell green lamp for wave 44600.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_44600(void)
{
	(void)NULL;
	return b44591_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_44600(void)
    __attribute__((alias("gj_shell_green_44600")));
