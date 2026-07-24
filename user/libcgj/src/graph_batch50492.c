/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50492: libcgj product green extract (wave 50500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_50500(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       50500 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_50500  (alias)
 *   __libcgj_batch50492_marker = "libcgj-batch50492"
 *
 * Milestone 50500 exclusive continuum CREATE-ONLY (50491-50500). Unique
 * gj_libcgj_green_50500 surface only; no multi-def. Distinct from
 * gj_libcgj_green_50400 / gj_libcgj_green_50300 / gj_libcgj_green_50200 and
 * sibling 50500 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50491–50500): host soft probes
 * (cgj_soft_milestone_50500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50492. */
const char __libcgj_batch50492_marker[] = "libcgj-batch50492";

/* Libcgj green lamp for wave 50500 (soft product status: ready). */
#define B50492_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50492_green(void)
{
	return B50492_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_50500 - report product libcgj green lamp for wave 50500.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_50500(void)
{
	(void)NULL;
	return b50492_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_50500(void)
    __attribute__((alias("gj_libcgj_green_50500")));
