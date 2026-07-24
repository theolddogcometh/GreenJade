/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41998: milestone 42000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_42000(void);
 *     - Returns the milestone tag for the milestone 42000 continuum
 *       (always 42000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_42000  (alias)
 *   __libcgj_batch41998_marker = "libcgj-batch41998"
 *
 * Milestone 42000 exclusive continuum CREATE-ONLY (41991-42000). Unique
 * gj_milestone_tag_42000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_41900 / gj_milestone_tag_41800 / gj_milestone_tag_41700,
 * gj_graph_milestone_42000 (batch42000), and gj_continuum_wave_42000
 * (batch41999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (41991–42000): host soft probes
 * (cgj_soft_milestone_42000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 41998. */
const char __libcgj_batch41998_marker[] = "libcgj-batch41998";

/* Soft milestone tag for wave 42000 (product identity = 42000). */
#define B41998_MILESTONE_TAG  42000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b41998_tag(void)
{
	return B41998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_42000 - report soft milestone product tag for wave 42000.
 *
 * Always returns 42000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 42000.
 */
uint32_t
gj_milestone_tag_42000(void)
{
	(void)NULL;
	return b41998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_42000(void)
    __attribute__((alias("gj_milestone_tag_42000")));
