/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5861: Wayland wl_display protocol version table.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_display_proto_ver_5861(void);
 *     - Returns the wl_display interface protocol version (1).
 *   uint32_t __gj_wl_display_proto_ver_5861  (alias)
 *   __libcgj_batch5861_marker = "libcgj-batch5861"
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

const char __libcgj_batch5861_marker[] = "libcgj-batch5861";

/* Wayland core wl_display interface version (wayland.xml). */
#define B5861_WL_DISPLAY_PROTO_VER  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5861_wl_display_proto_ver(void)
{
	return B5861_WL_DISPLAY_PROTO_VER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_display_proto_ver_5861 - report wl_display protocol version.
 *
 * Always returns 1. Soft compile-time constant for freestanding Wayland
 * protocol version tables; does not connect a display. No parent wires.
 */
uint32_t
gj_wl_display_proto_ver_5861(void)
{
	(void)NULL;
	return b5861_wl_display_proto_ver();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_display_proto_ver_5861(void)
    __attribute__((alias("gj_wl_display_proto_ver_5861")));
