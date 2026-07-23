/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/netstackd.elf for boot-time live spawn.
 */
.section .rodata
.global gj_netstackd_elf_blob
.global gj_netstackd_elf_blob_end
.align 16
gj_netstackd_elf_blob:
    .incbin "build/user/netstackd.elf"
gj_netstackd_elf_blob_end:

.section .note.GNU-stack,"",@progbits
