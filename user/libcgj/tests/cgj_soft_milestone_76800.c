/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76800 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_76800.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76800 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76800 \
 *   user/libcgj/tests/cgj_soft_milestone_76800.c -ldl
 * Direct: -DCGJ_SOFT_M76800_DIRECT + graph_batch76791.c … graph_batch76800.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M76800_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 76800
#define CGJ_SOFT_B0 76791
#define CGJ_SOFT_B1 76792
#define CGJ_SOFT_B2 76793
#define CGJ_SOFT_B3 76794
#define CGJ_SOFT_B4 76795
#define CGJ_SOFT_B5 76796
#define CGJ_SOFT_B6 76797
#define CGJ_SOFT_B7 76798
#define CGJ_SOFT_B8 76799
#include "_cgj_soft_m_template.inc"
