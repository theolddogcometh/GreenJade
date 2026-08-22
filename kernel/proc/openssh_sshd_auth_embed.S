/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/openssh-dut/sshd-auth (OpenSSH-portable LINUX personality ELF).
 * Symbols: gj_openssh_sshd_auth_elf_blob / gj_openssh_sshd_auth_elf_blob_end
 * (not gj_sshd_elf_blob — that name is abandoned sshd_gj).
 * Not linked until Makefile S_SRCS lists this file; later agent wires
 * S_SRCS + execve special-case (vfs_ram 32KiB cannot hold these ELFs).
 * Product SSH = OpenSSH-portable LINUX; Dual DoD B OPEN; G-AC-1.
 * Stamp 0.1.184. Do not spawn here.
 */
.section .rodata
.global gj_openssh_sshd_auth_elf_blob
.global gj_openssh_sshd_auth_elf_blob_end
.align 16
gj_openssh_sshd_auth_elf_blob:
    .incbin "build/openssh-dut/sshd-auth"
gj_openssh_sshd_auth_elf_blob_end:

.section .note.GNU-stack,"",@progbits
