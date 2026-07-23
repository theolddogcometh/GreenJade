/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5865: Wayland wl_seat protocol version table.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_seat_proto_ver_5865(void);
 *     - Returns the wl_seat interface protocol version (9).
 *   uint32_t __gj_wl_seat_proto_ver_5865  (alias)
 *   __libcgj_batch5865_marker = "libcgj-batch5865"
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

const char __libcgj_batch5865_marker[] = "libcgj-batch5865";

/* Wayland core wl_seat interface version (wayland.xml). */
#define B5865_WL_SEAT_PROTO_VER  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5865_wl_seat_proto_ver(void)
{
	return B5865_WL_SEAT_PROTO_VER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_seat_proto_ver_5865 - report wl_seat protocol version.
 *
 * Always returns 9. Soft compile-time constant for freestanding Wayland
 * protocol version tables; does not bind a seat. No parent wires.
 */
uint32_t
gj_wl_seat_proto_ver_5865(void)
{
	(void)NULL;
	return b5865_wl_seat_proto_ver();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_seat_proto_ver_5865(void)
    __attribute__((alias("gj_wl_seat_proto_ver_5865")));
