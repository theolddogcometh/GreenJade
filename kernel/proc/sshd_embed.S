/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/sshd.elf for boot-time live spawn (sshd on by default).
 */
.section .rodata
.global gj_sshd_elf_blob
.global gj_sshd_elf_blob_end
.align 16
gj_sshd_elf_blob:
    .incbin "build/user/sshd.elf"
gj_sshd_elf_blob_end:

.section .note.GNU-stack,"",@progbits
