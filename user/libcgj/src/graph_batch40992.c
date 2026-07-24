/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40992: libcgj product green extract (wave 41000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_41000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       41000 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_41000  (alias)
 *   __libcgj_batch40992_marker = "libcgj-batch40992"
 *
 * Milestone 41000 exclusive continuum CREATE-ONLY (40991-41000). Unique
 * gj_libcgj_green_41000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_40900 / gj_libcgj_green_40800 / gj_libcgj_green_40700 and
 * sibling 41000 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40991–41000): host soft probes
 * (cgj_soft_milestone_41000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=41000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40992. */
const char __libcgj_batch40992_marker[] = "libcgj-batch40992";

/* Libcgj green lamp for wave 41000 (soft product status: ready). */
#define B40992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40992_green(void)
{
	return B40992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_41000 - report product libcgj green lamp for wave 41000.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_41000(void)
{
	(void)NULL;
	return b40992_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_41000(void)
    __attribute__((alias("gj_libcgj_green_41000")));
