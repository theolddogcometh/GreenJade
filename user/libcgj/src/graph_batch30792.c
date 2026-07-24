/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30792: libcgj product green extract (wave 30800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_30800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       30800 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_30800  (alias)
 *   __libcgj_batch30792_marker = "libcgj-batch30792"
 *
 * Milestone 30800 exclusive continuum CREATE-ONLY (30791-30800). Unique
 * gj_libcgj_green_30800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_30700 / gj_libcgj_green_30600 / gj_libcgj_green_30500 and
 * sibling 30800 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30791–30800): host soft probes
 * (cgj_soft_milestone_30800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30792. */
const char __libcgj_batch30792_marker[] = "libcgj-batch30792";

/* Libcgj green lamp for wave 30800 (soft product status: ready). */
#define B30792_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30792_green(void)
{
	return B30792_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_30800 - report product libcgj green lamp for wave 30800.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_30800(void)
{
	(void)NULL;
	return b30792_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_30800(void)
    __attribute__((alias("gj_libcgj_green_30800")));
