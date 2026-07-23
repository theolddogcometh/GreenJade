/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9622: soft podman image-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_podman_image_ok_u_9622(void);
 *     - Returns 1 (podman image soft-id continuum ok). Pure-data
 *       product tag; does not pull/list images or invoke podman.
 *   uint32_t __gj_podman_image_ok_u_9622  (alias)
 *   __libcgj_batch9622_marker = "libcgj-batch9622"
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

const char __libcgj_batch9622_marker[] = "libcgj-batch9622";

/* Podman image soft-ok lamp for continuum 9621-9630. */
#define B9622_PODMAN_IMAGE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9622_image_ok(void)
{
	return B9622_PODMAN_IMAGE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_podman_image_ok_u_9622 - soft podman image-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not inspect image stores
 * or call libc. No parent wires.
 */
uint32_t
gj_podman_image_ok_u_9622(void)
{
	(void)NULL;
	return b9622_image_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_podman_image_ok_u_9622(void)
    __attribute__((alias("gj_podman_image_ok_u_9622")));
