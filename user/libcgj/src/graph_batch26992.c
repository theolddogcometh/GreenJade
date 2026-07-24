/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26992: libcgj product green extract (wave 27000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_27000(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       27000 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_27000  (alias)
 *   __libcgj_batch26992_marker = "libcgj-batch26992"
 *
 * Milestone 27000 exclusive continuum CREATE-ONLY (26991-27000). Unique
 * gj_libcgj_green_27000 surface only; no multi-def. Distinct from
 * gj_libcgj_green_26900 / gj_libcgj_green_26800 / gj_libcgj_green_26700 and
 * sibling 27000 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26991–27000): host soft probes
 * (cgj_soft_milestone_27000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26992. */
const char __libcgj_batch26992_marker[] = "libcgj-batch26992";

/* Libcgj green lamp for wave 27000 (soft product status: ready). */
#define B26992_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26992_green(void)
{
	return B26992_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_27000 - report product libcgj green lamp for wave 27000.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_27000(void)
{
	(void)NULL;
	return b26992_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_27000(void)
    __attribute__((alias("gj_libcgj_green_27000")));
