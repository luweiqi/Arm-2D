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

/*============================ INCLUDES ======================================*/

#define __USER_SCENE_MONO_TRACKING_LIST_IMPLEMENT__
#include "arm_2d_scene_mono_tracking_list.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB)

#include <stdlib.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wunused-function"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion" 
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 64,177
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe177,Pe174
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wformat="
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileCMSISLogo          c_tileCMSISLogoGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileCMSISLogo          c_tileCMSISLogoRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileCMSISLogo          c_tileCMSISLogoCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#undef this
#define this (*ptThis)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern
const
struct {
    implement(arm_2d_user_font_t);
    arm_2d_char_idx_t tUTF8Table;
}   ARM_2D_FONT_Arial14_A8,
    ARM_2D_FONT_Arial14_A4,
    ARM_2D_FONT_Arial14_A2,
    ARM_2D_FONT_Arial14_A1;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! define dirty regions */
IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions, static)

    /* a dirty region to be specified at runtime*/
    ADD_REGION_TO_LIST(s_tDirtyRegions,
        0  /* initialize at runtime later */
    ),
    
    /* add the last region:
        * it is the top left corner for text display 
        */
    ADD_LAST_REGION_TO_LIST(s_tDirtyRegions,
        .tLocation = {
            .iX = 0,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 0,
            .iHeight = 8,
        },
    ),

END_IMPL_ARM_2D_REGION_LIST(s_tDirtyRegions)

/*============================ IMPLEMENTATION ================================*/

static void __on_scene_mono_tracking_list_load(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __after_scene_mono_tracking_list_switching(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_tracking_list_depose(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    ptScene->ptPlayer = NULL;
    
    arm_foreach(int64_t,this.lTimestamp, ptItem) {
        *ptItem = 0;
    }

    text_tracking_list_depose(&this.tList);

    if (!this.bUserAllocated) {
        __arm_2d_free_scratch_memory(ARM_2D_MEM_TYPE_UNSPECIFIED, ptScene);
    }
}

/*----------------------------------------------------------------------------*
 * Scene mono_tracking_list                                                            *
 *----------------------------------------------------------------------------*/

static void __on_scene_mono_tracking_list_background_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static void __on_scene_mono_tracking_list_background_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}


static void __on_scene_mono_tracking_list_frame_start(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

    do {
        /* generate a new position every 2000 sec */
        if (arm_2d_helper_is_time_out(1000,  &this.lTimestamp[1])) {
            int32_t nSteps = 0;
            this.lTimestamp[1] = 0;
            srand(arm_2d_helper_get_system_timestamp());
            nSteps = rand() % 10;

            text_tracking_list_move_selection(&this.tList, nSteps - 5, 200);
        }
    } while(0);

    text_tracking_list_on_frame_start(&this.tList);
}

static void __on_scene_mono_tracking_list_frame_complete(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

#if 0
    /* switch to next scene after 7s */
    if (arm_2d_helper_is_time_out(7000, &this.lTimestamp[0])) {
        arm_2d_scene_player_switch_to_next_scene(ptScene->ptPlayer);
    }
#endif
}

static void __before_scene_mono_tracking_list_switching_out(arm_2d_scene_t *ptScene)
{
    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)ptScene;
    ARM_2D_UNUSED(ptThis);

}

static
IMPL_PFB_ON_DRAW(__pfb_draw_scene_mono_tracking_list_handler)
{
    ARM_2D_PARAM(pTarget);
    ARM_2D_PARAM(ptTile);
    ARM_2D_PARAM(bIsNewFrame);

    user_scene_mono_tracking_list_t *ptThis = (user_scene_mono_tracking_list_t *)pTarget;
    arm_2d_size_t tScreenSize = ptTile->tRegion.tSize;
    ARM_2D_UNUSED(tScreenSize);

    arm_2d_canvas(ptTile, __top_canvas) {
        arm_2d_layout(__top_canvas) {

            __item_line_dock_vertical(10) {

                arm_2d_fill_colour(ptTile, &__item_region, GLCD_COLOR_WHITE);

                /* print label */
                arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)ptTile);
                arm_lcd_text_set_font(&ARM_2D_FONT_6x8.use_as__arm_2d_font_t);
                arm_lcd_text_set_draw_region(&__item_region);
                arm_lcd_text_set_display_mode(ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR);
                
                arm_lcd_printf_label(ARM_2D_ALIGN_MIDDLE_LEFT, " Month");
                arm_lcd_printf_label(ARM_2D_ALIGN_MIDDLE_RIGHT, "_x");
            }

            __item_line_dock_vertical(4,    /* left margin */
                                      4,    /* right margin */
                                      2,    /* top margin */
                                      2) {  /* bottom margin */

                /* draw text list */
                arm_2d_dock_with_margin(__item_region, 
                                        0,  /* left margin */
                                        0,  /* right margin */
                                        0,  /* top margin */
                                        0) {/* bottom margin */

                    arm_lcd_text_set_display_mode(ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR);
                    while(arm_fsm_rt_cpl != text_tracking_list_show( &this.tList, 
                                                    ptTile, 
                                                    &__dock_region, 
                                                    bIsNewFrame));
                }
            }
        }
    }
    ARM_2D_OP_WAIT_ASYNC();

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
user_scene_mono_tracking_list_t *__arm_2d_scene_mono_tracking_list_init(
                                        arm_2d_scene_player_t *ptDispAdapter, 
                                        user_scene_mono_tracking_list_t *ptThis)
{
    bool bUserAllocated = false;
    assert(NULL != ptDispAdapter);

    if (NULL == ptThis) {
        ptThis = (user_scene_mono_tracking_list_t *)
                    __arm_2d_allocate_scratch_memory(   sizeof(user_scene_mono_tracking_list_t),
                                                        __alignof__(user_scene_mono_tracking_list_t),
                                                        ARM_2D_MEM_TYPE_UNSPECIFIED);
        assert(NULL != ptThis);
        if (NULL == ptThis) {
            return NULL;
        }
    } else {
        bUserAllocated = true;
    }

    memset(ptThis, 0, sizeof(user_scene_mono_tracking_list_t));

    *ptThis = (user_scene_mono_tracking_list_t){
        .use_as__arm_2d_scene_t = {

            /* the canvas colour */
            .tCanvas = {GLCD_COLOR_BLACK}, 

            /* Please uncommon the callbacks if you need them
             */
            .fnOnLoad       = &__on_scene_mono_tracking_list_load,
            .fnScene        = &__pfb_draw_scene_mono_tracking_list_handler,
            //.fnAfterSwitch  = &__after_scene_mono_tracking_list_switching,

            /* if you want to use predefined dirty region list, please uncomment the following code */
            //.ptDirtyRegion  = (arm_2d_region_list_item_t *)s_tDirtyRegions,
            

            //.fnOnBGStart    = &__on_scene_mono_tracking_list_background_start,
            //.fnOnBGComplete = &__on_scene_mono_tracking_list_background_complete,
            .fnOnFrameStart = &__on_scene_mono_tracking_list_frame_start,
            //.fnBeforeSwitchOut = &__before_scene_mono_tracking_list_switching_out,
            .fnOnFrameCPL   = &__on_scene_mono_tracking_list_frame_complete,
            .fnDepose       = &__on_scene_mono_tracking_list_depose,

            .bUseDirtyRegionHelper = true,
        },
        .bUserAllocated = bUserAllocated,
    };

    /* ------------   initialize members of user_scene_mono_tracking_list_t begin ---------------*/

    /* initialize text list */
    do {
        const static __disp_string_t c_strListItemStrings[] = {
            "  1 January", "  2 February", "  3 March", "  4 April", "  5 May", "  6 June", 
            "  7 July", "  8 August", "  9 September", " 10 October", " 11 November", " 12 December",
        };

        text_tracking_list_cfg_t tCFG = {
            //.tSettings = {
                .bUsePIMode = true,
                .bIndicatorAutoSize = true,
                .IndicatorColour.tValue = GLCD_COLOR_WHITE,
                .ScrollingBar.tValue = GLCD_COLOR_WHITE,
                .chScrollingBarAutoDisappearTimeX100Ms = 10,
                .bUseMonochromeMode = true,
                //.bDisableScrollingBar = true,
            //},

            .use_as__text_list_cfg_t = {
                .ptStrings = (__disp_string_t *)c_strListItemStrings,

                .use_as____simple_list_cfg_t = {
                    .hwCount = dimof(c_strListItemStrings),
                    
                    .tFontColour = GLCD_COLOR_WHITE,
                    .tBackgroundColour = GLCD_COLOR_BLACK,
                    //.bIgnoreBackground = true,
                    
                    //.bDisableRingMode = true,     /* you can disable the list ring mode here */

                    .chNextPadding = 1,
                    .chPreviousPadding = 1,
                    .tListSize = {
                        .iHeight = 0,           /* automatically set the height */
                        .iWidth = 0,            /* automatically set the width */
                    },
                    .tTextAlignment = ARM_2D_ALIGN_MIDDLE_LEFT,

                    .ptFont = (arm_2d_font_t *)&ARM_2D_FONT_6x8,

                    .bUseDirtyRegion = true,
                    .ptTargetScene = &this.use_as__arm_2d_scene_t,
                },
            },
        };
        text_tracking_list_init( 
            &this.tList, 
            &tCFG,
            &ARM_2D_LIST_CALCULATOR_NORMAL_FIXED_SIZED_ITEM_NO_STATUS_CHECK_VERTICAL);

        //text_tracking_list_move_selection(&this.tList, 0, 0);

    } while(0);

    /* ------------   initialize members of user_scene_mono_tracking_list_t end   ---------------*/

    arm_2d_scene_player_append_scenes(  ptDispAdapter, 
                                        &this.use_as__arm_2d_scene_t, 
                                        1);

    return ptThis;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif

