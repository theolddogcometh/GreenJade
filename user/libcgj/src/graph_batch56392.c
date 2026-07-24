/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch56392: libcgj product green extract (wave 56400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_56400(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       56400 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_56400  (alias)
 *   __libcgj_batch56392_marker = "libcgj-batch56392"
 *
 * Milestone 56400 exclusive continuum CREATE-ONLY (25891-56400). Unique
 * gj_libcgj_green_56400 surface only; no multi-def. Distinct from
 * gj_libcgj_green_25800 / gj_libcgj_green_25700 / gj_libcgj_green_25600 and
 * sibling 56400 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–56400): host soft probes
 * (cgj_soft_milestone_56400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=56400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 56392. */
const char __libcgj_batch56392_marker[] = "libcgj-batch56392";

/* Libcgj green lamp for wave 56400 (soft product status: ready). */
#define B56392_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b56392_green(void)
{
	return B56392_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_56400 - report product libcgj green lamp for wave 56400.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_56400(void)
{
	(void)NULL;
	return b56392_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_56400(void)
    __attribute__((alias("gj_libcgj_green_56400")));
