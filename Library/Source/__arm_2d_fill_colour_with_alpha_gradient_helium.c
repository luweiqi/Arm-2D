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
 * $Date:        14. Aug 2024
 * $Revision:    V.1.1.0
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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/


/*---------------------------------------------------------------------------*
 * Colour Filling with 4 sample points Alpha Gradient and Mask               *
 *---------------------------------------------------------------------------*/


__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient_and_mask)(
                        uint8_t *__RESTRICT pchTarget, 
                        int16_t iTargetStride, 
                        uint8_t *__RESTRICT pchMask,
                        int16_t iMaskStride, 
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen, 
                        uint8_t chColour,
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

    int32_t q16YRatioLeft, q16YRatioRight;
    
    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (   (   (int32_t)(tSamplePoints.chBottomLeft 
                            -   tSamplePoints.chTopLeft)) 
                            << 16)
                        / iHeight;

        q16YRatioRight =(   (   (int32_t)(tSamplePoints.chBottomRight 
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
    
        uint8_t *__RESTRICT pchTargetLine = pchTarget;

        do {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            mve_pred16_t tailPred = vctp16q(blkCnt);

        
            uint16x8_t vMask    = vldrbq_z_u16(pchMaskLine, tailPred);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);
        

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            vstrbq_p_u16(pchTargetLine,
                    __arm_2d_blend_gray8(   vldrbq_z_u16(pchTargetLine, tailPred),
                                            chColour, 
                                            vhwAlpha),
                    tailPred);

            vev += 8;
            vodd += 8;

        
            pchMaskLine += 8;
        

            pchTargetLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchTarget += iTargetStride;
    
        pchMask += iMaskStride;
    
    }
}


__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient_and_chn_mask)(
                        uint8_t *__RESTRICT pchTarget,
                        int16_t iTargetStride,
                        uint32_t *__RESTRICT pwMask,
                        int16_t iMaskStride,
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen,
                        uint8_t chColour,
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
        uint8_t *__RESTRICT pchTargetLine = pchTarget;
        uint16x8_t      vStride4Offs = vidupq_n_u16(0, 4);

        do
        {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
                           
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t vMask = vldrbq_gather_offset_z_u16(  pchMaskLine, 
                                                            vStride4Offs,
                                                            tailPred);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vstrbq_p_u16(pchTargetLine,
                    __arm_2d_blend_gray8(   vldrbq_z_u16(pchTargetLine, tailPred),
                                            chColour, 
                                            vhwAlpha),
                    tailPred);

            vev += 8;
            vodd += 8;

            pchMaskLine += 8*4;
            pchTargetLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}



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

            mve_pred16_t    tailPred = vctp16q(blkCnt);

        
            uint16x8_t vMask    = vldrbq_z_u16(pchMaskLine, tailPred);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);
        

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(  vldrhq_z_u16(phwTargetLine, tailPred),
                                            &ColorRGB, 
                                            vhwAlpha),
                    tailPred);

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

            mve_pred16_t    tailPred = vctp16q(blkCnt);
            uint16x8_t vMask = vldrbq_gather_offset_z_u16(pchMaskLine, 
                                                        vStride4Offs,
                                                        tailPred);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);


#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(  vldrhq_z_u16(phwTargetLine, tailPred),
                                            &ColorRGB, 
                                            vhwAlpha),
                    tailPred);

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


__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient_and_mask)(
                        uint32_t *__RESTRICT pwTarget, 
                        int16_t iTargetStride, 
                        uint8_t *__RESTRICT pchMask,
                        int16_t iMaskStride, 
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen, 
                        uint32_t wColour,
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

    uint16x8_t vColourRGB = vreinterpretq_u16(vdupq_n_u32(wColour));
    
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
        uint32x4_t vev                     = {0, 0, 1, 1};//vidupq_n_u32(0, 2);
        uint32x4_t vodd                    = {0, 0, 1, 1};//vidupq_n_u32(1, 2);
    
        uint8_t *__RESTRICT pchMaskLine    = pchMask;
    
        uint32_t *__RESTRICT pwTargetLine = pwTarget;

        do {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            

            mve_pred16_t    tailPred = vctp64q(blkCnt);
        
        
            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            static const uint16x8_t c_vOffset = { 0, 0, 0, 0, 1, 1, 1, 1 };
            uint16x8_t vMask = vldrbq_gather_offset_z_u16(pchMaskLine, c_vOffset, (0x3f3f & tailPred));
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);
        

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888( 
                                vldrbq_z_u16((uint8_t *)pwTargetLine,tailPred),
                                vColourRGB, 
                                vhwAlpha),
                            tailPred);

            vev += 2;
            vodd += 2;
        
            pchMaskLine += 2;
        
            pwTargetLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pwTarget += iTargetStride;
    
        pchMask += iMaskStride;
    
    }
}


__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient_and_chn_mask)(
                        uint32_t *__RESTRICT pwTarget, 
                        int16_t iTargetStride, 
                        uint32_t *__RESTRICT pwMask,
                        int16_t iMaskStride, 
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen, 
                        uint32_t wColour,
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

    uint16x8_t vColourRGB = vreinterpretq_u16(vdupq_n_u32(wColour));
    
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
        uint32x4_t vev                     = {0, 0, 1, 1};//vidupq_n_u32(0, 2);
        uint32x4_t vodd                    = {0, 0, 1, 1};//vidupq_n_u32(1, 2);
        uint8_t  *__RESTRICT pchMaskLine = (uint8_t *__RESTRICT)pwMask;
        uint32_t *__RESTRICT pwTargetLine = pwTarget;

        do {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            

            mve_pred16_t    tailPred = vctp64q(blkCnt);

            /*
               replicate alpha, but alpha location = 0 (zeroing) so that transparency = 0x100
               and leaves target 0 unchanged
               vSrcOpa = | opa0 | opa0 | opa0 |  0  | opa1 | opa1 | opa1 |  0  |
             */
            static const uint16x8_t c_vOffset = { 0, 0, 0, 0, 4, 4, 4, 4 };
            uint16x8_t vMask = vldrbq_gather_offset_z_u16(pchMaskLine, c_vOffset, (0x3f3f & tailPred));
            
            
            //uint16x8_t vMask    = vldrbq_z_u16(pchMaskLine, tailPred);
            uint16x8_t vhwAlpha = ((vMask * vOpacity) >> 8);

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888( 
                                vldrbq_z_u16((uint8_t *)pwTargetLine,tailPred),
                                vColourRGB, 
                                vhwAlpha),
                            tailPred);

            vev += 2;
            vodd += 2;

            pchMaskLine += 2 * 4;
            pwTargetLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pwTarget += iTargetStride;
        pwMask += iMaskStride;
    }
}



/*---------------------------------------------------------------------------*
 * Colour Filling with 4 sample points Alpha Gradient                        *
 *---------------------------------------------------------------------------*/


__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_gray8_fill_colour_with_4pts_alpha_gradient)(
                        uint8_t *__RESTRICT pchTarget, 
                        int16_t iTargetStride, 
                        int16_t iMaskStride, 
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen, 
                        uint8_t chColour,
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

    int32_t q16YRatioLeft, q16YRatioRight;
    
    /* calculate Y Ratios */
    do {
        int16_t iHeight = ptTargetRegionOnVirtualScreen->tSize.iHeight;

        q16YRatioLeft = (   (   (int32_t)(tSamplePoints.chBottomLeft 
                            -   tSamplePoints.chTopLeft)) 
                            << 16)
                        / iHeight;

        q16YRatioRight =(   (   (int32_t)(tSamplePoints.chBottomRight 
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
    
        uint8_t *__RESTRICT pchTargetLine = pchTarget;

        do {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            mve_pred16_t tailPred = vctp16q(blkCnt);

        
            uint16x8_t vhwAlpha = vOpacity;
        

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            vstrbq_p_u16(pchTargetLine,
                    __arm_2d_blend_gray8(   vldrbq_z_u16(pchTargetLine, tailPred),
                                            chColour, 
                                            vhwAlpha),
                    tailPred);

            vev += 8;
            vodd += 8;

        

            pchTargetLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        pchTarget += iTargetStride;
    
    }
}




__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_rgb565_fill_colour_with_4pts_alpha_gradient)(
                        uint16_t *__RESTRICT phwTarget, 
                        int16_t iTargetStride, 
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
    
        uint16_t *__RESTRICT phwTargetLine = phwTarget;

        do {
            
        
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            mve_pred16_t    tailPred = vctp16q(blkCnt);

        
            uint16x8_t vhwAlpha = vOpacity;
        

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vst1q_p(phwTargetLine,
                    __arm_2d_blend_rgb565(  vldrhq_z_u16(phwTargetLine, tailPred),
                                            &ColorRGB, 
                                            vhwAlpha),
                    tailPred);

            vev += 8;
            vodd += 8;
        
            phwTargetLine += 8;
            blkCnt -= 8;
        } while (blkCnt > 0);

        phwTarget += iTargetStride;
    
    }
}



__OVERRIDE_WEAK
void 
__MVE_WRAPPER(
    __arm_2d_impl_cccn888_fill_colour_with_4pts_alpha_gradient)(
                        uint32_t *__RESTRICT pwTarget, 
                        int16_t iTargetStride, 
                        int16_t iMaskStride, 
                        arm_2d_region_t *__RESTRICT ptValidRegionOnVirtualScreen,
                        arm_2d_region_t *ptTargetRegionOnVirtualScreen, 
                        uint32_t wColour,
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

    uint16x8_t vColourRGB = vreinterpretq_u16(vdupq_n_u32(wColour));
    
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
        uint32x4_t vev                     = {0, 0, 1, 1};//vidupq_n_u32(0, 2);
        uint32x4_t vodd                    = {0, 0, 1, 1};//vidupq_n_u32(1, 2);
    
        uint32_t *__RESTRICT pwTargetLine = pwTarget;

        do {
            uint32x4_t vxe = (vev + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;
            uint32x4_t vxo = (vodd + (uint32_t)tOffset.iX) * (uint32_t)q16XRatio 
                           + q16OpacityLeft;

            uint16x8_t vOpacity = vqshrnbq_n_u32(vuninitializedq_u16(), vxe, 16);
            vOpacity            = vqshrntq_n_u32(vOpacity, vxo, 16);

            

            mve_pred16_t    tailPred = vctp64q(blkCnt);
        
        
            uint16x8_t vhwAlpha = vOpacity;
        

#if !defined(__ARM_2D_CFG_UNSAFE_IGNORE_ALPHA_255_COMPENSATION__)
            vhwAlpha = vpselq(  vdupq_n_u16(256), 
                                vhwAlpha, 
                                vcmpeqq_n_u16(vhwAlpha, 255));
#endif
            vhwAlpha = 256 - vhwAlpha;

            
            vstrbq_p_u16(   (uint8_t*)pwTargetLine,
                            __arm_2d_blend_cccn888( 
                                vldrbq_z_u16((uint8_t *)pwTargetLine,tailPred),
                                vColourRGB, 
                                vhwAlpha),
                            tailPred);

            vev += 2;
            vodd += 2;
        
            pwTargetLine += 2;
            blkCnt -= 2;
        } while (blkCnt > 0);

        pwTarget += iTargetStride;
    
    }
}



#ifdef __cplusplus
}
#endif

#endif /* __ARM_2D_COMPILATION_UNIT */