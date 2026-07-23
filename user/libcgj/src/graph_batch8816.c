/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8816: bpf BPF_MAP_CREATE cmd id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_map_create_id_8816(void);
 *     - Return soft BPF_MAP_CREATE bpf(2) command id (0).
 *   uint32_t __gj_bpf_cmd_map_create_id_8816  (alias)
 *   __libcgj_batch8816_marker = "libcgj-batch8816"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Unique gj_bpf_cmd_map_create_id_8816
 * surface only; no multi-def. Distinct from gj_bpf_cmd_map_create_6761
 * (batch6761). Catalog id only; not a live bpf syscall. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8816_marker[] = "libcgj-batch8816";

/* Soft: BPF_MAP_CREATE (create a map). */
#define B8816_BPF_MAP_CREATE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8816_cmd_map_create_id(void)
{
	return B8816_BPF_MAP_CREATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_map_create_id_8816 - soft BPF_MAP_CREATE cmd catalog id.
 *
 * Always returns 0. Pure-data stub for product bpf-cmd catalogs; does
 * not issue bpf(2) or touch map fds. No parent wires.
 */
uint32_t
gj_bpf_cmd_map_create_id_8816(void)
{
	(void)NULL;
	return b8816_cmd_map_create_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_map_create_id_8816(void)
    __attribute__((alias("gj_bpf_cmd_map_create_id_8816")));
