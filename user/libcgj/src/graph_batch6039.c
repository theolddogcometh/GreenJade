/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6039: GPU memory heap budget type tags.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_type_device_local_6039(void);
 *     - Returns 1 (soft device-local heap type tag).
 *   uint32_t gj_gpu_heap_budget_type_host_visible_6039(void);
 *     - Returns 2 (soft host-visible heap type tag).
 *   uint32_t gj_gpu_heap_budget_type_host_coherent_6039(void);
 *     - Returns 3 (soft host-coherent heap type tag).
 *   uint32_t gj_gpu_heap_budget_type_ok_6039(uint32_t type);
 *     - Return 1 when type is in {1,2,3}, else 0.
 *   uint32_t __gj_gpu_heap_budget_type_device_local_6039  (alias)
 *   uint32_t __gj_gpu_heap_budget_type_host_visible_6039  (alias)
 *   uint32_t __gj_gpu_heap_budget_type_host_coherent_6039  (alias)
 *   uint32_t __gj_gpu_heap_budget_type_ok_6039  (alias)
 *   __libcgj_batch6039_marker = "libcgj-batch6039"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Soft heap-type tags for budget accounting; no parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6039_marker[] = "libcgj-batch6039";

#define B6039_TYPE_DEVICE_LOCAL   1u
#define B6039_TYPE_HOST_VISIBLE   2u
#define B6039_TYPE_HOST_COHERENT  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6039_type_ok(uint32_t u32Type)
{
	if (u32Type == B6039_TYPE_DEVICE_LOCAL) {
		return 1u;
	}
	if (u32Type == B6039_TYPE_HOST_VISIBLE) {
		return 1u;
	}
	if (u32Type == B6039_TYPE_HOST_COHERENT) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_type_device_local_6039 - soft device-local type tag.
 *
 * Always returns 1.
 */
uint32_t
gj_gpu_heap_budget_type_device_local_6039(void)
{
	(void)NULL;
	return B6039_TYPE_DEVICE_LOCAL;
}

/*
 * gj_gpu_heap_budget_type_host_visible_6039 - soft host-visible type tag.
 *
 * Always returns 2.
 */
uint32_t
gj_gpu_heap_budget_type_host_visible_6039(void)
{
	return B6039_TYPE_HOST_VISIBLE;
}

/*
 * gj_gpu_heap_budget_type_host_coherent_6039 - soft host-coherent type tag.
 *
 * Always returns 3.
 */
uint32_t
gj_gpu_heap_budget_type_host_coherent_6039(void)
{
	return B6039_TYPE_HOST_COHERENT;
}

/*
 * gj_gpu_heap_budget_type_ok_6039 - soft heap type validity check.
 *
 * type: heap type tag
 *
 * Returns 1 when type is device-local, host-visible, or host-coherent.
 */
uint32_t
gj_gpu_heap_budget_type_ok_6039(uint32_t u32Type)
{
	return b6039_type_ok(u32Type);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_type_device_local_6039(void)
    __attribute__((alias("gj_gpu_heap_budget_type_device_local_6039")));

uint32_t __gj_gpu_heap_budget_type_host_visible_6039(void)
    __attribute__((alias("gj_gpu_heap_budget_type_host_visible_6039")));

uint32_t __gj_gpu_heap_budget_type_host_coherent_6039(void)
    __attribute__((alias("gj_gpu_heap_budget_type_host_coherent_6039")));

uint32_t __gj_gpu_heap_budget_type_ok_6039(uint32_t u32Type)
    __attribute__((alias("gj_gpu_heap_budget_type_ok_6039")));
