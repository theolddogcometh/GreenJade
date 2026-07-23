/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8960: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8960(void);
 *     - Returns the compile-time graph batch number for this TU (8960).
 *   uint32_t __gj_batch_id_8960  (alias)
 *   __libcgj_batch8960_marker = "libcgj-batch8960"
 *
 * Exclusive continuum CREATE-ONLY (8951-8960: wayland/xdg id stubs —
 * wl_display_ready, wl_compositor_ready, wl_surface_ready, wl_seat_ready,
 * xdg_wm_base_ready, xdg_toplevel_ready, xdg_popup_ready, wl_shm_ready,
 * wayland_ready, batch_id_8960).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8960_marker[] = "libcgj-batch8960";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8960_id(void)
{
	return 8960u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8960 - report this TU's graph batch number.
 *
 * Always returns 8960. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8960(void)
{
	(void)NULL;
	return b8960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8960(void)
    __attribute__((alias("gj_batch_id_8960")));
