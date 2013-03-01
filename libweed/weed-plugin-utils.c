/* WEED is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   Weed is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this source code; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA


   Weed is developed by:

   Gabriel "Salsaman" Finch - http://lives.sourceforge.net


   mainly based on LiViDO, which is developed by:


   Niels Elburg - http://veejay.sf.net

   Gabriel "Salsaman" Finch - http://lives.sourceforge.net

   Denis "Jaromil" Rojo - http://freej.dyne.org

   Tom Schouten - http://zwizwa.fartit.com

   Andraz Tori - http://cvs.cinelerra.org

   reviewed with suggestions and contributions from:

   Silvano "Kysucix" Galliani - http://freej.dyne.org

   Kentaro Fukuchi - http://megaui.net/fukuchi

   Jun Iio - http://www.malib.net

   Carlo Prelz - http://www2.fluido.as:8080/

*/

/* (C) Gabriel "Salsaman" Finch, 2005 - 2010 */



#include <string.h>
#include <stdio.h>

#define __WEED_INTERNAL__

#ifdef HAVE_SYSTEM_WEED
#include <weed/weed.h>
#include <weed/weed-palettes.h>
#include <weed/weed-effects.h>
#include <weed/weed-plugin.h>
#else
#include "weed.h"
#include "weed-palettes.h"
#include "weed-effects.h"
#include "weed-plugin.h"
#endif


///////////////////////////////////////////////////////////
inline int _leaf_exists (weed_plant_t *plant, const char *key) {
  if (weed_leaf_get(plant,key,0,NULL)==WEED_ERROR_NOSUCH_LEAF) return 0;
  return 1;
}


int weed_get_api_version(weed_plant_t *plugin_info) {
  // return api version selected by host
  int api_version;
  weed_plant_t *host_info;
  weed_leaf_get(plugin_info,"host_info",0,&host_info);
  weed_leaf_get(host_info,"api_version",0,&api_version);
  return api_version;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

weed_plant_t *weed_channel_template_init (const char *name, int flags, int *palettes) {
  int i;
  weed_plant_t *chantmpl=weed_plant_new(WEED_PLANT_CHANNEL_TEMPLATE);

  weed_leaf_set(chantmpl,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(chantmpl,"flags",WEED_SEED_INT,1,&flags);

  for (i=0;palettes[i]!=WEED_PALETTE_END;i++);
  if (i==0) weed_leaf_set(chantmpl,"palette_list",WEED_SEED_INT,0,NULL);
  else weed_leaf_set(chantmpl,"palette_list",WEED_SEED_INT,i,palettes);
  return chantmpl;
}


weed_plant_t *weed_audio_channel_template_init (const char *name, int flags) {
  int wtrue=WEED_TRUE;
  weed_plant_t *chantmpl=weed_plant_new(WEED_PLANT_CHANNEL_TEMPLATE);

  weed_leaf_set(chantmpl,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(chantmpl,"flags",WEED_SEED_INT,1,&flags);
  weed_leaf_set(chantmpl,"is_audio",WEED_SEED_BOOLEAN,1,&wtrue);
  return chantmpl;
}


weed_plant_t *weed_filter_class_init (const char *name, const char *author, int version, int flags, weed_init_f init_func, weed_process_f process_func, weed_deinit_f deinit_func, weed_plant_t **in_chantmpls, weed_plant_t **out_chantmpls, weed_plant_t **in_paramtmpls, weed_plant_t **out_paramtmpls) {
  int i;
  weed_plant_t *filter_class=weed_plant_new(WEED_PLANT_FILTER_CLASS);

  weed_leaf_set(filter_class,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(filter_class,"author",WEED_SEED_STRING,1,&author);
  weed_leaf_set(filter_class,"version",WEED_SEED_INT,1,&version);
  weed_leaf_set(filter_class,"flags",WEED_SEED_INT,1,&flags);

  if (init_func!=NULL) {
    weed_init_f *fn_ptr_ptr=(weed_init_f *)weed_malloc(sizeof(weed_init_f));
    *fn_ptr_ptr=init_func;
    weed_leaf_set(filter_class,"init_func",WEED_SEED_VOIDPTR,1,&fn_ptr_ptr);
  }
  if (process_func!=NULL) {
    weed_process_f *fn_ptr_ptr=(weed_process_f *)weed_malloc(sizeof(weed_process_f));
    *fn_ptr_ptr=process_func;
    weed_leaf_set(filter_class,"process_func",WEED_SEED_VOIDPTR,1,&fn_ptr_ptr);
  }
  if (deinit_func!=NULL) {
    weed_deinit_f *fn_ptr_ptr=(weed_deinit_f *)weed_malloc(sizeof(weed_deinit_f));
    *fn_ptr_ptr=deinit_func;
    weed_leaf_set(filter_class,"deinit_func",WEED_SEED_VOIDPTR,1,&fn_ptr_ptr);
  }

  if (in_chantmpls==NULL||in_chantmpls[0]==NULL) weed_leaf_set(filter_class,"in_channel_templates",WEED_SEED_VOIDPTR,0,NULL);
  else {
    for (i=0;in_chantmpls[i]!=NULL;i++);
    weed_leaf_set(filter_class,"in_channel_templates",WEED_SEED_PLANTPTR,i,in_chantmpls);

  }

  if (out_chantmpls==NULL||out_chantmpls[0]==NULL) weed_leaf_set(filter_class,"out_channel_templates",WEED_SEED_VOIDPTR,0,NULL);
  else {
    for (i=0;out_chantmpls[i]!=NULL;i++);
    weed_leaf_set(filter_class,"out_channel_templates",WEED_SEED_PLANTPTR,i,out_chantmpls);
  }

  if (in_paramtmpls==NULL||in_paramtmpls[0]==NULL) weed_leaf_set(filter_class,"in_parameter_templates",WEED_SEED_VOIDPTR,0,NULL);
  else {
    for (i=0;in_paramtmpls[i]!=NULL;i++);
    weed_leaf_set(filter_class,"in_parameter_templates",WEED_SEED_PLANTPTR,i,in_paramtmpls);
  }

  if (out_paramtmpls==NULL||out_paramtmpls[0]==NULL) weed_leaf_set(filter_class,"out_parameter_templates",WEED_SEED_VOIDPTR,0,NULL);
  else {
    for (i=0;out_paramtmpls[i]!=NULL;i++);
    weed_leaf_set(filter_class,"out_parameter_templates",WEED_SEED_PLANTPTR,i,out_paramtmpls);
  }
  
  return filter_class;
} 


void weed_plugin_info_add_filter_class (weed_plant_t *plugin_info, weed_plant_t *filter_class) {
  int num_filters=0,i;
  weed_plant_t **filters;

  if (_leaf_exists(plugin_info,"filters")) num_filters=weed_leaf_num_elements(plugin_info,"filters");
  filters=(weed_plant_t **)weed_malloc((num_filters+1)*sizeof(weed_plant_t *));
  for (i=0;i<num_filters;i++) weed_leaf_get(plugin_info,"filters",i,&filters[i]);
  filters[i]=filter_class;
  weed_leaf_set(plugin_info,"filters",WEED_SEED_PLANTPTR,i+1,filters);
  weed_leaf_set(filter_class,"plugin_info",WEED_SEED_PLANTPTR,1,&plugin_info);
  weed_free(filters);
}


weed_plant_t *weed_parameter_template_get_gui (weed_plant_t *paramt) {
  weed_plant_t *gui;

  if (_leaf_exists(paramt,"gui")) {
    weed_leaf_get(paramt,"gui",0,&gui);
    return gui;
  }

  gui=weed_plant_new(WEED_PLANT_GUI);
  weed_leaf_set(paramt,"gui",WEED_SEED_PLANTPTR,1,&gui);
  return gui;
}


weed_plant_t *weed_filter_class_get_gui (weed_plant_t *filter) {
  weed_plant_t *gui;

  if (_leaf_exists(filter,"gui")) {
    weed_leaf_get(filter,"gui",0,&gui);
    return gui;
  }

  gui=weed_plant_new(WEED_PLANT_GUI);
  weed_leaf_set(filter,"gui",WEED_SEED_PLANTPTR,1,&gui);
  return gui;
}

  
weed_plant_t *weed_parameter_get_gui(weed_plant_t *param) {
  weed_plant_t *template;

  if (_leaf_exists(param,"template")) {
    weed_leaf_get(param,"template",0,&template);
    return weed_parameter_template_get_gui(template);
  }
  return NULL;
}



//////////////////////////////////////////////////////////////////////////////////////////////

weed_plant_t *weed_integer_init (const char *name, const char *label, int def, int min, int max) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_INTEGER;
  weed_plant_t *gui;
  int wtrue=WEED_TRUE;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_INT,1,&def);
  weed_leaf_set(paramt,"min",WEED_SEED_INT,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_INT,1,&max);

  gui=weed_parameter_template_get_gui(paramt);
  weed_leaf_set(gui,"label",WEED_SEED_STRING,1,&label);
  weed_leaf_set(gui,"use_mnemonic",WEED_SEED_BOOLEAN,1,&wtrue);

  return paramt;
}


weed_plant_t *weed_string_list_init (const char *name, const char *label, int def, const char ** const list) {
  int i=0;
  weed_plant_t *paramt,*gui;
  int min=0;

  while (list[i]!=NULL) i++;
  i--;

  if (def<=-1) min=def=-1;

  paramt=weed_integer_init(name,label,def,min,i);
  gui=weed_parameter_template_get_gui(paramt);

  weed_leaf_set(gui,"choices",WEED_SEED_STRING,i+1,list);

  return paramt;
}


weed_plant_t *weed_switch_init (const char *name, const char *label, int def) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_SWITCH;
  weed_plant_t *gui;
  int wtrue=WEED_TRUE;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_BOOLEAN,1,&def);

  gui=weed_parameter_template_get_gui(paramt);
  weed_leaf_set(gui,"label",WEED_SEED_STRING,1,&label);
  weed_leaf_set(gui,"use_mnemonic",WEED_SEED_BOOLEAN,1,&wtrue);

  return paramt;
}

weed_plant_t *weed_radio_init (const char *name, const char *label, int def, int group) {
  weed_plant_t *paramt=weed_switch_init(name,label,def);
  weed_leaf_set(paramt,"group",WEED_SEED_INT,1,&group);
  return paramt;
}



weed_plant_t *weed_float_init (const char *name, const char *label, double def, double min, double max) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_FLOAT;
  weed_plant_t *gui;
  int wtrue=WEED_TRUE;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_DOUBLE,1,&def);
  weed_leaf_set(paramt,"min",WEED_SEED_DOUBLE,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_DOUBLE,1,&max);

  gui=weed_parameter_template_get_gui(paramt);
  weed_leaf_set(gui,"label",WEED_SEED_STRING,1,&label);
  weed_leaf_set(gui,"use_mnemonic",WEED_SEED_BOOLEAN,1,&wtrue);

  return paramt;
}


weed_plant_t *weed_text_init (const char *name, const char *label, const char *def) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_TEXT;
  weed_plant_t *gui;
  int wtrue=WEED_TRUE;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_STRING,1,&def);

  gui=weed_parameter_template_get_gui(paramt);
  weed_leaf_set(gui,"label",WEED_SEED_STRING,1,&label);
  weed_leaf_set(gui,"use_mnemonic",WEED_SEED_BOOLEAN,1,&wtrue);

  return paramt;
}


weed_plant_t *weed_colRGBi_init (const char *name, const char *label, int red, int green, int blue) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_COLOR;
  int cspace=WEED_COLORSPACE_RGB;
  int def[3]={red,green,blue};
  int min=0;
  int max=255;
  weed_plant_t *gui;
  int wtrue=WEED_TRUE;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"colorspace",WEED_SEED_INT,1,&cspace);

  weed_leaf_set(paramt,"default",WEED_SEED_INT,3,def);
  weed_leaf_set(paramt,"min",WEED_SEED_INT,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_INT,1,&max);

  gui=weed_parameter_template_get_gui(paramt);
  weed_leaf_set(gui,"label",WEED_SEED_STRING,1,&label);
  weed_leaf_set(gui,"use_mnemonic",WEED_SEED_BOOLEAN,1,&wtrue);

  return paramt;
}


weed_plant_t *weed_colRGBd_init (const char *name, const char *label, double red, double green, double blue) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_COLOR;
  int cspace=WEED_COLORSPACE_RGB;
  double def[3]={red,green,blue};
  double min=0.;
  double max=1.;
  weed_plant_t *gui;
  int wtrue=WEED_TRUE;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"colorspace",WEED_SEED_INT,1,&cspace);

  weed_leaf_set(paramt,"default",WEED_SEED_DOUBLE,3,def);
  weed_leaf_set(paramt,"min",WEED_SEED_DOUBLE,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_DOUBLE,1,&max);

  gui=weed_parameter_template_get_gui(paramt);
  weed_leaf_set(gui,"label",WEED_SEED_STRING,1,&label);
  weed_leaf_set(gui,"use_mnemonic",WEED_SEED_BOOLEAN,1,&wtrue);

  return paramt;
}


weed_plant_t *weed_out_param_integer_init (const char *name, int def, int min, int max) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_INTEGER;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_INT,1,&def);
  weed_leaf_set(paramt,"min",WEED_SEED_INT,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_INT,1,&max);
  return paramt;
}


weed_plant_t *weed_out_param_switch_init (const char *name, int def) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_SWITCH;
  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_BOOLEAN,1,&def);
  return paramt;
}



weed_plant_t *weed_out_param_float_init (const char *name, double def, double min, double max) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_FLOAT;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_DOUBLE,1,&def);
  weed_leaf_set(paramt,"min",WEED_SEED_DOUBLE,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_DOUBLE,1,&max);

  return paramt;
}



weed_plant_t *weed_out_param_text_init (const char *name, const char *def) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_TEXT;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"default",WEED_SEED_STRING,1,&def);

  return paramt;
}



weed_plant_t *weed_out_param_colRGBi_init (const char *name, int red, int green, int blue) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_COLOR;
  int cspace=WEED_COLORSPACE_RGB;
  int def[3]={red,green,blue};
  int min=0;
  int max=255;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"colorspace",WEED_SEED_INT,1,&cspace);

  weed_leaf_set(paramt,"default",WEED_SEED_INT,3,def);
  weed_leaf_set(paramt,"min",WEED_SEED_INT,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_INT,1,&max);

  return paramt;
}



weed_plant_t *weed_out_param_colRGBd_init (const char *name, double red, double green, double blue) {
  weed_plant_t *paramt=weed_plant_new(WEED_PLANT_PARAMETER_TEMPLATE);
  int hint=WEED_HINT_COLOR;
  int cspace=WEED_COLORSPACE_RGB;
  double def[3]={red,green,blue};
  double min=0.;
  double max=1.;

  weed_leaf_set(paramt,"name",WEED_SEED_STRING,1,&name);
  weed_leaf_set(paramt,"hint",WEED_SEED_INT,1,&hint);
  weed_leaf_set(paramt,"colorspace",WEED_SEED_INT,1,&cspace);

  weed_leaf_set(paramt,"default",WEED_SEED_DOUBLE,3,def);
  weed_leaf_set(paramt,"min",WEED_SEED_DOUBLE,1,&min);
  weed_leaf_set(paramt,"max",WEED_SEED_DOUBLE,1,&max);

  return paramt;
}



///////////////////////////////////////////////////////////////////////

static void _weed_clone_leaf (weed_plant_t *from, const char *key, weed_plant_t *to) {
  int i,num=weed_leaf_num_elements(from,key);

  int *datai;
  double *datad;
  char **datac;
  int64_t *datai6;
  void **datav;
  weed_plant_t **datap;
  size_t stlen;

  int seed_type=weed_leaf_seed_type(from,key);

  if (num==0) weed_leaf_set(to,key,seed_type,0,NULL);
  else {
    switch (seed_type) {
    case WEED_SEED_INT:
      datai=(int *)weed_malloc(num*sizeof(int));
      for (i=0;i<num;i++) weed_leaf_get(from,key,i,&datai[i]);
      weed_leaf_set(to,key,WEED_SEED_INT,num,datai);
      weed_free(datai);
      break;
    case WEED_SEED_INT64:
      datai6=(int64_t *)weed_malloc(num*sizeof(int64_t));
      for (i=0;i<num;i++) weed_leaf_get(from,key,i,&datai6[i]);
      weed_leaf_set(to,key,WEED_SEED_INT64,num,datai6);
      weed_free(datai6);
      break;
    case WEED_SEED_BOOLEAN:
      datai=(int *)weed_malloc(num*sizeof(int));
      for (i=0;i<num;i++) weed_leaf_get(from,key,i,&datai[i]);
      weed_leaf_set(to,key,WEED_SEED_BOOLEAN,num,datai);
      weed_free(datai);
      break;
    case WEED_SEED_DOUBLE:
      datad=(double *)weed_malloc(num*sizeof(double));
      for (i=0;i<num;i++) weed_leaf_get(from,key,i,&datad[i]);
      weed_leaf_set(to,key,WEED_SEED_DOUBLE,num,datad);
      weed_free(datad);
      break;
    case WEED_SEED_VOIDPTR:
      datav=(void **)weed_malloc(num*sizeof(void *));
      for (i=0;i<num;i++) weed_leaf_get(from,key,i,&datav[i]);
      weed_leaf_set(to,key,WEED_SEED_VOIDPTR,num,datav);
      weed_free(datav);
      break;
    case WEED_SEED_PLANTPTR:
      datap=(weed_plant_t **)weed_malloc(num*sizeof(weed_plant_t *));
      for (i=0;i<num;i++) weed_leaf_get(from,key,i,&datap[i]);
      weed_leaf_set(to,key,WEED_SEED_PLANTPTR,num,datap);
      weed_free(datap);
      break;
    case WEED_SEED_STRING:
      datac=(char **)weed_malloc(num*sizeof(char *));
      for (i=0;i<num;i++) {
	stlen=weed_leaf_element_size(from,key,i);
	datac[i]=(char *)weed_malloc(stlen+1);
	weed_leaf_get(from,key,i,&datac[i]);
	weed_memset(datac[i]+stlen,0,1);
      }
      weed_leaf_set(to,key,WEED_SEED_STRING,num,datac);
      for (i=0;i<num;i++) weed_free(datac[i]);
      weed_free(datac);
      break;
    }
  }
}


weed_plant_t **weed_clone_plants (weed_plant_t **plants) {
  //plants must be a NULL terminated array
  int i,j,k,type,num_plants;
  weed_plant_t **ret,*gui,*gui2;
  char **leaves,**leaves2;
  for (i=0;plants[i]!=NULL;i++);
  num_plants=i;
  ret=(weed_plant_t **)weed_malloc((num_plants+1)*sizeof(weed_plant_t *));

  for (i=0;i<num_plants;i++) {
    weed_leaf_get(plants[i],"type",0,&type);
    ret[i]=weed_plant_new(type);

    leaves=weed_plant_list_leaves(plants[i]);
    for (j=0;leaves[j]!=NULL;j++) {
      if (!strcmp(leaves[j],"gui")) {
	weed_leaf_get(plants[i],"gui",0,&gui);
	gui2=weed_plant_new(WEED_PLANT_GUI);
	weed_leaf_set(ret[i],"gui",WEED_SEED_PLANTPTR,1,&gui2);
	leaves2=weed_plant_list_leaves(gui);
	for (k=0;leaves2[k]!=NULL;k++) {
	  _weed_clone_leaf(gui,leaves2[k],gui2);
	  weed_free(leaves2[k]);
	}
	weed_free(leaves2);
      }
      else _weed_clone_leaf(plants[i],leaves[j],ret[i]);
      weed_free(leaves[j]);
    }
    weed_free(leaves);
  }
  ret[i]=NULL;
  return ret;
}
