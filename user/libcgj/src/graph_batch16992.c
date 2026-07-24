/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16992: libcgj product green extract (wave 17000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_17000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       17000 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_17000  (alias)
 *   __libcgj_batch16992_marker = "libcgj-batch16992"
 *
 * Milestone 17000 exclusive continuum CREATE-ONLY (16991-17000). Unique
 * gj_libcgj_green_17000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_16900 / gj_libcgj_green_16800 / gj_libcgj_green_16700 and
 * sibling 17000 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16991–17000): host soft probes
 * (cgj_soft_milestone_17000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=17000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16992. */
const char __libcgj_batch16992_marker[] = "libcgj-batch16992";

/* Libcgj green lamp for wave 17000 (soft product status: ready). */
#define B16992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16992_green(void)
{
	return B16992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_17000 - report product libcgj green lamp for wave 17000.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_17000(void)
{
	(void)NULL;
	return b16992_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_17000(void)
    __attribute__((alias("gj_libcgj_green_17000")));
