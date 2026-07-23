/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11987: bar3 audio checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_audio_check_u_11987(void);
 *     - Returns 0 (soft stub: audio checklist slot is soft/unprobed;
 *       not a runtime audio stack probe).
 *   uint32_t __gj_bar3_audio_check_u_11987  (alias)
 *   __libcgj_batch11987_marker = "libcgj-batch11987"
 *
 * Exclusive continuum CREATE-ONLY (11981-11990: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_audio_check_u_11987 surface only; no
 * multi-def. Distinct from gj_bar3_audio_check_u_11787 and sibling
 * check_u soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11987_marker[] = "libcgj-batch11987";

/* Soft-stub audio checklist value (unprobed). */
#define B11987_AUDIO_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11987_audio_check(void)
{
	return B11987_AUDIO_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_audio_check_u_11987 - soft bar3 audio checklist stub.
 *
 * Always returns 0 (soft/unprobed audio slot). Does not call libc or
 * probe audio devices. No parent wires.
 */
uint32_t
gj_bar3_audio_check_u_11987(void)
{
	(void)NULL;
	return b11987_audio_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_audio_check_u_11987(void)
    __attribute__((alias("gj_bar3_audio_check_u_11987")));
