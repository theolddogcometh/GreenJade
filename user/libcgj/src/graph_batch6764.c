/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6764: bpf BPF_MAP_DELETE_ELEM cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_map_delete_elem_6764(void);
 *     - Returns 3 (soft BPF_MAP_DELETE_ELEM bpf(2) command id).
 *       Pure-data catalog stub; not a live bpf syscall.
 *   uint32_t __gj_bpf_cmd_map_delete_elem_6764  (alias)
 *   __libcgj_batch6764_marker = "libcgj-batch6764"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs). Unique
 * gj_bpf_cmd_map_delete_elem_6764 surface only; no multi-def.
 * Distinct from update (batch6763) and get_next_key (batch6765).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6764_marker[] = "libcgj-batch6764";

/* Soft BPF_MAP_DELETE_ELEM (delete key from map). */
#define B6764_BPF_MAP_DELETE_ELEM  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6764_map_delete_elem(void)
{
	return B6764_BPF_MAP_DELETE_ELEM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_map_delete_elem_6764 - soft BPF_MAP_DELETE_ELEM cmd id.
 *
 * Always returns 3. Pure-data stub for product bpf-cmd catalogs;
 * does not issue bpf(2) or touch map fds. No parent wires.
 */
uint32_t
gj_bpf_cmd_map_delete_elem_6764(void)
{
	(void)NULL;
	return b6764_map_delete_elem();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_map_delete_elem_6764(void)
    __attribute__((alias("gj_bpf_cmd_map_delete_elem_6764")));
