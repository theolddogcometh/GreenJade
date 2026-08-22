/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/dash.elf as product /bin/sh (vendored dash + libcgj).
 * Symbol names stay gj_shell_elf_blob (boot live-spawn + execve /bin/sh).
 * Rebuild: make dash && make
 */
.section .rodata
.global gj_shell_elf_blob
.global gj_shell_elf_blob_end
.align 16
gj_shell_elf_blob:
    .incbin "build/user/dash.elf"
gj_shell_elf_blob_end:

.section .note.GNU-stack,"",@progbits
