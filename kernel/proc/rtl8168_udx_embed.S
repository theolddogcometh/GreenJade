/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/drivers/rtl8168_udx for process_spawn_host_launch class 1
 * (Dual DoD B NIC host 10ec:8168). Soft!=product; freestanding class SKIP.
 * Product Own handoff + L2 thr-poll residual (.84 large DMA tier).
 */
.section .rodata
.global gj_rtl8168_udx_elf_blob
.global gj_rtl8168_udx_elf_blob_end
.align 16
gj_rtl8168_udx_elf_blob:
    .incbin "build/user/drivers/rtl8168_udx"
gj_rtl8168_udx_elf_blob_end:

.section .note.GNU-stack,"",@progbits
