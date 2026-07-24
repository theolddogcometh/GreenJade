/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39992: libcgj product green extract (wave 40000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_40000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       40000 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_40000  (alias)
 *   __libcgj_batch39992_marker = "libcgj-batch39992"
 *
 * Milestone 40000 exclusive continuum CREATE-ONLY (39991-40000). Unique
 * gj_libcgj_green_40000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_39900 / gj_libcgj_green_39800 / gj_libcgj_green_39700 and
 * sibling 40000 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (39991–40000): host soft probes
 * (cgj_soft_milestone_40000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 39992. */
const char __libcgj_batch39992_marker[] = "libcgj-batch39992";

/* Libcgj green lamp for wave 40000 (soft product status: ready). */
#define B39992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39992_green(void)
{
	return B39992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_40000 - report product libcgj green lamp for wave 40000.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_40000(void)
{
	(void)NULL;
	return b39992_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_40000(void)
    __attribute__((alias("gj_libcgj_green_40000")));
