/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9625: soft podman network-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_podman_network_ok_u_9625(void);
 *     - Returns 1 (podman network soft-id continuum ok). Pure-data
 *       product tag; does not create/list networks or invoke podman.
 *   uint32_t __gj_podman_network_ok_u_9625  (alias)
 *   __libcgj_batch9625_marker = "libcgj-batch9625"
 *
 * Exclusive continuum CREATE-ONLY (9621-9630: podman soft id stubs —
 * podman_ok_u_9621, image_ok_u_9622, pod_ok_u_9623, volume_ok_u_9624,
 * network_ok_u_9625, build_ok_u_9626, run_ok_u_9627, quadlet_ok_u_9628,
 * ready_u_9629, batch_id_9630). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9625_marker[] = "libcgj-batch9625";

/* Podman network soft-ok lamp for continuum 9621-9630. */
#define B9625_PODMAN_NETWORK_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9625_network_ok(void)
{
	return B9625_PODMAN_NETWORK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_podman_network_ok_u_9625 - soft podman network-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not configure CNI/netavark
 * or call libc. No parent wires.
 */
uint32_t
gj_podman_network_ok_u_9625(void)
{
	(void)NULL;
	return b9625_network_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_podman_network_ok_u_9625(void)
    __attribute__((alias("gj_podman_network_ok_u_9625")));
