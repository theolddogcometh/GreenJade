/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5515: firmware amdgpu blob list count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fw_amdgpu_blob_count_5515(void);
 *     - Returns 8 (stub count of expected Deck APU amdgpu firmware
 *       blob slots). Soft compile-time list size only.
 *   uint32_t __gj_fw_amdgpu_blob_count_5515  (alias)
 *   __libcgj_batch5515_marker = "libcgj-batch5515"
 *
 * Exclusive CREATE-ONLY (5511-5520: kernel modules path probes, firmware
 * blobs list stubs, amdgpu/mesa readiness for Deck APU). Unique
 * gj_fw_amdgpu_blob_count_5515 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5515_marker[] = "libcgj-batch5515";

/*
 * Stub list length for Deck APU amdgpu firmware slots (gc/sdma/vcn/psp
 * family placeholders). Not a real filesystem inventory.
 */
#define B5515_AMDGPU_BLOB_COUNT  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5515_amdgpu_blob_count(void)
{
	return B5515_AMDGPU_BLOB_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fw_amdgpu_blob_count_5515 - stub amdgpu firmware blob list length.
 *
 * Always returns 8. Soft pure-data list-size stub for Deck APU amdgpu
 * firmware blobs; does not enumerate /lib/firmware/amdgpu. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_fw_amdgpu_blob_count_5515(void)
{
	(void)NULL;
	return b5515_amdgpu_blob_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fw_amdgpu_blob_count_5515(void)
    __attribute__((alias("gj_fw_amdgpu_blob_count_5515")));
