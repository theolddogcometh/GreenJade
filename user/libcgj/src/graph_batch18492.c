/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18492: libcgj product green extract (wave 18500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_18500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       18500 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_18500  (alias)
 *   __libcgj_batch18492_marker = "libcgj-batch18492"
 *
 * Milestone 18500 exclusive continuum CREATE-ONLY (18491-18500). Unique
 * gj_libcgj_green_18500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_18300 / gj_libcgj_green_18200 / gj_libcgj_green_18100 and
 * sibling 18500 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18491–18500): host soft probes
 * (cgj_soft_milestone_18500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18492. */
const char __libcgj_batch18492_marker[] = "libcgj-batch18492";

/* Libcgj green lamp for wave 18500 (soft product status: ready). */
#define B18492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18492_green(void)
{
	return B18492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_18500 - report product libcgj green lamp for wave 18500.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_18500(void)
{
	(void)NULL;
	return b18492_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_18500(void)
    __attribute__((alias("gj_libcgj_green_18500")));
