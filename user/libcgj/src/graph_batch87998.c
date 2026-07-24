/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87998: milestone 88000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_88000(void);
 *     - Returns the milestone tag for the milestone 88000 continuum
 *       (always 88000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_88000  (alias)
 *   __libcgj_batch87998_marker = "libcgj-batch87998"
 *
 * Milestone 88000 exclusive continuum CREATE-ONLY (25891-88000). Unique
 * gj_milestone_tag_88000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_88000 (batch88000), and gj_continuum_wave_88000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–88000): host soft probes
 * (cgj_soft_milestone_88000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=88000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87998. */
const char __libcgj_batch87998_marker[] = "libcgj-batch87998";

/* Soft milestone tag for wave 88000 (product identity = 88000). */
#define B87998_MILESTONE_TAG  88000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87998_tag(void)
{
	return B87998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_88000 - report soft milestone product tag for wave 88000.
 *
 * Always returns 88000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 88000.
 */
uint32_t
gj_milestone_tag_88000(void)
{
	(void)NULL;
	return b87998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_88000(void)
    __attribute__((alias("gj_milestone_tag_88000")));
