/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8811: bpf BPF_PROG_TYPE_SOCKET_FILTER id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_prog_type_socket_id_8811(void);
 *     - Return soft BPF_PROG_TYPE_SOCKET_FILTER prog type id (1).
 *   uint32_t __gj_bpf_prog_type_socket_id_8811  (alias)
 *   __libcgj_batch8811_marker = "libcgj-batch8811"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Unique gj_bpf_prog_type_socket_id_8811
 * surface only; no multi-def. Catalog id only; not a live bpf syscall.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8811_marker[] = "libcgj-batch8811";

/* Soft: BPF_PROG_TYPE_SOCKET_FILTER. */
#define B8811_BPF_PROG_TYPE_SOCKET_FILTER  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8811_prog_type_socket_id(void)
{
	return B8811_BPF_PROG_TYPE_SOCKET_FILTER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_prog_type_socket_id_8811 - soft SOCKET_FILTER prog type id.
 *
 * Always returns 1. Pure-data catalog stub; does not issue bpf(2) or
 * load bytecode. No parent wires.
 */
uint32_t
gj_bpf_prog_type_socket_id_8811(void)
{
	(void)NULL;
	return b8811_prog_type_socket_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_prog_type_socket_id_8811(void)
    __attribute__((alias("gj_bpf_prog_type_socket_id_8811")));
