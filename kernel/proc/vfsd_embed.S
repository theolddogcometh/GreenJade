/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/vfsd.elf for boot-time live spawn.
 */
.section .rodata
.global gj_vfsd_elf_blob
.global gj_vfsd_elf_blob_end
.align 16
gj_vfsd_elf_blob:
    .incbin "build/user/vfsd.elf"
gj_vfsd_elf_blob_end:

.section .note.GNU-stack,"",@progbits
