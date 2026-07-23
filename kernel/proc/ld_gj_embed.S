/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/ld-gj.so.1 for product INTERP path when vfs has only
 * a placeholder. Rebuild: make userland && make
 */
.section .rodata
.global gj_ld_gj_elf_blob
.global gj_ld_gj_elf_blob_end
.align 16
gj_ld_gj_elf_blob:
    .incbin "build/user/ld-gj.so.1"
gj_ld_gj_elf_blob_end:

.section .note.GNU-stack,"",@progbits
