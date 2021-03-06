// simple_blend.c
// weed plugin
// (c) G. Finch (salsaman) 2005 - 2011
//
// released under the GNU GPL 3 or later
// see file COPYING or www.gnu.org for details

///////////////////////////////////////////////////////////////////

static int package_version = 1; // version of this package

//////////////////////////////////////////////////////////////////

#define NEED_PALETTE_CONVERSIONS
#define NEED_PALETTE_UTILS

#ifndef NEED_LOCAL_WEED_PLUGIN
#include <weed/weed-plugin.h>
#include <weed/weed-utils.h> // optional
#include <weed/weed-plugin-utils.h> // optional
#else
#include "../../libweed/weed-plugin.h"
#include "../../../libweed/weed-utils.h" // optional
#include "../../libweed/weed-plugin-utils.h" // optional
#endif

#include "weed-plugin-utils.c"

/////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _sdata {
  unsigned char obf;
  unsigned char blend[256][256];
  volatile weed_timecode_t master_tc;
} _sdata;


static void make_blend_table(_sdata *sdata, unsigned char bf, unsigned char bfn) {
  register int i, j;
  for (i = 0; i < 256; i++) {
    for (j = 0; j < 256; j++) {
      sdata->blend[i][j] = (unsigned char)((bf * i + bfn * j) >> 8);
    }
  }
}


/////////////////////////////////////////////////////////////////////////

static weed_error_t chroma_init(weed_plant_t *inst) {
  _sdata *sdata = weed_malloc(sizeof(_sdata));
  if (sdata == NULL) return WEED_ERROR_MEMORY_ALLOCATION;
  sdata->obf = 0;
  sdata->master_tc = -1;
  make_blend_table(sdata, 0, 255);
  weed_set_voidptr_value(inst, "plugin_internal", sdata);
  return WEED_SUCCESS;
}


static weed_error_t chroma_deinit(weed_plant_t *inst) {
  _sdata *sdata = weed_get_voidptr_value(inst, "plugin_internal", NULL);
  if (sdata != NULL) weed_free(sdata);
  return WEED_SUCCESS;
}


static weed_error_t common_process(int type, weed_plant_t *inst, weed_timecode_t timecode) {
  _sdata *sdata = NULL;

  weed_plant_t **in_channels = weed_get_in_channels(inst, NULL),
                 *out_channel = weed_get_out_channel(inst, 0);
  weed_plant_t *in_param;

  unsigned char *src1 = weed_channel_get_pixel_data(in_channels[0]);
  unsigned char *src2 = weed_channel_get_pixel_data(in_channels[1]);

  unsigned char *dst = weed_channel_get_pixel_data(out_channel);

  unsigned char blendneg;
  unsigned char blend_factor;

  int width = weed_channel_get_width(in_channels[0]);
  int height = weed_channel_get_height(in_channels[0]);
  int pal = weed_channel_get_palette(in_channels[0]);
  int irowstride1 = weed_channel_get_stride(in_channels[0]);
  int irowstride2 = weed_channel_get_stride(in_channels[1]);
  int orowstride = weed_channel_get_stride(out_channel);
  int inplace = (src1 == dst);
  int bf, psize = 4;
  int start = 0, row = 0;
  int offset = 0;

  unsigned char *end = src1 + height * irowstride1;

  register int j;

  if (pal == WEED_PALETTE_RGB24 || pal == WEED_PALETTE_BGR24) psize = 3;
  if (pal == WEED_PALETTE_ARGB32) start = 1;

  width *= psize;

  in_param = weed_get_in_param(inst, 0);
  bf = weed_param_get_value_int(in_param);

  blend_factor = (unsigned char)bf;
  blendneg = blend_factor ^ 0xFF;

  // new threading arch
  if (weed_plant_has_leaf(out_channel, WEED_LEAF_OFFSET)) {
    int dheight = weed_get_int_value(out_channel, WEED_LEAF_HEIGHT, NULL);
    offset = weed_get_int_value(out_channel, WEED_LEAF_OFFSET, NULL);
    src1 += offset * irowstride1;
    end = src1 + dheight * irowstride1;
    src2 += offset * irowstride2;
    dst += offset * orowstride;
  }

  if (type == 0) {
    sdata = (_sdata *)weed_get_voidptr_value(inst, "plugin_internal", NULL);
    if (offset == 0) {
      if (sdata->obf != blend_factor) {
        make_blend_table(sdata, blend_factor, blendneg);
        sdata->obf = blend_factor;
      }
      sdata->master_tc = timecode;
    } else {
      while (sdata->master_tc != timecode) {
        usleep(100);
      }
    }
  }

  for (; src1 < end; src1 += irowstride1) {
    for (j = start; j < width; j += psize) {
      switch (type) {
      case 0:
        // chroma blend
        dst[j] = sdata->blend[src2[j]][src1[j]];
        dst[j + 1] = sdata->blend[src2[j + 1]][src1[j + 1]];
        dst[j + 2] = sdata->blend[src2[j + 2]][src1[j + 2]];
        break;
      case 4:
        // avg luma overlay
        if (j > start && j < width - 1 && row > 0 && row < height - 1) {
          uint8_t av_luma = (calc_luma(&src1[j], pal, 0) + calc_luma(&src1[j - 1], pal, 0) + calc_luma(&src1[j + 1], pal,
                             0) + calc_luma(&src1[j - irowstride1], pal, 0) + calc_luma(&src1[j - 1 - irowstride1], pal,
                                 0) + calc_luma(&src1[j + 1 - irowstride1], pal,
                                                0) + calc_luma(&src1[j + irowstride1], pal, 0) + calc_luma(&src1[j - 1 + irowstride1], pal,
                                                    0) + calc_luma(&src1[j + 1 + irowstride1], pal,
                                                        0)) / 9;
          if (av_luma < (blend_factor)) weed_memcpy(&dst[j], &src2[j], 3);
          else if (!inplace) weed_memcpy(&dst[j], &src1[j], 3);
          row++;
          break;
        }
      case 1:
        // luma overlay
        if (calc_luma(&src1[j], pal, 0) < (blend_factor)) weed_memcpy(&dst[j], &src2[j], 3);
        else if (!inplace) weed_memcpy(&dst[j], &src1[j], 3);
        break;
      case 2:
        // luma underlay
        if (calc_luma(&src2[j], pal, 0) > (blendneg)) weed_memcpy(&dst[j], &src2[j], 3);
        else if (!inplace) weed_memcpy(&dst[j], &src1[j], 3);
        break;
      case 3:
        // neg lum overlay
        if (calc_luma(&src1[j], pal, 0) > (blendneg)) weed_memcpy(&dst[j], &src2[j], 3);
        else if (!inplace) weed_memcpy(&dst[j], &src1[j], 3);
        break;
      }
    }
    src2 += irowstride2;
    dst += orowstride;
  }
  weed_free(in_channels);
  return WEED_SUCCESS;
}


static weed_error_t chroma_process(weed_plant_t *inst, weed_timecode_t timestamp) {
  return common_process(0, inst, timestamp);
}


static weed_error_t lumo_process(weed_plant_t *inst, weed_timecode_t timestamp) {
  return common_process(1, inst, timestamp);
}


static weed_error_t lumu_process(weed_plant_t *inst, weed_timecode_t timestamp) {
  return common_process(2, inst, timestamp);
}


static weed_error_t nlumo_process(weed_plant_t *inst, weed_timecode_t timestamp) {
  return common_process(3, inst, timestamp);
}


static weed_error_t avlumo_process(weed_plant_t *inst, weed_timecode_t timestamp) {
  return common_process(4, inst, timestamp);
}


WEED_SETUP_START(200, 200) {
  weed_plant_t **clone1, **clone2, **clone3;
  int palette_list[] = ALL_RGB_PALETTES;
  weed_plant_t *in_chantmpls[] = {weed_channel_template_init("in channel 0", 0),
                                  weed_channel_template_init("in channel 1", 0), NULL
                                 };
  weed_plant_t *out_chantmpls[] = {weed_channel_template_init("out channel 0", WEED_CHANNEL_CAN_DO_INPLACE), NULL};
  weed_plant_t *in_params1[] = {weed_integer_init("amount", "Blend _amount", 128, 0, 255), NULL};
  weed_plant_t *in_params2[] = {weed_integer_init("threshold", "luma _threshold", 64, 0, 255), NULL};

  weed_plant_t *filter_class = weed_filter_class_init("chroma blend", "salsaman", 1,
                               WEED_FILTER_HINT_MAY_THREAD | WEED_FILTER_PREF_LINEAR_GAMMA, palette_list, chroma_init,
                               chroma_process, chroma_deinit, in_chantmpls, out_chantmpls, in_params1, NULL);

  weed_set_boolean_value(in_params1[0], WEED_LEAF_IS_TRANSITION, WEED_TRUE);
  weed_set_boolean_value(in_params2[0], WEED_LEAF_IS_TRANSITION, WEED_TRUE);

  weed_plugin_info_add_filter_class(plugin_info, filter_class);

  filter_class = weed_filter_class_init("luma overlay", "salsaman", 1,
                                        WEED_FILTER_HINT_MAY_THREAD, palette_list,
                                        NULL, lumo_process, NULL,
                                        (clone1 = weed_clone_plants(in_chantmpls)),
                                        (clone2 = weed_clone_plants(out_chantmpls)),
                                        in_params2, NULL);

  weed_plugin_info_add_filter_class(plugin_info, filter_class);
  weed_free(clone1);
  weed_free(clone2);

  filter_class = weed_filter_class_init("luma underlay", "salsaman", 1,
                                        WEED_FILTER_HINT_MAY_THREAD, palette_list,
                                        NULL, lumu_process, NULL,
                                        (clone1 = weed_clone_plants(in_chantmpls)),
                                        (clone2 = weed_clone_plants(out_chantmpls)),
                                        (clone3 = weed_clone_plants(in_params2)), NULL);

  weed_plugin_info_add_filter_class(plugin_info, filter_class);
  weed_free(clone1);
  weed_free(clone2);
  weed_free(clone3);

  filter_class = weed_filter_class_init("negative luma overlay", "salsaman", 1,
                                        WEED_FILTER_HINT_MAY_THREAD, palette_list,
                                        NULL, nlumo_process, NULL, (clone1 = weed_clone_plants(in_chantmpls)),
                                        (clone2 = weed_clone_plants(out_chantmpls)),
                                        (clone3 = weed_clone_plants(in_params2)), NULL);

  weed_plugin_info_add_filter_class(plugin_info, filter_class);
  weed_free(clone1);
  weed_free(clone2);
  weed_free(clone3);

  filter_class = weed_filter_class_init("averaged luma overlay", "salsaman", 1, WEED_FILTER_PREF_LINEAR_GAMMA, palette_list,
                                        NULL, &avlumo_process, NULL, (clone1 = weed_clone_plants(in_chantmpls)),
                                        (clone2 = weed_clone_plants(out_chantmpls)),
                                        (clone3 = weed_clone_plants(in_params2)), NULL);

  weed_plugin_info_add_filter_class(plugin_info, filter_class);
  weed_free(clone1);
  weed_free(clone2);
  weed_free(clone3);

  weed_set_int_value(plugin_info, WEED_LEAF_VERSION, package_version);
}
WEED_SETUP_END;
