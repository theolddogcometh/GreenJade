/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8954: Wayland wl_seat id ready stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_seat_ready_u_8954(void);
 *     - Return 1 (soft ready lamp for wl_seat capability id path).
 *   uint32_t __gj_wl_seat_ready_u_8954  (alias)
 *   __libcgj_batch8954_marker = "libcgj-batch8954"
 *
 * Exclusive continuum CREATE-ONLY (8951-8960: wayland/xdg id stubs —
 * wl_display_ready, wl_compositor_ready, wl_surface_ready, wl_seat_ready,
 * xdg_wm_base_ready, xdg_toplevel_ready, xdg_popup_ready, wl_shm_ready,
 * wayland_ready, batch_id_8960).
 * Unique gj_wl_seat_ready_u_8954 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8954_marker[] = "libcgj-batch8954";

/* Soft ready lamp for wl_seat id path. */
#define B8954_WL_SEAT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8954_wl_seat_ready(void)
{
	return B8954_WL_SEAT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_seat_ready_u_8954 - soft wl_seat id ready lamp.
 *
 * Always returns 1. Link-time / catalog presence tag only; does not
 * bind seats or probe input devices. No parent wires.
 */
uint32_t
gj_wl_seat_ready_u_8954(void)
{
	(void)NULL;
	return b8954_wl_seat_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_seat_ready_u_8954(void)
    __attribute__((alias("gj_wl_seat_ready_u_8954")));
