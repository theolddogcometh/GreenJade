/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8817: bpf BPF_PROG_LOAD cmd id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_prog_load_id_8817(void);
 *     - Return soft BPF_PROG_LOAD bpf(2) command id (5).
 *   uint32_t __gj_bpf_cmd_prog_load_id_8817  (alias)
 *   __libcgj_batch8817_marker = "libcgj-batch8817"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Unique gj_bpf_cmd_prog_load_id_8817
 * surface only; no multi-def. Distinct from gj_bpf_cmd_prog_load_6766
 * (batch6766). Catalog id only; not a live bpf syscall. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8817_marker[] = "libcgj-batch8817";

/* Soft: BPF_PROG_LOAD (verify and load eBPF program). */
#define B8817_BPF_PROG_LOAD  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8817_cmd_prog_load_id(void)
{
	return B8817_BPF_PROG_LOAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_prog_load_id_8817 - soft BPF_PROG_LOAD cmd catalog id.
 *
 * Always returns 5. Pure-data stub for product bpf-cmd catalogs; does
 * not issue bpf(2) or load bytecode. No parent wires.
 */
uint32_t
gj_bpf_cmd_prog_load_id_8817(void)
{
	(void)NULL;
	return b8817_cmd_prog_load_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_prog_load_id_8817(void)
    __attribute__((alias("gj_bpf_cmd_prog_load_id_8817")));
