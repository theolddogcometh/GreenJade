/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/sessiond.elf for boot-time live sessiond spawn.
 * Rebuild: make sessiond-gj && make
 */
.section .rodata
.global gj_sessiond_elf_blob
.global gj_sessiond_elf_blob_end
.align 16
gj_sessiond_elf_blob:
    .incbin "build/user/sessiond.elf"
gj_sessiond_elf_blob_end:

.section .note.GNU-stack,"",@progbits
