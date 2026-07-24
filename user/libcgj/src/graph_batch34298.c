/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34298: milestone 34300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_34300(void);
 *     - Returns the milestone tag for the milestone 34300 continuum
 *       (always 34300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_34300  (alias)
 *   __libcgj_batch34298_marker = "libcgj-batch34298"
 *
 * Milestone 34300 exclusive continuum CREATE-ONLY (34291-34300). Unique
 * gj_milestone_tag_34300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_34200 / gj_milestone_tag_34100 / gj_milestone_tag_34000,
 * gj_graph_milestone_34300 (batch34300), and gj_continuum_wave_34300
 * (batch34299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (34291–34300): host soft probes
 * (cgj_soft_milestone_34300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=34300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 34298. */
const char __libcgj_batch34298_marker[] = "libcgj-batch34298";

/* Soft milestone tag for wave 34300 (product identity = 34300). */
#define B34298_MILESTONE_TAG  34300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34298_tag(void)
{
	return B34298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_34300 - report soft milestone product tag for wave 34300.
 *
 * Always returns 34300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 34300.
 */
uint32_t
gj_milestone_tag_34300(void)
{
	(void)NULL;
	return b34298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_34300(void)
    __attribute__((alias("gj_milestone_tag_34300")));
