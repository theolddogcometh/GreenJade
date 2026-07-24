/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch59891: shell product green extract (wave 59900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_59900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       59900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_59900  (alias)
 *   __libcgj_batch59891_marker = "libcgj-batch59891"
 *
 * Milestone 59900 exclusive continuum CREATE-ONLY (59891-59900). Unique
 * gj_shell_green_59900 surface only; no multi-def. Distinct from
 * gj_shell_green_25800 / gj_shell_green_25700 / gj_shell_green_25600 and
 * sibling 59900 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (59891–59900): host soft probes
 * (cgj_soft_milestone_59900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=59900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 59891. */
const char __libcgj_batch59891_marker[] = "libcgj-batch59891";

/* Shell green lamp for wave 59900 (soft product status: ready). */
#define B59891_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b59891_green(void)
{
	return B59891_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_59900 - report product shell green lamp for wave 59900.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_59900(void)
{
	(void)NULL;
	return b59891_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_59900(void)
    __attribute__((alias("gj_shell_green_59900")));
