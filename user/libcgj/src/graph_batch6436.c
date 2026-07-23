/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6436: depot chunk size soft-ok stub.
 *
 * Surface (unique symbols):
 *   int gj_depot_chunk_size_ok_6436(uint32_t size);
 *     - Return 1 if size is a power of two in [1024, 1048576]
 *       (typical depot CDN/chunk transfer sizes), else 0.
 *   int __gj_depot_chunk_size_ok_6436  (alias)
 *   __libcgj_batch6436_marker = "libcgj-batch6436"
 *
 * Exclusive continuum CREATE-ONLY (6431-6440: DLC/depot manifest size
 * stubs — dlc_id_ok_6431, depot_id_ok_6432, manifest_size_ok_6433,
 * dlc_manifest_hdr_size_6434, depot_manifest_max_size_6435,
 * depot_chunk_size_ok_6436, manifest_entry_count_ok_6437,
 * dlc_size_pack_6438, depot_manifest_size_gate_6439, batch_id_6440).
 * Distinct from gj_steam_cdn_chunk_align_ok_5468. Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6436_marker[] = "libcgj-batch6436";

#define B6436_CHUNK_MIN  1024u
#define B6436_CHUNK_MAX  1048576u  /* 1 MiB */

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6436_is_pow2(uint32_t u)
{
	if (u == 0u) {
		return 0;
	}
	return ((u & (u - 1u)) == 0u) ? 1 : 0;
}

static int
b6436_chunk_ok(uint32_t uSize)
{
	if (uSize < B6436_CHUNK_MIN) {
		return 0;
	}
	if (uSize > B6436_CHUNK_MAX) {
		return 0;
	}
	return b6436_is_pow2(uSize);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_chunk_size_ok_6436 - soft depot chunk size validity gate.
 *
 * size: candidate depot content chunk length in bytes.
 * Returns 1 when size is a power of two in [1024, 1048576], else 0.
 * Does not perform I/O. Does not call libc. No parent wires.
 */
int
gj_depot_chunk_size_ok_6436(uint32_t uSize)
{
	(void)NULL;
	return b6436_chunk_ok(uSize);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_depot_chunk_size_ok_6436(uint32_t uSize)
    __attribute__((alias("gj_depot_chunk_size_ok_6436")));
