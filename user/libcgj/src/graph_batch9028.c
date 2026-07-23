/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9028: PipeWire link-id ok soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_link_ok_u_9028(uint32_t link_id);
 *     - Return 1 if link_id is nonzero (valid soft link handle), else 0.
 *   uint32_t __gj_pw_link_ok_u_9028  (alias)
 *   __libcgj_batch9028_marker = "libcgj-batch9028"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_link_ok_u_9028 surface only; no
 * multi-def. Distinct from gj_pw_link_ready_p_6139,
 * gj_pw_link_ends_pack_6133, and gj_pw_node_ok_u_9027. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9028_marker[] = "libcgj-batch9028";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9028_pw_link_ok(uint32_t u32LinkId)
{
	/* Soft stub: zero remains invalid / unset; nonzero is ok. */
	return (u32LinkId != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_link_ok_u_9028 - 1 if soft PipeWire-shaped link id is set.
 *
 * link_id: soft graph link identifier (0 = invalid / unset).
 * Returns 1 for nonzero ids, else 0. Soft pure-data validity check.
 * No parent wires.
 */
uint32_t
gj_pw_link_ok_u_9028(uint32_t u32LinkId)
{
	(void)NULL;
	return b9028_pw_link_ok(u32LinkId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_link_ok_u_9028(uint32_t u32LinkId)
    __attribute__((alias("gj_pw_link_ok_u_9028")));
