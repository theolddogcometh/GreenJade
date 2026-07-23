/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9381: OCI config ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_oci_config_ok_u_9381(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       OCI config.json validity probe.
 *   uint32_t __gj_oci_config_ok_u_9381  (alias)
 *   __libcgj_batch9381_marker = "libcgj-batch9381"
 *
 * Exclusive continuum CREATE-ONLY (9381-9390: container/oci soft id
 * stubs — oci_config_ok_u_9381, oci_rootfs_ok_u_9382,
 * oci_runtime_ok_u_9383, oci_bundle_ok_u_9384, ctr_create_ok_u_9385,
 * ctr_start_ok_u_9386, ctr_stop_ok_u_9387, ctr_delete_ok_u_9388,
 * container_ready_u_9389, batch_id_9390). Unique surface only; no
 * multi-def. No parent wires. No __int128. No OCI/runtime
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9381_marker[] = "libcgj-batch9381";

/* Soft OCI config-ok lamp: always off (not a real config.json probe). */
#define B9381_OCI_CONFIG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9381_oci_config_ok(void)
{
	return B9381_OCI_CONFIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oci_config_ok_u_9381 - OCI config ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse config.json or
 * call libc. No parent wires.
 */
uint32_t
gj_oci_config_ok_u_9381(void)
{
	(void)NULL;
	return b9381_oci_config_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oci_config_ok_u_9381(void)
    __attribute__((alias("gj_oci_config_ok_u_9381")));
