/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11137: SPA soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spa_ok_u_11137(void);
 *     - Always returns 0. Soft stub for audio soft-id continuum;
 *       SPA (Simple Plugin API) path is intentionally not reported ok.
 *   uint32_t __gj_spa_ok_u_11137  (alias)
 *   __libcgj_batch11137_marker = "libcgj-batch11137"
 *
 * Exclusive continuum CREATE-ONLY (11131-11140: audio soft id stubs —
 * pipewire_ok_u_11131, pulseaudio_ok_u_11132, alsa_ok_u_11133,
 * jack_ok_u_11134, wireplumber_ok_u_11135, pipewire_pulse_ok_u_11136,
 * spa_ok_u_11137, rtkit_ok_u_11138, audio_soft_ready_u_11139,
 * batch_id_11140). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11137_marker[] = "libcgj-batch11137";

/* Soft SPA-ok lamp: always off for this continuum. */
#define B11137_SPA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11137_spa_ok(void)
{
	return B11137_SPA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spa_ok_u_11137 - SPA soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not load SPA plugins or
 * call libc. No parent wires.
 */
uint32_t
gj_spa_ok_u_11137(void)
{
	(void)NULL;
	return b11137_spa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spa_ok_u_11137(void)
    __attribute__((alias("gj_spa_ok_u_11137")));
