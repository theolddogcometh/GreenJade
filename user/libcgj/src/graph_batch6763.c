/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6763: bpf BPF_MAP_UPDATE_ELEM cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_map_update_elem_6763(void);
 *     - Returns 2 (soft BPF_MAP_UPDATE_ELEM bpf(2) command id).
 *       Pure-data catalog stub; not a live bpf syscall.
 *   uint32_t __gj_bpf_cmd_map_update_elem_6763  (alias)
 *   __libcgj_batch6763_marker = "libcgj-batch6763"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs). Unique
 * gj_bpf_cmd_map_update_elem_6763 surface only; no multi-def.
 * Distinct from lookup (batch6762) and delete (batch6764).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6763_marker[] = "libcgj-batch6763";

/* Soft BPF_MAP_UPDATE_ELEM (create or update key/value). */
#define B6763_BPF_MAP_UPDATE_ELEM  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6763_map_update_elem(void)
{
	return B6763_BPF_MAP_UPDATE_ELEM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_map_update_elem_6763 - soft BPF_MAP_UPDATE_ELEM cmd id.
 *
 * Always returns 2. Pure-data stub for product bpf-cmd catalogs;
 * does not issue bpf(2) or touch map fds. No parent wires.
 */
uint32_t
gj_bpf_cmd_map_update_elem_6763(void)
{
	(void)NULL;
	return b6763_map_update_elem();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_map_update_elem_6763(void)
    __attribute__((alias("gj_bpf_cmd_map_update_elem_6763")));
