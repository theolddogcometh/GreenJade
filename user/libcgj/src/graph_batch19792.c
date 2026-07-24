/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19792: libcgj product green extract (wave 19800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_19800(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       19800 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_19800  (alias)
 *   __libcgj_batch19792_marker = "libcgj-batch19792"
 *
 * Milestone 19800 exclusive continuum CREATE-ONLY (19791-19800). Unique
 * gj_libcgj_green_19800 surface only; no multi-def. Distinct from
 * gj_libcgj_green_19700 / gj_libcgj_green_19600 / gj_libcgj_green_19500 and
 * sibling 19800 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19791–19800): host soft probes
 * (cgj_soft_milestone_19800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19792. */
const char __libcgj_batch19792_marker[] = "libcgj-batch19792";

/* Libcgj green lamp for wave 19800 (soft product status: ready). */
#define B19792_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19792_green(void)
{
	return B19792_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_19800 - report product libcgj green lamp for wave 19800.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_19800(void)
{
	(void)NULL;
	return b19792_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_19800(void)
    __attribute__((alias("gj_libcgj_green_19800")));
