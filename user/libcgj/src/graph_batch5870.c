/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5870: batch identity for Wayland proto-version wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5870(void);
 *     - Returns the compile-time graph batch number for this TU (5870).
 *   uint32_t __gj_batch_id_5870  (alias)
 *   __libcgj_batch5870_marker = "libcgj-batch5870"
 *
 * Exclusive continuum CREATE-ONLY (5861-5870: Wayland protocol version
 * tables —
 * wl_display_proto_ver_5861, wl_registry_proto_ver_5862,
 * wl_compositor_proto_ver_5863, wl_shm_proto_ver_5864,
 * wl_seat_proto_ver_5865, wl_surface_proto_ver_5866,
 * wl_output_proto_ver_5867, xdg_wm_base_proto_ver_5868,
 * wl_data_device_proto_ver_5869, batch_id_5870). Unique gj_batch_id_5870
 * surface only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5870_marker[] = "libcgj-batch5870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5870_id(void)
{
	return 5870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5870 - report this TU's graph batch number.
 *
 * Always returns 5870.
 */
uint32_t
gj_batch_id_5870(void)
{
	(void)NULL;
	return b5870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5870(void)
    __attribute__((alias("gj_batch_id_5870")));
