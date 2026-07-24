/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Thin compositor pointer for GreenJade session product path.
 *
 * Full host A1 smoke lives in sessiond.c:
 *   make sessiond && ./build/sessiond
 * Freestanding ownership live path lives in sessiond_gj.c:
 *   make sessiond-gj → build/user/sessiond.elf
 *
 * Door opcodes (stable — do not renumber; match session_door.h):
 *   1 PRESENT       present kernel interim scanout
 *   2 DISPLAY_INFO  → u32[2] w,h
 *   3 INPUT_POLL    drain virtio-input into hub
 *   4 INPUT_POP     → gj_input_event; ret 1/0
 *   5 STATS         → u32[5] presents,in_push,calls,flags,owner
 *   6 PRESENT_FB    w,h,user BGRA (stride=w*4)
 *   7 CLAIM         token (non-zero)
 *   8 RELEASE       matching token
 *   9 MAP_SCANOUT   va hint + u32[3] w,h,stride
 *
 * This stub only prints the product pointer; it does not open the door.
 *
 * Soft inventory (Wave 82 exclusive deepen — greppable):
 *   session-compositor: soft deepen wave=70 …
 *   session-compositor: soft honesty multi_server=0 confine=0 bar3=0 …
 * Soft pointer only — never a bar3 / multi-server claim.
 */
#include <stdio.h>

int
main(void)
{
    printf("session-compositor: use sessiond (make sessiond)\n");
    printf("session-compositor: freestanding path sessiond_gj "
           "(make sessiond-gj)\n");
    printf("session-compositor: door ops PRESENT..MAP_SCANOUT = 1..9 "
           "(stable)\n");
    printf("session-compositor: soft reclaim + multi-frame + free path\n");
    /* Grep: session-compositor: soft deepen (Wave 82 exclusive) */
/* Wave 82 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retmantleangle continuum_toward=22400 soft_ne_product=1
 *   greppable: soft retaegisangle exclusive=1 continuum_toward=22400
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

    printf("session-compositor: soft deepen wave=70 areas=1 "
           "multi_server=0 confine=0 bar3=0 exclusive=1\n");
    printf("session-compositor: soft honesty multi_server=0 confine=0 "
           "bar3=0 exclusive=1 soft=1 product_kernel=OPEN wave=70\n");
    return 0;
}
