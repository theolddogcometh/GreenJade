/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15292: libcgj product green extract (wave 15300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_15300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       15300 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_15300  (alias)
 *   __libcgj_batch15292_marker = "libcgj-batch15292"
 *
 * Milestone 15300 exclusive continuum CREATE-ONLY (15291-15300). Unique
 * gj_libcgj_green_15300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_15200 / gj_libcgj_green_15100 / gj_libcgj_green_15000 and
 * sibling 15300 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15291–15300): host soft probes
 * (cgj_soft_milestone_15300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15292. */
const char __libcgj_batch15292_marker[] = "libcgj-batch15292";

/* Libcgj green lamp for wave 15300 (soft product status: ready). */
#define B15292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15292_green(void)
{
	return B15292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_15300 - report product libcgj green lamp for wave 15300.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_15300(void)
{
	(void)NULL;
	return b15292_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_15300(void)
    __attribute__((alias("gj_libcgj_green_15300")));
