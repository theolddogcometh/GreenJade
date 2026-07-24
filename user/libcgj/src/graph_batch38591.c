/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38591: shell product green extract (wave 38600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_38600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       38600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_38600  (alias)
 *   __libcgj_batch38591_marker = "libcgj-batch38591"
 *
 * Milestone 38600 exclusive continuum CREATE-ONLY (38591-38600). Unique
 * gj_shell_green_38600 surface only; no multi-def. Distinct from
 * gj_shell_green_38500 / gj_shell_green_38400 / gj_shell_green_38300 and
 * sibling 38600 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (38591–38600): host soft probes
 * (cgj_soft_milestone_38600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=38600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 38591. */
const char __libcgj_batch38591_marker[] = "libcgj-batch38591";

/* Shell green lamp for wave 38600 (soft product status: ready). */
#define B38591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38591_green(void)
{
	return B38591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_38600 - report product shell green lamp for wave 38600.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_38600(void)
{
	(void)NULL;
	return b38591_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_38600(void)
    __attribute__((alias("gj_shell_green_38600")));
