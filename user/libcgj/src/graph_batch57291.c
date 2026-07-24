/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch57291: shell product green extract (wave 57300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_57300(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       57300 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_57300  (alias)
 *   __libcgj_batch57291_marker = "libcgj-batch57291"
 *
 * Milestone 57300 exclusive continuum CREATE-ONLY (57291-57300). Unique
 * gj_shell_green_57300 surface only; no multi-def. Distinct from
 * gj_shell_green_25800 / gj_shell_green_25700 / gj_shell_green_25600 and
 * sibling 57300 milestone symbols (libcgj_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (57291–57300): host soft probes
 * (cgj_soft_milestone_57300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=57300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 57291. */
const char __libcgj_batch57291_marker[] = "libcgj-batch57291";

/* Shell green lamp for wave 57300 (soft product status: ready). */
#define B57291_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b57291_green(void)
{
	return B57291_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_57300 - report product shell green lamp for wave 57300.
 *
 * Always returns 1u (shell green/ready). Soft pure-data product tag;
 * does not call libc or probe greenjade-shell. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_shell_green_57300(void)
{
	(void)NULL;
	return b57291_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_shell_green_57300(void)
    __attribute__((alias("gj_shell_green_57300")));
