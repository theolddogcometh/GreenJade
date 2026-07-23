/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 arch hooks (scaffold). Default product build remains x86_64.
 */
#pragma once

#ifdef GJ_ARCH_AARCH64

#define GJ_ARCH_NAME "aarch64"

static inline void
gj_arch_cpu_relax(void)
{
    __asm__ volatile("yield" ::: "memory");
}

static inline void
gj_arch_mfence(void)
{
    __asm__ volatile("dmb ish" ::: "memory");
}

#endif /* GJ_ARCH_AARCH64 */
