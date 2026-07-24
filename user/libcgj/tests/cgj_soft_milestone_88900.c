/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88900 \
 *   user/libcgj/tests/cgj_soft_milestone_88900.c -ldl
 * Direct: -DCGJ_SOFT_M88900_DIRECT + graph_batch88891.c … graph_batch88900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88900
#define CGJ_SOFT_B0 88891
#define CGJ_SOFT_B1 88892
#define CGJ_SOFT_B2 88893
#define CGJ_SOFT_B3 88894
#define CGJ_SOFT_B4 88895
#define CGJ_SOFT_B5 88896
#define CGJ_SOFT_B6 88897
#define CGJ_SOFT_B7 88898
#define CGJ_SOFT_B8 88899
#include "_cgj_soft_m_template.inc"
