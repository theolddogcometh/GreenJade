/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9022: PipeWire capture stream soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_stream_cap_id_9022(void);
 *     - Return soft PipeWire/Pulse capture stream catalog id (2).
 *   uint32_t __gj_pw_stream_cap_id_9022  (alias)
 *   __libcgj_batch9022_marker = "libcgj-batch9022"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_stream_cap_id_9022 surface only; no
 * multi-def. Distinct from gj_pw_stream_play_id_9021 (batch9021) and
 * gj_pw_media_class_tag_6134 source tag. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9022_marker[] = "libcgj-batch9022";

/* Soft capture stream catalog id (input / record path). */
#define B9022_PW_STREAM_CAP_ID  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9022_pw_stream_cap_id(void)
{
	return B9022_PW_STREAM_CAP_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_stream_cap_id_9022 - soft PipeWire capture stream catalog id.
 *
 * Always returns 2 (capture / input stream kind). Soft pure-data
 * catalog id only; does not open a capture stream or call libc.
 * No parent wires.
 */
uint32_t
gj_pw_stream_cap_id_9022(void)
{
	(void)NULL;
	return b9022_pw_stream_cap_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_stream_cap_id_9022(void)
    __attribute__((alias("gj_pw_stream_cap_id_9022")));
