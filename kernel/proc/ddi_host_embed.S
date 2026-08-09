/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/drivers/ddi_host for process_spawn_host_launch class 0.
 * Soft!=product Dual DoD OPEN; product = UDX/DDI userspace host path.
 */
.section .rodata
.global gj_ddi_host_elf_blob
.global gj_ddi_host_elf_blob_end
.align 16
gj_ddi_host_elf_blob:
    .incbin "build/user/drivers/ddi_host"
gj_ddi_host_elf_blob_end:

.section .note.GNU-stack,"",@progbits
