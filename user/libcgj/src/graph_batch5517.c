/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5517: amdgpu kernel module Deck APU readiness.
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdgpu_mod_ready_5517(void);
 *     - Returns 1 (amdgpu module readiness lamp for Deck APU path).
 *       Soft compile-time tag only.
 *   uint32_t __gj_amdgpu_mod_ready_5517  (alias)
 *   __libcgj_batch5517_marker = "libcgj-batch5517"
 *
 * Exclusive CREATE-ONLY (5511-5520: kernel modules path probes, firmware
 * blobs list stubs, amdgpu/mesa readiness for Deck APU). Unique
 * gj_amdgpu_mod_ready_5517 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5517_marker[] = "libcgj-batch5517";

/* Soft lamp: amdgpu kmod considered ready for Deck APU continuum. */
#define B5517_AMDGPU_MOD_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5517_amdgpu_mod_ready(void)
{
	return B5517_AMDGPU_MOD_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdgpu_mod_ready_5517 - amdgpu module readiness for Deck APU.
 *
 * Always returns 1. Soft pure-data lamp; does not modprobe, open sysfs,
 * or inspect PCI IDs. Does not call libc. No parent wires.
 */
uint32_t
gj_amdgpu_mod_ready_5517(void)
{
	(void)NULL;
	return b5517_amdgpu_mod_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdgpu_mod_ready_5517(void)
    __attribute__((alias("gj_amdgpu_mod_ready_5517")));
