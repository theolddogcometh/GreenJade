/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Embed build/user/init.elf for boot-time load smoke.
 * Rebuild userland before kernel if init changes: make userland && make
 */
.section .rodata
.global gj_init_elf_blob
.global gj_init_elf_blob_end
.align 16
gj_init_elf_blob:
    .incbin "build/user/init.elf"
gj_init_elf_blob_end:

.section .note.GNU-stack,"",@progbits
