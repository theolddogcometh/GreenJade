/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8814: bpf BPF_MAP_TYPE_ARRAY id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_map_array_id_8814(void);
 *     - Return soft BPF_MAP_TYPE_ARRAY map type id (2).
 *   uint32_t __gj_bpf_map_array_id_8814  (alias)
 *   __libcgj_batch8814_marker = "libcgj-batch8814"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Unique gj_bpf_map_array_id_8814 surface
 * only; no multi-def. Catalog id only; not a live map create. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8814_marker[] = "libcgj-batch8814";

/* Soft: BPF_MAP_TYPE_ARRAY. */
#define B8814_BPF_MAP_TYPE_ARRAY  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8814_map_array_id(void)
{
	return B8814_BPF_MAP_TYPE_ARRAY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_map_array_id_8814 - soft BPF_MAP_TYPE_ARRAY map type id.
 *
 * Always returns 2. Pure-data catalog stub; does not create maps or
 * issue bpf(2). No parent wires.
 */
uint32_t
gj_bpf_map_array_id_8814(void)
{
	(void)NULL;
	return b8814_map_array_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_map_array_id_8814(void)
    __attribute__((alias("gj_bpf_map_array_id_8814")));
