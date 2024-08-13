/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        __arm_2d_fill_colour_with_alpha_gradient_and_mask_helium.c
 * Description:  The source code of APIs for colour-filling-with-alpha-gradient
 *               -and-mask
 *
 * $Date:        6. Aug 2024
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores with helium
 *
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wempty-translation-unit"
#endif

#ifdef __ARM_2D_COMPILATION_UNIT
#undef __ARM_2D_COMPILATION_UNIT

#define __ARM_2D_IMPL__

#include "__arm_2d_impl.h"
#include "arm_2d.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#elif defined(__IS_COMPILER_ARM_COMPILER_5__)
#pragma diag_suppress 174, 177, 188, 68, 513, 144
#elif defined(__IS_COMPILER_GCC__)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/

/* /!\ Scratch memory dimenensions must be mutiple of for Helium */
#define ROUND_UP_8(x) (((x) + 7) & ~7)

    /*============================ MACROFIED FUNCTIONS ===========================*/
    /*============================ TYPES =========================================*/
    /*============================ GLOBAL VARIABLES ==============================*/
    /*============================ PROTOTYPES ====================================*/
    /*============================ LOCAL VARIABLES ===============================*/
    /*============================ IMPLEMENTATION ================================*/



__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient_and_mask)(
                        uint16_t *__RESTRICT phwTarget, 
                        int16_t iTargetStride, 
                        uint8_t *__RESTRICT pchMask,
                        int16_t iMaskStride, 
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen, 
                        uint16_t hwColour,
                        arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth  = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX 
            - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY 
            - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    int32_t q16YRatioLeft, q16YRatioRight;
    
    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (   (   (int32_t)(tSamplePoints.chBottomLeft 
                            -   tSamplePoints.chTopLeft)) 
                            << 16)
                        / iHeight;

        q16YRatioRight =    (   (   (int32_t)(tSamplePoints.chBottomRight 
                                -   tSamplePoints.chTopRight)) 
                                << 16) 
                       / iHeight;
    } while (0);


    uint32_t invWidth = 0xffffffffUL / (uint32_t)iWidth;
    int32_t TopDiff = ( (   (int32_t)tSamplePoints.chTopRight 
                        -   (int32_t)tSamplePoints.chTopLeft) 
                      << 16);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16)
                               + (y + tOffset.iY) * q16YRatioLeft;

        int32_t q16XRatio;

        /* calculate X Ratios */
        int32_t num = (TopDiff + (y + tOffset.iY) 
                    * (q16YRatioRight - q16YRatioLeft));
        q16XRatio   = ((q63_t)num * (int32_t)invWidth + 0x80000000LL) >> 32;


        int32_t blkCnt                     = iWidth;
        /* even / odd generators for 32-bit widened Opacity computation */
        /* This one will be narrowed in a 16-bit vector */
        uint32x4_t vev                     = vidupq_n_u32(0, 2);
        uint32x4_t vodd                    = vidupq_n_u32(1, 2);
        uint8_t *__RESTRICT pchMaskLine    = pchMask;
        uint16_t *__RESTRICT phwTargetLine = phwTarget;

        do {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            uint16x8_t vMask    = vldrbq_u16(pchMaskLine);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(  vld1q(phwTargetLine),
                                            &ColorRGB, 
                                            vhwAlpha),
                    vctp16q(blkCnt));

            vev += 8;
            vodd += 8;

            pchMaskLine += 8;
            phwTargetLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        phwTarget += iTargetStride;
        pchMask += iMaskStride;
    }
}


__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient_and_chn_mask)(
                        uint16_t *__RESTRICT phwTarget,
                        int16_t iTargetStride,
                        uint32_t *__RESTRICT pwMask,
                        int16_t iMaskStride,
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                        uint16_t hwColour,
                        arm_2d_alpha_samples_4pts_t tSamplePoints)
{
    int_fast16_t iWidth  = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset = {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX 
            - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY 
            - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    __arm_2d_color_fast_rgb_t ColorRGB;
    __arm_2d_rgb565_unpack(hwColour, &ColorRGB);

    int32_t q16YRatioLeft, q16YRatioRight;

    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (   (   (int32_t)(tSamplePoints.chBottomLeft 
                            -   tSamplePoints.chTopLeft)) 
                            << 16) 
                      / iHeight;

        q16YRatioRight = (  (   (int32_t)(tSamplePoints.chBottomRight 
                            -   tSamplePoints.chTopRight)) 
                            << 16) 
                       / iHeight;
    } while (0);


    uint32_t invWidth = 0xffffffffUL / (uint32_t)iWidth;
    int32_t TopDiff = ( (   (int32_t)tSamplePoints.chTopRight 
                        -   (int32_t)tSamplePoints.chTopLeft) 
                        << 16);

    for (int_fast16_t y = 0; y < iHeight; y++) {

        /* calculate the end points of the current row */
        int32_t q16OpacityLeft = (((int32_t)tSamplePoints.chTopLeft) << 16) 
                               + (y + tOffset.iY) * q16YRatioLeft;

        int32_t q16XRatio;

        /* calculate X Ratios */
        int32_t num = (TopDiff + (y + tOffset.iY) 
                    * (q16YRatioRight - q16YRatioLeft));
        q16XRatio   = ((q63_t)num * (int32_t)invWidth + 0x80000000LL) >> 32;


        int32_t blkCnt                     = iWidth;
        /* even / odd generators for 32-bit widened Opacity computation */
        /* This one will be narrowed in a 16-bit vector */
        uint32x4_t vev                     = vidupq_n_u32(0, 2);
        uint32x4_t vodd                    = vidupq_n_u32(1, 2);
        uint8_t  *__RESTRICT pchMaskLine = ( uint8_t *__RESTRICT)pwMask;
        uint16_t *__RESTRICT phwTargetLine = phwTarget;
        uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);

        do
        {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
                           
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            uint16x8_t vMask = vldrbq_gather_offset_u16(pchMaskLine, 
                                                        vStride4Offs);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            vst1q_p(phwTargetLine,
                __arm_2d_blend_rgb565(  vld1q(phwTargetLine),
                                        &ColorRGB, 
                                        vhwAlpha),
                vctp16q(blkCnt));

            vev += 8;
            vodd += 8;

            pchMaskLine += 8*4;
            phwTargetLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        phwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */