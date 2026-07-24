/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37998: milestone 38000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_38000(void);
 *     - Returns the milestone tag for the milestone 38000 continuum
 *       (always 38000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_38000  (alias)
 *   __libcgj_batch37998_marker = "libcgj-batch37998"
 *
 * Milestone 38000 exclusive continuum CREATE-ONLY (37991-38000). Unique
 * gj_milestone_tag_38000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_37900 / gj_milestone_tag_37800 / gj_milestone_tag_37700,
 * gj_graph_milestone_38000 (batch38000), and gj_continuum_wave_38000
 * (batch37999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (37991–38000): host soft probes
 * (cgj_soft_milestone_38000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=38000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 37998. */
const char __libcgj_batch37998_marker[] = "libcgj-batch37998";

/* Soft milestone tag for wave 38000 (product identity = 38000). */
#define B37998_MILESTONE_TAG  38000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37998_tag(void)
{
	return B37998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_38000 - report soft milestone product tag for wave 38000.
 *
 * Always returns 38000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 38000.
 */
uint32_t
gj_milestone_tag_38000(void)
{
	(void)NULL;
	return b37998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_38000(void)
    __attribute__((alias("gj_milestone_tag_38000")));
