/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch982: compiler memory barrier.
 *
 * Surface (unique symbols):
 *   void gj_compiler_barrier(void);
 *     — Compiler-only memory barrier (__asm__ volatile "" ::: "memory").
 *       Prevents reordering of memory accesses across the call at compile
 *       time. Not a CPU fence (no mfence/sfence/lfence).
 *   void __gj_compiler_barrier  (alias)
 *   __libcgj_batch982_marker = "libcgj-batch982"
 *
 * Does NOT redefine gj_secure_wipe (batch295) or other fence surfaces.
 * Clean-room freestanding pure C. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch982_marker[] = "libcgj-batch982";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compiler_barrier — stop the compiler from reordering across this point.
 *
 * Uses an empty volatile asm with a "memory" clobber. No instruction is
 * emitted for the CPU; this is purely a compile-time fence.
 */
void
gj_compiler_barrier(void)
{
	__asm__ __volatile__("" ::: "memory");
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_compiler_barrier(void)
    __attribute__((alias("gj_compiler_barrier")));
