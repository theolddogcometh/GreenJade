/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5863: Wayland wl_compositor protocol version table.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_compositor_proto_ver_5863(void);
 *     - Returns the wl_compositor interface protocol version (6).
 *   uint32_t __gj_wl_compositor_proto_ver_5863  (alias)
 *   __libcgj_batch5863_marker = "libcgj-batch5863"
 *
 * Exclusive continuum CREATE-ONLY (5861-5870: Wayland protocol version
 * tables —
 * wl_display_proto_ver_5861, wl_registry_proto_ver_5862,
 * wl_compositor_proto_ver_5863, wl_shm_proto_ver_5864,
 * wl_seat_proto_ver_5865, wl_surface_proto_ver_5866,
 * wl_output_proto_ver_5867, xdg_wm_base_proto_ver_5868,
 * wl_data_device_proto_ver_5869, batch_id_5870). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5863_marker[] = "libcgj-batch5863";

/* Wayland core wl_compositor interface version (wayland.xml). */
#define B5863_WL_COMPOSITOR_PROTO_VER  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5863_wl_compositor_proto_ver(void)
{
	return B5863_WL_COMPOSITOR_PROTO_VER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_compositor_proto_ver_5863 - report wl_compositor protocol version.
 *
 * Always returns 6. Soft compile-time constant for freestanding Wayland
 * protocol version tables; does not bind a compositor. No parent wires.
 */
uint32_t
gj_wl_compositor_proto_ver_5863(void)
{
	(void)NULL;
	return b5863_wl_compositor_proto_ver();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_compositor_proto_ver_5863(void)
    __attribute__((alias("gj_wl_compositor_proto_ver_5863")));
