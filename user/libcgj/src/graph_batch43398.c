/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43398: milestone 43400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_43400(void);
 *     - Returns the milestone tag for the milestone 43400 continuum
 *       (always 43400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_43400  (alias)
 *   __libcgj_batch43398_marker = "libcgj-batch43398"
 *
 * Milestone 43400 exclusive continuum CREATE-ONLY (43391-43400). Unique
 * gj_milestone_tag_43400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_43300 / gj_milestone_tag_43200 / gj_milestone_tag_43100,
 * gj_graph_milestone_43400 (batch43400), and gj_continuum_wave_43400
 * (batch43399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (43391–43400): host soft probes
 * (cgj_soft_milestone_43400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=43400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 43398. */
const char __libcgj_batch43398_marker[] = "libcgj-batch43398";

/* Soft milestone tag for wave 43400 (product identity = 43400). */
#define B43398_MILESTONE_TAG  43400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43398_tag(void)
{
	return B43398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_43400 - report soft milestone product tag for wave 43400.
 *
 * Always returns 43400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 43400.
 */
uint32_t
gj_milestone_tag_43400(void)
{
	(void)NULL;
	return b43398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_43400(void)
    __attribute__((alias("gj_milestone_tag_43400")));
