/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10787: bar3 audio checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_audio_check_u_10787(void);
 *     - Returns 0 (soft stub: audio checklist slot is soft/unprobed;
 *       not a runtime audio device probe).
 *   uint32_t __gj_bar3_audio_check_u_10787  (alias)
 *   __libcgj_batch10787_marker = "libcgj-batch10787"
 *
 * Exclusive continuum CREATE-ONLY (10781-10790: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_audio_check_u_10787 surface only; no
 * multi-def. Distinct from gj_bar3_audio_check_u_10587,
 * gj_bar3_audio_check_u_10387, and sibling check_u soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10787_marker[] = "libcgj-batch10787";

/* Soft-stub audio checklist value (unprobed). */
#define B10787_AUDIO_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10787_audio_check(void)
{
	return B10787_AUDIO_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_audio_check_u_10787 - soft bar3 audio checklist stub.
 *
 * Always returns 0 (soft/unprobed audio slot). Does not call libc or
 * probe audio devices. No parent wires.
 */
uint32_t
gj_bar3_audio_check_u_10787(void)
{
	(void)NULL;
	return b10787_audio_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_audio_check_u_10787(void)
    __attribute__((alias("gj_bar3_audio_check_u_10787")));
