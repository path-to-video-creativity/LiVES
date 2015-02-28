// liblives.cpp
// LiVES (lives-exe)
// (c) G. Finch <salsaman@gmail.com> 2015
// Released under the GPL 3 or later
// see file ../COPYING for licensing details

/** \file liblives.cpp
    liblives interface
 */

#ifndef DOXYGEN_SKIP
extern "C" {
#include <libOSC/libosc.h>
#include <libOSC/OSC-client.h>
#include "main.h"
#include "lbindings.h"
}


#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>


#include "liblives.hpp"

extern "C" {
  int real_main(int argc, char *argv[], ulong id);

  bool is_big_endian(void);

  bool lives_osc_cb_quit(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_cb_play(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_cb_stop(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_cb_fgclip_select(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_record_start(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_record_stop(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_record_toggle(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);
  bool lives_osc_cb_saveset(void *context, int arglen, const void *vargs, OSCTimeTag when, void * ra);

}


inline int pad4(int val) {
  return (int)((val+4)/4)*4;
}


static int padup(char **str, int arglen) {
  int newlen = pad4(arglen);
  char *ostr = *str;
  *str = (char *)lives_calloc(1,newlen);
  lives_memcpy(*str, ostr, arglen);
  lives_free(ostr);
  return newlen;
}


static int add_int_arg(char **str, int arglen, int val) {
  int newlen = arglen + 4;
  char *ostr = *str;
  *str = (char *)lives_calloc(1,newlen);
  lives_memcpy(*str, ostr, arglen);
  if (!is_big_endian()) {
    (*str)[arglen] = (unsigned char)((val&0xFF000000)>>3);
    (*str)[arglen+1] = (unsigned char)((val&0x00FF0000)>>2);
    (*str)[arglen+2] = (unsigned char)((val&0x0000FF00)>>1);
    (*str)[arglen+3] = (unsigned char)(val&0x000000FF);
  }
  else {
    lives_memcpy(*str + arglen, &val, 4);
  }
  lives_free(ostr);
  return newlen;
}


static int add_string_arg(char **str, int arglen, const char *val) {
  int newlen = arglen + strlen(val) + 1;
  char *ostr = *str;
  *str = (char *)lives_calloc(1,newlen);
  lives_memcpy(*str, ostr, arglen);
  lives_memcpy(*str + arglen, val, strlen(val));
  lives_free(ostr);
  return newlen;
}


static void *play_thread(void *) {
  int arglen = 1;
  char **vargs=(char **)lives_malloc(sizeof(char *));
  *vargs = strdup(",");
  arglen = padup(vargs, arglen);
  lives_osc_cb_play(NULL, arglen, (const void *)(*vargs), OSCTT_CurrentTime(), NULL);
  lives_free(*vargs);
  return NULL;
}

static volatile bool spinning;
static ulong msg_id;
static char *private_response;

static bool private_cb(lives::_privateInfo *info, void *data) {
  if (info->id == msg_id) {
    private_response = strdup(info->response);
    spinning = false;
    return false;
  }

  return true;
}

#endif // doxygen_skip

//////////////////////////////////////////////////

namespace lives {

#ifndef DOXYGEN_SKIP
  typedef struct {
    ulong id;
    livesApp *app;
  } livesAppCtx;

  static list<livesAppCtx> appMgr;

  static livesApp *find_instance_for_id(ulong id) {
    list<livesAppCtx>::iterator it = appMgr.begin();
    while (it != appMgr.end()) {
      if ((*it).id == id) {
	return (*it).app;
      }
      ++it;
    }
    return NULL;
  }

#endif

  void LiVESString::toEncoding(lives_char_encoding_t enc) {
    if (enc == LIVES_CHAR_ENCODING_UTF8) {
      if (m_encoding == LIVES_CHAR_ENCODING_LOCAL8BIT) {
	assign(L2U8(this->c_str()));
      }
      else if (m_encoding == LIVES_CHAR_ENCODING_FILESYSTEM) {
	assign(F2U8(this->c_str()));
      }
    }
    else if (enc == LIVES_CHAR_ENCODING_FILESYSTEM) {
      if (m_encoding == LIVES_CHAR_ENCODING_UTF8) {
	assign(U82F(this->c_str()));
      }
      if (m_encoding == LIVES_CHAR_ENCODING_LOCAL8BIT) {
	assign(L2U8(this->c_str()));
	assign(U82F(this->c_str()));
      }
    }
    else if (enc == LIVES_CHAR_ENCODING_LOCAL8BIT) {
      if (m_encoding == LIVES_CHAR_ENCODING_UTF8) {
	assign(U82L(this->c_str()));
      }
      if (m_encoding == LIVES_CHAR_ENCODING_FILESYSTEM) {
	assign(F2U8(this->c_str()));
	assign(U82L(this->c_str()));
      }
    }
  }
  


  void livesApp::init(int argc, char *oargv[]) {
    char **argv;
    char progname[] = "lives-exe";
    if (argc < 0) argc=0;
    argc++;

    argv=(char **)malloc(argc * sizeof(char *));
    argv[0]=strdup(progname);

    for (int i=1; i < argc; i++) {
      argv[i]=strdup(oargv[i-1]);
    }

    ulong id = lives_random();
    livesAppCtx ctx;

    ctx.id = id;
    ctx.app = this;
    appMgr.push_back(ctx);

    real_main(argc, argv, id);
    free(argv);
    m_id = id;
  }


  livesApp::livesApp() : m_set(this), m_id(0l) {
    if (appMgr.empty())
      init(0,NULL);
  }

  livesApp::livesApp(int argc, char *argv[]) : m_set(this), m_id(0l) {
    if (appMgr.empty())
      init(argc,argv);
  }


  livesApp::~livesApp() {
    if (!isValid()) return;

    int arglen = 1;
    char **vargs=(char **)lives_malloc(sizeof(char *));
    *vargs = strdup(",");
    arglen = padup(vargs, arglen);

    // call object destructor callback
    binding_cb (LIVES_CALLBACK_OBJECT_DESTROYED, NULL, (ulong)this);
    
    closureListIterator it = m_closures.begin();
    while (it != m_closures.end()) {
      delete *it;
      it = m_closures.erase(it);
    }

    appMgr.clear();

    lives_osc_cb_quit(NULL, arglen, (const void *)(*vargs), OSCTT_CurrentTime(), NULL);
    lives_free(*vargs);
  }


  bool livesApp::isValid() {
    return m_id != 0l;
  }

  set * const livesApp::currentSet() {
    return &m_set;
  }

  ulong livesApp::appendClosure(lives_callback_t cb_type, callback_f func, void *data) {
    closure *cl = new closure;
    cl->id = lives_random();
    cl->object = this;
    cl->cb_type = cb_type;
    cl->func = (callback_f)func;
    cl->data = data;
    m_closures.push_back(cl);
    return cl->id;
  }

  ulong livesApp::addCallback(lives_callback_t cb_type, modeChanged_callback_f func, void *data) {
    if (cb_type != LIVES_CALLBACK_MODE_CHANGED) return 0l;
    return appendClosure(cb_type, (callback_f)func, data);
  }

  ulong livesApp::addCallback(lives_callback_t cb_type, private_callback_f func, void *data) {
    if (cb_type != LIVES_CALLBACK_PRIVATE) return 0l;
    return appendClosure(cb_type, (callback_f)func, data);
  }

  ulong livesApp::addCallback(lives_callback_t cb_type, objectDestroyed_callback_f func, void *data) {
    if (cb_type != LIVES_CALLBACK_OBJECT_DESTROYED) return 0l;
    return appendClosure(cb_type, (callback_f)func, data);
  }

  ulong livesApp::addCallback(lives_callback_t cb_type, appQuit_callback_f func, void *data) {
    if (cb_type != LIVES_CALLBACK_APP_QUIT) return 0l;
    return appendClosure(cb_type, (callback_f)func, data);
  }

  bool livesApp::removeCallback(ulong id) {
    closureListIterator it = m_closures.begin();
    while (it != m_closures.end()) {
      if ((*it)->id == id) {
	delete *it;
	m_closures.erase(it);
	return true;
      }
      ++it;
    }
    return false;
  }


  void livesApp::play() {
    if (!isValid()) return;
    play_thread(NULL);
  }

  bool livesApp::stop() {
    if (!isValid()) return FALSE;
    // return false if we are not playing
    return lives_osc_cb_stop(NULL, 0, NULL, OSCTT_CurrentTime(), NULL);
  }


  lives_dialog_response_t livesApp::showInfo(LiVESString text, bool blocking) {
    lives_dialog_response_t ret=LIVES_DIALOG_RESPONSE_INVALID;
    if (!isValid()) return ret;
    // if blocking wait for response
    if (blocking) {
      spinning = true;
      msg_id = lives_random();
      ulong cbid = addCallback(LIVES_CALLBACK_PRIVATE, private_cb, NULL); 
      if (!idle_show_info(text.c_str(),blocking,msg_id)) {
	spinning = false;
	removeCallback(cbid);
      }
      else {
	while (spinning) usleep(100);
	ret = (lives_dialog_response_t)atoi(private_response);
	lives_free(private_response);
      }
      return ret;
    }
    if (idle_show_info(text.c_str(),blocking,0))
      return LIVES_DIALOG_RESPONSE_NONE;
    return ret;
  }

  LiVESString livesApp::chooseFileWithPreview(LiVESString dirname, lives_filechooser_t preview_type, LiVESString title) {
    LiVESString emptystr;
    if (!isValid()) return emptystr;
    if (preview_type != LIVES_FILE_CHOOSER_VIDEO_AUDIO && preview_type != LIVES_FILE_CHOOSER_AUDIO_ONLY) return emptystr;
    spinning = true;
    msg_id = lives_random();
    ulong cbid = addCallback(LIVES_CALLBACK_PRIVATE, private_cb, NULL);
    char *ret = NULL;
    if (!idle_choose_file_with_preview(dirname.c_str(),title.c_str(),preview_type,msg_id)) {
      spinning = false;
      removeCallback(cbid);
    }
    else {
      while (spinning) usleep(100);
      // last 2 chars are " " and %d (deinterlace choice)
      LiVESString str(private_response, strlen(private_response - 2));
      str.toEncoding(LIVES_CHAR_ENCODING_FILESYSTEM);
      m_deinterlace = (bool)atoi(private_response + strlen(private_response) - 2);
      lives_free(private_response);
      return str;
    }
    return emptystr;
  }


  LiVESString livesApp::chooseSet() {
    LiVESString emptystr;
    if (!isValid()) return emptystr;
    spinning = true;
    msg_id = lives_random();
    ulong cbid = addCallback(LIVES_CALLBACK_PRIVATE, private_cb, NULL);
    char *ret = NULL;
    if (!idle_choose_set(msg_id)) {
      spinning = false;
      removeCallback(cbid);
    }
    else {
      while (spinning) usleep(100);
      // last 2 chars are " " and %d (deinterlace choice)
      LiVESString str(private_response);
      str.toEncoding(LIVES_CHAR_ENCODING_FILESYSTEM);
      lives_free(private_response);
      return str;
    }
    return emptystr;
  }


  clip livesApp::openFile(LiVESString fname, bool with_audio, double stime, int frames, bool deinterlace) {
    if (!isValid()) return clip(0);
    if (fname.empty()) return clip(0);
    spinning = true;
    msg_id = lives_random();
    ulong cbid = addCallback(LIVES_CALLBACK_PRIVATE, private_cb, NULL);
    ulong cid = 0l;
    LiVESString ffname = LiVESString(fname);
    ffname.toEncoding(LIVES_CHAR_ENCODING_FILESYSTEM);
    if (!idle_open_file(ffname.c_str(), stime, frames, msg_id)) {
      spinning = false;
      removeCallback(cbid);
    }
    else {
      while (spinning) usleep(100);
      cid = strtoul(private_response, NULL, 10);
      lives_free(private_response);
    }
    return clip(cid);
  }

  bool livesApp::deinterlaceOption() {
    return m_deinterlace;
  }

  lives_interface_mode_t livesApp::mode() {
    if (!isValid()) return LIVES_INTERFACE_MODE_INVALID;
    if (mainw->multitrack != NULL) return LIVES_INTERFACE_MODE_MULTITRACK;
    return LIVES_INTERFACE_MODE_CLIPEDIT;
  }

  lives_status_t livesApp::status() {
    if (!isValid()) return LIVES_STATUS_INVALID;
    if (!mainw->is_ready) return LIVES_STATUS_NOTREADY;
    if (mainw->playing_file > -1) return LIVES_STATUS_PLAYING;
    if (mainw->is_processing) return LIVES_STATUS_PROCESSING;
    if (mainw->preview) return LIVES_STATUS_PREVIEW;
    return LIVES_STATUS_READY;
  }

  closureList livesApp::closures() {
    return m_closures;
  }

  void livesApp::invalidate() {
    m_id = 0l;
  }

  bool livesApp::interactive() {
    return mainw->interactive;
  }


  void livesApp::setInteractive(bool setting) {
    spinning = true;
    msg_id = lives_random();
    ulong cbid = addCallback(LIVES_CALLBACK_PRIVATE, private_cb, NULL); 
    if (!idle_set_interactive(setting, msg_id)) {
      spinning = false;
      removeCallback(cbid);
    }
    while (spinning) usleep(100);
  }


  //////////////// set ////////////////////

  set::set(livesApp *lives) {
    m_lives = lives;
    m_name = NULL;
  }

  set::set() {
    m_lives = NULL;
    m_name = NULL;
  }

  bool set::isValid() {
    return m_lives != NULL && m_lives->isValid();
  }

  set::~set() {
    if (m_name != NULL) lives_free(m_name);

    clipListIterator it = m_clips.begin();
    while (it != m_clips.end()) {
      delete *it;
      it = m_clips.erase(it);
    }
  }

  LiVESString set::name() {
    if (!isValid()) return NULL;
    setName(get_set_name());
    return LiVESString(m_name);
  }

  void set::setName(const char *name) {
    char noname[] = "";
    if (m_name != NULL) {
      if (!strcmp(m_name, name)) return;
      lives_free(m_name);
    }
    if (name == NULL) m_name = strdup(noname);
    else m_name = strdup(name);
  }


  bool set::save(LiVESString name, bool force_append) {
    if (!isValid()) return FALSE;
    int arglen = 3;
    char **vargs=(char **)lives_malloc(sizeof(char *));
    const char *cname = name.c_str();
    *vargs = strdup(",si");
    arglen = padup(vargs, arglen);
    arglen = add_string_arg(vargs, arglen, cname);
    arglen = add_int_arg(vargs, arglen, force_append);

    spinning = true;
    msg_id = lives_random();

    ulong cbid = m_lives->addCallback(LIVES_CALLBACK_PRIVATE, private_cb, NULL); 

    bool ret = false;

    if (!idle_save_set(cname,arglen,(const void *)(*vargs),msg_id)) {
      spinning = false;
      m_lives->removeCallback(cbid);
    }
    else {
      while (spinning) usleep(100);
      ret = (bool)(atoi(private_response));
      lives_free(private_response);
    }
    lives_free(*vargs);
    return ret;
  }


  clipList set::cliplist() {
    // clear old cliplist
    clipListIterator it = m_clips.begin();
    while (it != m_clips.end()) {
      delete *it;
      it = m_clips.erase(it);
    }
    if (isValid()) {
      ulong *ids = get_unique_ids();

      for (int i=0; ids[i] != 0l; i++) {
	clip *c = new clip(ids[i]);
	m_clips.push_back(c);
      }
      lives_free(ids);
    }

    return m_clips;
  }


  /////////////// clip ////////////////

  clip::clip() {
    m_uid=0l;
  }

  clip::clip(ulong uid) {
    m_uid = uid;
  }

  bool clip::isValid() {
    return (cnum_for_uid(m_uid) != -1);
  }

  int clip::frames() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->frames;
    return -1;
  }

  int clip::width() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->hsize;
    return -1;
  }

  int clip::height() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->vsize;
    return -1;
  }

  double clip::fps() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->fps;
    return -1.;
  }

  int clip::audioRate() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->arate;
    return -1;
  }

  int clip::audioChannels() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->achans;
    return -1;
  }

  int clip::audioSampleSize() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->asampsize;
    return -1;
  }

  bool clip::audioSigned() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return !(mainw->files[cnum]->signed_endian & AFORM_UNSIGNED);
    return true;
  }

  lives_endian_t clip::audioEndian() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) {
      if (mainw->files[cnum]->signed_endian & AFORM_BIG_ENDIAN) return LIVES_BIGENDIAN;
    }
    return LIVES_LITTLEENDIAN;
  }

  LiVESString clip::name() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return LiVESString(mainw->files[cnum]->name);
    LiVESString emptystr;
    return emptystr;
  }

  int clip::selectionStart() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->start;
    return -1;
  }

  int clip::selectionEnd() {
    int cnum = cnum_for_uid(m_uid);
    if (cnum > -1 && mainw->files[cnum] != NULL) return mainw->files[cnum]->end;
    return -1;
  }



  

  namespace prefs {
    LiVESString currentVideoLoadDir() {
      return LiVESString(mainw->vid_load_dir);
    }


  }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SKIP

void binding_cb (lives_callback_t cb_type, const char *msgstring, ulong id) {
  bool ret;
  lives::livesApp *lapp;

  if (cb_type == LIVES_CALLBACK_OBJECT_DESTROYED) lapp = (lives::livesApp *)id;
  else lapp = lives::find_instance_for_id(id);

  if (lapp == NULL) return;

  lives::closureList cl = lapp->closures();

  lives::closureListIterator it = cl.begin();
  while (it != cl.end()) {

    if ((*it)->cb_type == cb_type) {
      switch (cb_type) {
      case LIVES_CALLBACK_MODE_CHANGED:
	{
	  lives::modeChangedInfo info;
	  info.mode = (lives_interface_mode_t)atoi(msgstring);
	  lives::modeChanged_callback_f fn = (lives::modeChanged_callback_f)((*it)->func);
	  ret = (fn)((*it)->object, &info, (*it)->data);
	}
	break;
      case LIVES_CALLBACK_APP_QUIT:
	{
	  lives::appQuitInfo info;
	  info.signum = atoi(msgstring);
	  lives::appQuit_callback_f fn = (lives::appQuit_callback_f)((*it)->func);
	  lapp->invalidate();
	  ret = (fn)((*it)->object, &info, (*it)->data);
	}
	break;
      case LIVES_CALLBACK_OBJECT_DESTROYED:
	{
	  lives::objectDestroyed_callback_f fn = (lives::objectDestroyed_callback_f)((*it)->func);
	  ret = (fn)((*it)->object, (*it)->data);
	}
	break;
      case LIVES_CALLBACK_PRIVATE:
	{
	  // private event type
	  lives::_privateInfo info;
	  char **msgtok = lives_strsplit(msgstring, " ", -1);
	  info.id = strtoul(msgtok[0],NULL,10);
	  if (get_token_count(msgstring,' ')==1) info.response=NULL;
	  else info.response = strdup(msgtok[1]);
	  lives_strfreev(msgtok);
	  lives::private_callback_f fn = (lives::private_callback_f)((*it)->func);
	  ret = (fn)(&info, (*it)->data);
	}
	break;
      default:
	continue;
      }
      if (!ret) {
	//delete *it;
	it = cl.erase(it);
	continue;
      }
    }
    ++it;
  }
}

#endif // doxygen_skip