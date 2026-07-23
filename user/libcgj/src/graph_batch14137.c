/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14137: PipeWire context flags soft mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_context_flags_mask_14137(uint32_t no_dbus,
 *                                           uint32_t no_config,
 *                                           uint32_t remote);
 *     - Build a soft context-option capability mask:
 *         bit0 = no_dbus nonzero
 *         bit1 = no_config nonzero
 *         bit2 = remote nonzero
 *       Other bits are zero. Soft pure-data only.
 *   uint32_t __gj_pw_context_flags_mask_14137  (alias)
 *   __libcgj_batch14137_marker = "libcgj-batch14137"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_context_flags_mask_14137 surface only;
 * no multi-def. Distinct from gj_pw_port_dir_mask_6138. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14137_marker[] = "libcgj-batch14137";

#define B14137_FLAG_NO_DBUS    1u
#define B14137_FLAG_NO_CONFIG  2u
#define B14137_FLAG_REMOTE     4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14137_context_flags_mask(uint32_t uNoDbus, uint32_t uNoConfig,
    uint32_t uRemote)
{
	uint32_t uMask;

	uMask = 0u;
	if (uNoDbus != 0u) {
		uMask |= B14137_FLAG_NO_DBUS;
	}
	if (uNoConfig != 0u) {
		uMask |= B14137_FLAG_NO_CONFIG;
	}
	if (uRemote != 0u) {
		uMask |= B14137_FLAG_REMOTE;
	}
	return uMask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_context_flags_mask_14137 - soft PipeWire context option mask.
 *
 * no_dbus / no_config / remote: nonzero means that option is set.
 * Returns mask with bit0=no_dbus, bit1=no_config, bit2=remote.
 * Soft pure-data stub; does not open a pw_context. No parent wires.
 */
uint32_t
gj_pw_context_flags_mask_14137(uint32_t uNoDbus, uint32_t uNoConfig,
    uint32_t uRemote)
{
	(void)NULL;
	return b14137_context_flags_mask(uNoDbus, uNoConfig, uRemote);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_context_flags_mask_14137(uint32_t uNoDbus,
    uint32_t uNoConfig, uint32_t uRemote)
    __attribute__((alias("gj_pw_context_flags_mask_14137")));
