/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25492: libcgj product green extract (wave 25500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_25500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       25500 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_25500  (alias)
 *   __libcgj_batch25492_marker = "libcgj-batch25492"
 *
 * Milestone 25500 exclusive continuum CREATE-ONLY (25491-25500). Unique
 * gj_libcgj_green_25500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_25400 / gj_libcgj_green_25300 / gj_libcgj_green_25200 and
 * sibling 25500 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25491–25500): host soft probes
 * (cgj_soft_milestone_25500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=25500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25492. */
const char __libcgj_batch25492_marker[] = "libcgj-batch25492";

/* Libcgj green lamp for wave 25500 (soft product status: ready). */
#define B25492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25492_green(void)
{
	return B25492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_25500 - report product libcgj green lamp for wave 25500.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_25500(void)
{
	(void)NULL;
	return b25492_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_25500(void)
    __attribute__((alias("gj_libcgj_green_25500")));
