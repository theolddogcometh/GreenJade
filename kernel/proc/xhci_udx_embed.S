/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/drivers/xhci_udx for process_spawn_host_launch class 2
 * (Dual DoD A USB HC host 8086:a12f). Soft!=product; freestanding MSC SKIP.
 */
.section .rodata
.global gj_xhci_udx_elf_blob
.global gj_xhci_udx_elf_blob_end
.align 16
gj_xhci_udx_elf_blob:
    .incbin "build/user/drivers/xhci_udx"
gj_xhci_udx_elf_blob_end:

.section .note.GNU-stack,"",@progbits
