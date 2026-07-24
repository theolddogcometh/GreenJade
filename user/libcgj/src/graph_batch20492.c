/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20492: libcgj product green extract (wave 20500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_20500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       20500 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_20500  (alias)
 *   __libcgj_batch20492_marker = "libcgj-batch20492"
 *
 * Milestone 20500 exclusive continuum CREATE-ONLY (20491-20500). Unique
 * gj_libcgj_green_20500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_20400 / gj_libcgj_green_20300 / gj_libcgj_green_20200 and
 * sibling 20500 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20491–20500): host soft probes
 * (cgj_soft_milestone_20500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20492. */
const char __libcgj_batch20492_marker[] = "libcgj-batch20492";

/* Libcgj green lamp for wave 20500 (soft product status: ready). */
#define B20492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20492_green(void)
{
	return B20492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_20500 - report product libcgj green lamp for wave 20500.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_20500(void)
{
	(void)NULL;
	return b20492_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_20500(void)
    __attribute__((alias("gj_libcgj_green_20500")));
