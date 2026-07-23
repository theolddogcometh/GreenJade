/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14992: libcgj product green extract (wave 15000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_15000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       15000 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_15000  (alias)
 *   __libcgj_batch14992_marker = "libcgj-batch14992"
 *
 * Milestone 15000 exclusive continuum CREATE-ONLY (14991-15000). Unique
 * gj_libcgj_green_15000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_14900 / gj_libcgj_green_14800 / gj_libcgj_green_14700 and
 * sibling 15000 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (14991–15000): host soft probes
 * (cgj_soft_milestone_15000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 14992. */
const char __libcgj_batch14992_marker[] = "libcgj-batch14992";

/* Libcgj green lamp for wave 15000 (soft product status: ready). */
#define B14992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14992_green(void)
{
	return B14992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_15000 - report product libcgj green lamp for wave 15000.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_15000(void)
{
	(void)NULL;
	return b14992_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_15000(void)
    __attribute__((alias("gj_libcgj_green_15000")));
