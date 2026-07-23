/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6253: input debounce window clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_debounce_ms_6253(uint32_t ms);
 *     - Soft debounce window clamp. Maps requested ms into [0, 50];
 *       values above 50 clamp to 50. Pure data; does not arm timers.
 *   uint32_t __gj_input_debounce_ms_6253  (alias)
 *   __libcgj_batch6253_marker = "libcgj-batch6253"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_debounce_ms_6253 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6253_marker[] = "libcgj-batch6253";

/* Soft debounce ceiling (milliseconds). */
#define B6253_MS_MAX  50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6253_debounce_clamp(uint32_t u32Ms)
{
	return (u32Ms > B6253_MS_MAX) ? B6253_MS_MAX : u32Ms;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_debounce_ms_6253 - input debounce window clamp stub.
 *
 * ms: requested soft debounce window in milliseconds
 *
 * Returns clamped window in [0, 50]. Soft pure-data clamp for the
 * 6251-6260 continuum. No parent wires.
 */
uint32_t
gj_input_debounce_ms_6253(uint32_t ms)
{
	(void)NULL;
	return b6253_debounce_clamp(ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_debounce_ms_6253(uint32_t ms)
    __attribute__((alias("gj_input_debounce_ms_6253")));
