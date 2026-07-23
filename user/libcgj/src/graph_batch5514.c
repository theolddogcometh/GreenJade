/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5514: firmware /lib/firmware path probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fw_path_probe_ok_5514(void);
 *     - Returns 1 (firmware tree path probe surface ready). Soft
 *       compile-time stub; does not walk /lib/firmware.
 *   uint32_t __gj_fw_path_probe_ok_5514  (alias)
 *   __libcgj_batch5514_marker = "libcgj-batch5514"
 *
 * Exclusive CREATE-ONLY (5511-5520: kernel modules path probes, firmware
 * blobs list stubs, amdgpu/mesa readiness for Deck APU). Unique
 * gj_fw_path_probe_ok_5514 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5514_marker[] = "libcgj-batch5514";

/* Soft lamp: /lib/firmware path probe surface considered present. */
#define B5514_FW_PATH_PROBE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5514_fw_path_probe_ok(void)
{
	return B5514_FW_PATH_PROBE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fw_path_probe_ok_5514 - firmware path probe readiness.
 *
 * Always returns 1. Soft pure-data path-probe tag for /lib/firmware style
 * trees; does not open or stat any path. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_fw_path_probe_ok_5514(void)
{
	(void)NULL;
	return b5514_fw_path_probe_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fw_path_probe_ok_5514(void)
    __attribute__((alias("gj_fw_path_probe_ok_5514")));
