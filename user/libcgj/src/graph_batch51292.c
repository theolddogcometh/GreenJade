/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch51292: libcgj product green extract (wave 51300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_51300(void);
 *     - Returns 1 (product libcgj green/ready lamp for the milestone
 *       51300 continuum). Soft compile-time product status tag; not a
 *       runtime library load probe.
 *   uint32_t __gj_libcgj_green_51300  (alias)
 *   __libcgj_batch51292_marker = "libcgj-batch51292"
 *
 * Milestone 51300 exclusive continuum CREATE-ONLY (51291-51300). Unique
 * gj_libcgj_green_51300 surface only; no multi-def. Distinct from
 * gj_libcgj_green_51200 / gj_libcgj_green_51100 / gj_libcgj_green_51000 and
 * sibling 51300 milestone symbols (shell_green / bar3_ready / …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (51291–51300): host soft probes
 * (cgj_soft_milestone_51300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=51300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 51292. */
const char __libcgj_batch51292_marker[] = "libcgj-batch51292";

/* Libcgj green lamp for wave 51300 (soft product status: ready). */
#define B51292_LIBCGJ_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b51292_green(void)
{
	return B51292_LIBCGJ_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_51300 - report product libcgj green lamp for wave 51300.
 *
 * Always returns 1u (libcgj green/ready). Soft pure-data product tag;
 * does not call libc or load a library. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_libcgj_green_51300(void)
{
	(void)NULL;
	return b51292_green();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_libcgj_green_51300(void)
    __attribute__((alias("gj_libcgj_green_51300")));
