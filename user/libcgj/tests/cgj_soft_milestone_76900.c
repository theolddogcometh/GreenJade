/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_76900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76900 \
 *   user/libcgj/tests/cgj_soft_milestone_76900.c -ldl
 * Direct: -DCGJ_SOFT_M76900_DIRECT + graph_batch76891.c … graph_batch76900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M76900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 76900
#define CGJ_SOFT_B0 76891
#define CGJ_SOFT_B1 76892
#define CGJ_SOFT_B2 76893
#define CGJ_SOFT_B3 76894
#define CGJ_SOFT_B4 76895
#define CGJ_SOFT_B5 76896
#define CGJ_SOFT_B6 76897
#define CGJ_SOFT_B7 76898
#define CGJ_SOFT_B8 76899
#include "_cgj_soft_m_template.inc"
