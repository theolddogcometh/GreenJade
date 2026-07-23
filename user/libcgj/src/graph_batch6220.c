/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6220: DRM connector continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_drm_connector_continuum_6220(void);
 *     - Returns 1 (soft compile-time product tag: DRM/KMS connector
 *       type stubs continuum 6211-6220 is complete / ready).
 *   uint32_t gj_batch_id_6220(void);
 *     - Returns the compile-time graph batch number for this TU (6220).
 *   uint32_t __gj_drm_connector_continuum_6220  (alias)
 *   uint32_t __gj_batch_id_6220  (alias)
 *   __libcgj_batch6220_marker = "libcgj-batch6220"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs — type_ok_6211, is_digital_6212, is_analog_6213, is_panel_6214,
 * is_hdmi_6215, is_dp_family_6216, status_ok_6217, type_clamp_6218,
 * type_bit_6219, continuum + batch_id_6220). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6220_marker[] = "libcgj-batch6220";

/* Continuum-complete lamp for the 6211-6220 exclusive wave. */
#define B6220_CONTINUUM_READY  1u
#define B6220_BATCH_ID         6220u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6220_continuum(void)
{
	return B6220_CONTINUUM_READY;
}

static uint32_t
b6220_id(void)
{
	return B6220_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_continuum_6220 - continuum-ready tag for 6211-6220.
 *
 * Always returns 1. Soft pure-data product tag that the DRM/KMS
 * connector type stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_drm_connector_continuum_6220(void)
{
	(void)NULL;
	return b6220_continuum();
}

/*
 * gj_batch_id_6220 - report this TU's graph batch number.
 *
 * Always returns 6220.
 */
uint32_t
gj_batch_id_6220(void)
{
	return b6220_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_drm_connector_continuum_6220(void)
    __attribute__((alias("gj_drm_connector_continuum_6220")));

uint32_t __gj_batch_id_6220(void)
    __attribute__((alias("gj_batch_id_6220")));
