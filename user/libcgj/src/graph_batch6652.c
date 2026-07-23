/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6652: eventfd flags soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_flags_ok_6652(uint32_t flags);
 *     - Soft eventfd2 flags check: 1 when only known flag bits are set
 *       (EFD_SEMAPHORE=0x1, EFD_NONBLOCK=0x800, EFD_CLOEXEC=0x80000);
 *       else 0.
 *   uint32_t __gj_efd_flags_ok_6652  (alias)
 *   __libcgj_batch6652_marker = "libcgj-batch6652"
 *
 * Exclusive continuum CREATE-ONLY (6651-6660: eventfd counter stubs —
 * initval_ok_6651, flags_ok_6652, counter_bytes_6653, read_len_ok_6654,
 * write_val_ok_6655, sem_mode_6656, counter_add_lo_6657,
 * counter_sem_take_6658, counter_visible_6659, counter_ready +
 * batch_id_6660). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6652_marker[] = "libcgj-batch6652";

/* Known eventfd2 flag bits (linux/eventfd.h soft mirror). */
#define B6652_EFD_SEMAPHORE  0x00000001u
#define B6652_EFD_NONBLOCK   0x00000800u
#define B6652_EFD_CLOEXEC    0x00080000u
#define B6652_EFD_FLAGS_MASK (B6652_EFD_SEMAPHORE | B6652_EFD_NONBLOCK \
			      | B6652_EFD_CLOEXEC)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6652_flags_ok(uint32_t u32Flags)
{
	if ((u32Flags & ~B6652_EFD_FLAGS_MASK) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_flags_ok_6652 - soft eventfd2 flags validity.
 *
 * flags: eventfd2 flags word
 *
 * Returns 1 when only SEMAPHORE/NONBLOCK/CLOEXEC bits are set, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_efd_flags_ok_6652(uint32_t flags)
{
	(void)NULL;
	return b6652_flags_ok(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_flags_ok_6652(uint32_t flags)
    __attribute__((alias("gj_efd_flags_ok_6652")));
