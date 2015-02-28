// lbindings.c
// LiVES (lives-exe)
// (c) G. Finch <salsaman@gmail.com> 2015
// Released under the GPL 3 or later
// see file ../COPYING for licensing details

#include "main.h"
#include "interface.h"
#include "callbacks.h"
#include "liblives.hpp"


typedef boolean Boolean;

#include <libOSC/libosc.h>
#include <libOSC/OSC-client.h>

typedef struct {
  ulong id;
  char *msg;
} msginfo;


typedef struct {
  ulong id;
  int arglen;
  const void *vargs;
} oscdata;


typedef struct {
  ulong id;
  char *fname;
  double stime;
  int frames;
} opfidata;


typedef struct {
  ulong id;
  char *dir;
  char *title;
  int preview_type;
} fprev;


typedef struct {
  ulong id;
  boolean setting;
} sintdata;

typedef struct {
  ulong id;
} udata;


/////////////////////////////////////////
/// extern functions with no headers

boolean lives_osc_cb_saveset(void *context, int arglen, const void *vargs, OSCTimeTag when, NetworkReturnAddressPtr ra);


/////////// value return functions ///////////////////////


static void ext_caller_return_int(ulong caller_id, int ret) {
  // this is for the C++ binding
  char *msgstring = lives_strdup_printf("%lu %d",caller_id,ret);
  binding_cb (LIVES_CALLBACK_PRIVATE, msgstring, mainw->id);
  lives_free(msgstring);
}


static void ext_caller_return_ulong(ulong caller_id, ulong ret) {
  // this is for the C++ binding
  char *msgstring = lives_strdup_printf("%lu %lu",caller_id,ret);
  binding_cb (LIVES_CALLBACK_PRIVATE, msgstring, mainw->id);
  lives_free(msgstring);
}


static void ext_caller_return_string(ulong caller_id, const char *ret) {
  // this is for the C++ binding
  char *msgstring = lives_strdup_printf("%lu %s",caller_id,ret);
  binding_cb (LIVES_CALLBACK_PRIVATE, msgstring, mainw->id);
  lives_free(msgstring);
}


///////////////////////////////////////////////////////////////
/// utility functions for liblives /////

ulong *get_unique_ids(void) {
  // return array of unique_id (ulong) for all "real" clips
  int i=0;
  ulong *uids;
  pthread_mutex_lock(&mainw->clip_list_mutex);
  LiVESList *list=mainw->cliplist;
  uids=(ulong *)lives_malloc((lives_list_length(mainw->cliplist) + 1)*sizeof(ulong));
  while (list!=NULL) {
    int uid=0;
    int cnum=LIVES_POINTER_TO_INT(list->data);
    if (mainw->files[cnum]!=NULL) uid=mainw->files[cnum]->unique_id;
    list=list->next;
    if (uid==0) continue;
    uids[i++]=uid;
  }
  uids[i]=0;
  pthread_mutex_unlock(&mainw->clip_list_mutex);
  return uids;
}


int cnum_for_uid(ulong uid) {
  pthread_mutex_lock(&mainw->clip_list_mutex);
  LiVESList *list=mainw->cliplist;
  while (list!=NULL) {
    int cnum=LIVES_POINTER_TO_INT(list->data);
    if (mainw->files[cnum]!=NULL && uid==mainw->files[cnum]->unique_id) return cnum;
    list=list->next;
  }
  pthread_mutex_unlock(&mainw->clip_list_mutex);
  return -1;
}


#define const_domain_notify 1
#define const_domain_response 1

inline int trans_rev(int consta, int a, int b) {
  if (consta==a) return b;
  else return consta;
}

int trans_constant(int consta, int domain) {
  int nconsta;
  if (domain==const_domain_notify) {
    nconsta=trans_rev(consta,LIVES_CALLBACK_FRAME_SYNCH,LIVES_OSC_NOTIFY_FRAME_SYNCH);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_PLAYBACK_STARTED,LIVES_OSC_NOTIFY_PLAYBACK_STARTED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_PLAYBACK_STOPPED,LIVES_OSC_NOTIFY_PLAYBACK_STOPPED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_PLAYBACK_STOPPED_RD,LIVES_OSC_NOTIFY_PLAYBACK_STOPPED_RD);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_RECORD_STARTED,LIVES_OSC_NOTIFY_RECORD_STARTED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_RECORD_STOPPED,LIVES_OSC_NOTIFY_RECORD_STOPPED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_APP_QUIT,LIVES_OSC_NOTIFY_QUIT);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_CLIP_OPENED,LIVES_OSC_NOTIFY_CLIP_OPENED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_CLIP_CLOSED,LIVES_OSC_NOTIFY_CLIP_CLOSED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_CLIPSET_OPENED,LIVES_OSC_NOTIFY_CLIPSET_OPENED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_CLIPSET_SAVED,LIVES_OSC_NOTIFY_CLIPSET_SAVED);
    if (nconsta!=consta) return nconsta;
    nconsta=trans_rev(consta,LIVES_CALLBACK_MODE_CHANGED,LIVES_OSC_NOTIFY_MODE_CHANGED);
    if (nconsta!=consta) return nconsta;
  }
  if (domain==const_domain_response) {
    if (consta==LIVES_RESPONSE_NONE) return LIVES_DIALOG_RESPONSE_NONE;
    if (consta==LIVES_RESPONSE_OK) return LIVES_DIALOG_RESPONSE_OK;
    if (consta==LIVES_RESPONSE_CANCEL) return LIVES_DIALOG_RESPONSE_CANCEL;
    if (consta==LIVES_RESPONSE_ACCEPT) return LIVES_DIALOG_RESPONSE_ACCEPT;
    if (consta==LIVES_RESPONSE_YES) return LIVES_DIALOG_RESPONSE_YES;
    if (consta==LIVES_RESPONSE_NO) return LIVES_DIALOG_RESPONSE_NO;

    // positive values for custom responses
    if (consta==LIVES_RESPONSE_INVALID) return LIVES_DIALOG_RESPONSE_INVALID;
    if (consta==LIVES_RESPONSE_RETRY) return LIVES_DIALOG_RESPONSE_RETRY;
    if (consta==LIVES_RESPONSE_ABORT) return LIVES_DIALOG_RESPONSE_ABORT;
    if (consta==LIVES_RESPONSE_RESET) return LIVES_DIALOG_RESPONSE_RESET;
    if (consta==LIVES_RESPONSE_SHOW_DETAILS) return LIVES_DIALOG_RESPONSE_SHOW_DETAILS;
  }


  return consta;
}



//// interface callers


static boolean call_osc_show_info(livespointer text) {
  // function that is picked up on idle
  do_info_dialog(text);
  return FALSE;
}



static boolean call_osc_show_blocking_info(livespointer data) {
  // function that is picked up on idle
  int ret;
  msginfo *minfo = (msginfo *)data;
  ret=do_blocking_info_dialog(minfo->msg);
  lives_free(minfo->msg);
  ret=trans_constant(ret,const_domain_notify);
  ext_caller_return_int(minfo->id,ret);
  lives_free(minfo);
  return FALSE;
}


static boolean call_osc_save_set(livespointer data) {
  // function that is picked up on idle
  boolean ret;
  oscdata *oscd = (oscdata *)data;
  ret=lives_osc_cb_saveset(NULL, oscd->arglen, oscd->vargs, OSCTT_CurrentTime(), NULL);
  ext_caller_return_int(oscd->id,(int)ret);
  lives_free(oscd);
  return FALSE;
}


static boolean call_file_choose_with_preview(livespointer data) {
  LiVESWidget *chooser;
  fprev *fdata = (fprev *)data;

  char *fname=NULL,*rstr;

  int preview_type;
  int response;

  if (fdata->preview_type==LIVES_FILE_CHOOSER_VIDEO_AUDIO) preview_type=LIVES_FILE_SELECTION_VIDEO_AUDIO;
  else preview_type=LIVES_FILE_SELECTION_AUDIO_ONLY;
  chooser=choose_file_with_preview(fdata->dir, fdata->title, preview_type);
  response=lives_dialog_run(LIVES_DIALOG(chooser));
  if (response == LIVES_RESPONSE_ACCEPT) {
    fname=lives_file_chooser_get_filename (LIVES_FILE_CHOOSER(chooser));
    lives_widget_destroy(chooser);
  }
  if (fdata->dir!=NULL) lives_free(fdata->dir);
  if (fdata->title!=NULL) lives_free(fdata->title);

  rstr = lives_strdup_printf("%s %d", fname, mainw->open_deint);

  ext_caller_return_string(fdata->id,rstr);
  lives_free(rstr);
  lives_free(fdata);
  return FALSE;
}


static boolean call_choose_set(livespointer data) {
  udata *ud=(udata *)data;
  char *setname=on_load_set_activate(NULL,NULL);
  ext_caller_return_string(ud->id,setname);
  lives_free(setname);
  lives_free(ud);
  return FALSE;
}




static boolean call_open_file(livespointer data) {
  opfidata *opfi = (opfidata *)data;
  ulong uid=open_file_sel(opfi->fname,opfi->stime,opfi->frames);
  if (opfi->fname!=NULL) lives_free(opfi->fname);
  ext_caller_return_ulong(opfi->id,uid);
  lives_free(opfi);
  return FALSE;
}

static boolean call_set_interactive(livespointer data) {
  sintdata *sint = (sintdata *)data;
  mainw->interactive=sint->setting;
  set_interactive(mainw->interactive);
  ext_caller_return_int(sint->id,0);
  lives_free(sint);
  return FALSE;
}


/// idlefunc hooks


boolean idle_show_info(const char *text, boolean blocking, ulong id) {
  if (mainw->preview||mainw->is_processing||mainw->playing_file>-1) {
    return FALSE;
  }
  if (text==NULL) return FALSE;
  if (!blocking) lives_idle_add(call_osc_show_info,(livespointer)text);
  else {
    msginfo *minfo = (msginfo *)lives_malloc(sizeof(msginfo));
    minfo->msg = strdup(text);
    minfo->id = id;
    lives_idle_add(call_osc_show_blocking_info,(livespointer)minfo);
  }
  return TRUE;
}


boolean idle_save_set(const char *name, int arglen, const void *vargs, ulong id) {
  if (mainw->preview||mainw->is_processing||mainw->playing_file>-1) {
    return FALSE;
  }
  else {
    oscdata *data = (oscdata *)lives_malloc(sizeof(oscdata));
    data->id=id;
    data->arglen=arglen;
    data->vargs=vargs;
    lives_idle_add(call_osc_save_set,(livespointer)data);
  }
  return TRUE;
}


boolean idle_choose_file_with_preview(const char *dirname, const char *title, int preview_type, ulong id) {
  fprev *data;

  if (mainw->preview||mainw->is_processing||mainw->playing_file>-1) {
    return FALSE;
  }

  data= (fprev *)lives_malloc(sizeof(fprev));
  data->id=id;

  if (dirname!=NULL && strlen(dirname) > 0) data->dir=strdup(dirname);
  else data->dir=NULL;
  
  if (title!=NULL && strlen(title) > 0) data->title=strdup(title);
  else data->title=NULL;

  data->preview_type=preview_type;
  lives_idle_add(call_file_choose_with_preview,(livespointer)data);
  return TRUE;
}


boolean idle_choose_set(ulong id) {
  udata *data;
  if (mainw->preview||mainw->is_processing||mainw->playing_file>-1) {
    return FALSE;
  }
  data=(udata *)lives_malloc(sizeof(udata));
  lives_idle_add(call_choose_set,(livespointer)data);
  return TRUE;
}


boolean idle_open_file(const char *fname, double stime, int frames, ulong id) {
  opfidata *data;

  if (mainw->preview||mainw->is_processing||mainw->playing_file>-1) {
    return FALSE;
  }

  if (fname==NULL || strlen(fname)==0) return FALSE;

  data= (opfidata *)lives_malloc(sizeof(opfidata));
  data->id=id;
  data->fname=strdup(fname);
  data->stime=stime;
  data->frames=frames;

  lives_idle_add(call_open_file,(livespointer)data);
  return TRUE;
}



boolean idle_set_interactive(boolean setting, ulong id) {
  sintdata *data=(sintdata *)lives_malloc(sizeof(sintdata));
  data->id=id;
  data->setting=setting;
  lives_idle_add(call_set_interactive,(livespointer)data);
  return TRUE;
}