/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/shell.elf for boot-time live spawn.
 * Rebuild userland before kernel if shell changes: make userland && make
 */
.section .rodata
.global gj_shell_elf_blob
.global gj_shell_elf_blob_end
.align 16
gj_shell_elf_blob:
    .incbin "build/user/shell.elf"
gj_shell_elf_blob_end:

.section .note.GNU-stack,"",@progbits
