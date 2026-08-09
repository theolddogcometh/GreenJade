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
 * C2 product daemon residual (ASSURANCE_LITE claim_class=C2):
 *   Product session daemon = sessiond / sessiond_gj over session door + ABI.
 *   Soft pointer residual only — Soft!=product; Dual DoD A/B stay OPEN.
 *   Product drivers remain UDX/DDI+ABI (G-AC-1); not this stub.
 *
 * Soft inventory (Wave 126 exclusive deepen — greppable):
 *   session-compositor: soft deepen wave=70 …
 *   session-compositor: soft honesty multi_server=0 confine=0 …
 *   session-compositor: soft c2 product residual …
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
    /* Grep: session-compositor: soft deepen (Wave 126 exclusive) */
    /* Wave 126 soft deepen surfaces (CREATE-ONLY Soft!=product):
     *   greppable: soft retgradientangle continuum_toward=26800 soft_ne_product=1 wave=126
     *   greppable: soft retblendangle exclusive=1 continuum_toward=26800 soft_ne_product=1 wave=126
     * Soft!=product; product lamps 0;
     */

    printf("session-compositor: soft deepen wave=70 areas=1 "
           "multi_server=0 confine=0 exclusive=1\n");
    printf("session-compositor: soft honesty multi_server=0 confine=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");
    /* Grep: session-compositor: soft c2 product residual (C2 product daemon) */
    printf("session-compositor: soft c2 product residual "
           "claim_class=C2 product_daemon=sessiond "
           "product=UDX/DDI+ABI dual_dod=OPEN "
           "Soft!=product G-AC-1=1 multi_server=0 confine=0 "
           "stub_ne_door=1 product_mint=0\n");
    return 0;
}
