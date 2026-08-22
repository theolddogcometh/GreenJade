/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Abandoned: was embed of sshd_gj.elf for NATIVE :22 spawn.
 * Not linked. Product SSH is OpenSSH-portable (LINUX ABI), not this blob.
 */
.section .rodata
.global gj_sshd_elf_blob
.global gj_sshd_elf_blob_end
.align 16
gj_sshd_elf_blob:
    .incbin "build/user/sshd.elf"
gj_sshd_elf_blob_end:

.section .note.GNU-stack,"",@progbits
