/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6767: bpf BPF_OBJ_PIN cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_obj_pin_6767(void);
 *     - Returns 6 (soft BPF_OBJ_PIN bpf(2) command id). Pure-data
 *       catalog stub; not a live bpf syscall.
 *   uint32_t __gj_bpf_cmd_obj_pin_6767  (alias)
 *   __libcgj_batch6767_marker = "libcgj-batch6767"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs). Unique
 * gj_bpf_cmd_obj_pin_6767 surface only; no multi-def.
 * Distinct from prog_load (batch6766) and obj_get (batch6768).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6767_marker[] = "libcgj-batch6767";

/* Soft BPF_OBJ_PIN (pin map/prog to bpffs path). */
#define B6767_BPF_OBJ_PIN  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6767_obj_pin(void)
{
	return B6767_BPF_OBJ_PIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_obj_pin_6767 - soft BPF_OBJ_PIN cmd catalog id.
 *
 * Always returns 6. Pure-data stub for product bpf-cmd catalogs;
 * does not issue bpf(2) or touch bpffs. No parent wires.
 */
uint32_t
gj_bpf_cmd_obj_pin_6767(void)
{
	(void)NULL;
	return b6767_obj_pin();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_obj_pin_6767(void)
    __attribute__((alias("gj_bpf_cmd_obj_pin_6767")));
