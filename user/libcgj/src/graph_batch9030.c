/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9030: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9030(void);
 *     - Returns the compile-time graph batch number for this TU (9030).
 *   uint32_t __gj_batch_id_9030  (alias)
 *   __libcgj_batch9030_marker = "libcgj-batch9030"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9030_marker[] = "libcgj-batch9030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9030_id(void)
{
	return 9030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9030 - report this TU's graph batch number.
 *
 * Always returns 9030. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9030(void)
{
	(void)NULL;
	return b9030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9030(void)
    __attribute__((alias("gj_batch_id_9030")));
