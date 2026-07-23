/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9024: PipeWire F32 PCM format soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_format_f32_id_9024(void);
 *     - Return soft F32 PCM format catalog id (32).
 *   uint32_t __gj_pw_format_f32_id_9024  (alias)
 *   __libcgj_batch9024_marker = "libcgj-batch9024"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_format_f32_id_9024 surface only; no
 * multi-def. Distinct from gj_pw_format_s16_id_9023 (batch9023). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9024_marker[] = "libcgj-batch9024";

/* Soft 32-bit float PCM format catalog id (bit-width mnemonic). */
#define B9024_PW_FMT_F32_ID  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9024_pw_format_f32_id(void)
{
	return B9024_PW_FMT_F32_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_format_f32_id_9024 - soft PipeWire F32 PCM format catalog id.
 *
 * Always returns 32. Soft pure-data format tag only; does not open a
 * stream or negotiate SPA params. No float arithmetic. No parent wires.
 */
uint32_t
gj_pw_format_f32_id_9024(void)
{
	(void)NULL;
	return b9024_pw_format_f32_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_format_f32_id_9024(void)
    __attribute__((alias("gj_pw_format_f32_id_9024")));
