/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14487: bar3 audio checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_audio_check_u_14487(void);
 *     - Returns 0 (soft stub: audio checklist slot is soft/unprobed;
 *       not a runtime PipeWire/ALSA probe).
 *   uint32_t __gj_bar3_audio_check_u_14487  (alias)
 *   __libcgj_batch14487_marker = "libcgj-batch14487"
 *
 * Exclusive continuum CREATE-ONLY (14481-14490: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_audio_check_u_14487
 * surface only; no multi-def. Distinct from gj_bar3_*_check_u_1438x / 1428x / 1418x and
 * sibling check_u soft stubs in this wave. Distinct from
 * bar3 readiness soft stubs (14411-14420). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14487_marker[] = "libcgj-batch14487";

/* Bar3 checklist soft lamp: always off. */
#define B14487_BAR3_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14487_check(void)
{
	return B14487_BAR3_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_audio_check_u_14487 - soft bar3 audio checklist stub
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bar3_audio_check_u_14487(void)
{
	(void)NULL;
	return b14487_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_audio_check_u_14487(void)
    __attribute__((alias("gj_bar3_audio_check_u_14487")));
