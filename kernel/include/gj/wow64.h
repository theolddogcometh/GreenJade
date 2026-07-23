/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * WoW64 / 32-bit Linux personality scaffold (kernel long mode only).
 * Pure C; dual MIT OR Apache-2.0 — clean-room NR map, no Wine/GPL paste.
 * Full PE32 loader is userspace; kernel maps i386 NRs and zero-extends args.
 * CS32 int 0x80 is handled in trap_dispatch; this table serves hybrid SYSCALL.
 */
#pragma once

#include <gj/types.h>

/** Non-zero when process is in 32-bit Linux personality mode. */
int  wow64_enabled(void);
void wow64_set(int fOn);

/**
 * Map i386 Linux syscall NR → x86_64 NR for hybrid dispatch.
 * Returns 0 and writes *pOutNr; -1 if pOutNr is NULL (defensive).
 * Unmapped NRs pass through (identity) so cold path can ENOSYS cleanly.
 * Notable: i386 mmap2 (192) → mmap (9); path NRs (open/openat/stat family) mapped.
 * mmap2 page-offset vs mmap byte-offset conversion is not done here.
 */
int  wow64_translate_nr(u32 u32Nr32, u32 *pOutNr);
u32  wow64_calls(void);
