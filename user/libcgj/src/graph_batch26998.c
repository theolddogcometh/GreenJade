/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26998: milestone 27000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_27000(void);
 *     - Returns the milestone tag for the milestone 27000 continuum
 *       (always 27000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_27000  (alias)
 *   __libcgj_batch26998_marker = "libcgj-batch26998"
 *
 * Milestone 27000 exclusive continuum CREATE-ONLY (26991-27000). Unique
 * gj_milestone_tag_27000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_26900 / gj_milestone_tag_26800 / gj_milestone_tag_26700,
 * gj_graph_milestone_27000 (batch27000), and gj_continuum_wave_27000
 * (batch26999).
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

/* CGJ TU marker: greppable continuum identity string for batch 26998. */
const char __libcgj_batch26998_marker[] = "libcgj-batch26998";

/* Soft milestone tag for wave 27000 (product identity = 27000). */
#define B26998_MILESTONE_TAG  27000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26998_tag(void)
{
	return B26998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_27000 - report soft milestone product tag for wave 27000.
 *
 * Always returns 27000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 27000.
 */
uint32_t
gj_milestone_tag_27000(void)
{
	(void)NULL;
	return b26998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_27000(void)
    __attribute__((alias("gj_milestone_tag_27000")));
