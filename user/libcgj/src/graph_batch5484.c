/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5484: launcher hook presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_launcher_hook_id_5484(void);
 *     - Returns the soft launcher-hook identity tag for this wave
 *       (always 5484). Link-time presence tags launcher-hook surface.
 *   uint32_t __gj_launcher_hook_id_5484  (alias)
 *   __libcgj_batch5484_marker = "libcgj-batch5484"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_launcher_hook_id_5484 surface only; no
 * multi-def. Distinct from deck rank/score stubs. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5484_marker[] = "libcgj-batch5484";

#define B5484_HOOK_ID  5484u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5484_hook_id(void)
{
	return B5484_HOOK_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_launcher_hook_id_5484 - report launcher-hook identity stub.
 *
 * Always returns 5484. Soft compile-time product tag for launcher
 * pre-game hooks (no process attach). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_launcher_hook_id_5484(void)
{
	(void)NULL;
	return b5484_hook_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_launcher_hook_id_5484(void)
    __attribute__((alias("gj_launcher_hook_id_5484")));
