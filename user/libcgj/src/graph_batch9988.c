/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9988: milestone 10000 fourCC product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_10000(void);
 *     - Returns the milestone fourCC tag for the milestone 10000
 *       continuum (always 0x4d31304b, fourCC "M10K"). Soft compile-time
 *       product identity tag.
 *   uint32_t __gj_milestone_tag_10000  (alias)
 *   __libcgj_batch9988_marker = "libcgj-batch9988"
 *
 * Milestone 10000 exclusive continuum CREATE-ONLY (9981-9990). Unique
 * gj_milestone_tag_10000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_9900 (batch9898), gj_milestone_tag_9800
 * (batch9798), gj_milestone_tag_9700 (batch9698), and
 * gj_batch_id_9990 (batch9990). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9988_marker[] = "libcgj-batch9988";

/* Soft "M10K" fourCC-style milestone tag for wave 10000. */
#define B9988_MILESTONE_TAG  0x4d31304bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9988_tag(void)
{
	return B9988_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_10000 - report milestone fourCC tag for wave 10000.
 *
 * Always returns 0x4d31304b ("M10K"). Soft pure-data only; does not
 * parse ELF notes or call libc. No parent wires.
 */
uint32_t
gj_milestone_tag_10000(void)
{
	(void)NULL;
	return b9988_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_milestone_tag_10000(void)
    __attribute__((alias("gj_milestone_tag_10000")));
