// mainwindow.h
// LiVES (lives-exe)
// (c) G. Finch <salsaman+lives@gmail.com> 2003 - 2019
// Released under the GPL 3 or later
// see file ../COPYING for licensing details

#ifndef HAS_LIVES_MAINWINDOW_H
#define HAS_LIVES_MAINWINDOW_H

#include <pthread.h>

#include "effects-data.h"

#include "audio.h"

#ifdef ALSA_MIDI
#include <alsa/asoundlib.h>
#endif

#ifdef HAVE_PULSE_AUDIO
#include "pulse.h"
#endif

// hardware related prefs

/// fraction of a second quantisation for event timing
///
/// > 10**8 is not recommended, since we sometimes store seconds in a double
#define TICKS_PER_SECOND_DBL ((double)100000000.)   ///< actually microseconds / 100.
#define TICKS_PER_SECOND (int64_t)100000000 ///< ticks per second
#define USEC_TO_TICKS (TICKS_PER_SECOND / 1000000l) ///< how many TICKS in a microsecond [default 100]

#define LIVES_SHORTEST_TIMEOUT  (2. * TICKS_PER_SECOND_DBL) // 2 sec timeout
#define LIVES_SHORT_TIMEOUT  (5. * TICKS_PER_SECOND_DBL) // 5 sec timeout
#define LIVES_DEFAULT_TIMEOUT  (10. * TICKS_PER_SECOND_DBL) // 10 sec timeout
#define LIVES_LONGER_TIMEOUT  (20. * TICKS_PER_SECOND_DBL) // 20 sec timeout
#define LIVES_LONGEST_TIMEOUT  (30. * TICKS_PER_SECOND_DBL) // 30 sec timeout

#define DEF_FPS 25.

/// rate to change pb fps when faster/slower pressed (TODO: make pref)
#define DEF_FPSCHANGE_AMOUNT 30000

/// forward/back scratch value (TODO: make pref)
#define DEF_SCRATCHBACK_AMOUNT 80000
#define DEF_SCRATCHFWD_AMOUNT 80000

/// adjustment amount for effect parameter via keyboard (TODO: make pref)
#define DEF_BLENDCHANGE_AMOUNT 100

#define LOOP_LOCK_MIN_FRAMES (cfile->pb_fps + 1)

#define DEF_DL_BANDWIDTH 5000 ///< Kb / sec

/////// GUI related constants /////////////////////////////////////////////////////////

// parameters for resizing the image frames, and for capture
#define H_RESIZE_ADJUST ((widget_opts.packing_width + 2) * 2)
#define V_RESIZE_ADJUST ((widget_opts.packing_height + 2) * 2)

// space to reserve for the CE timeline
// IMPORTANT to fine tune this - TODO
#define CE_TIMELINE_HSPACE ((int)(420. * widget_opts.scale))

/// char width of combo entries (default)
#define COMBOWIDTHCHARS 12

/// char width of framecounter
#define FCWIDTHCHARS 18

/// char width of preview spinbutton
#define PREVSBWIDTHCHARS 8

// min sizes for the separate play window
#define MIN_SEPWIN_WIDTH 600
#define MIN_SEPWIN_HEIGHT 36

/// sepwin/screen size safety margins in pixels
#define SCR_WIDTH_SAFETY ((int)(100. * widget_opts.scale))
#define SCR_HEIGHT_SAFETY ((int)(200. * widget_opts.scale))

/// height of preview widgets in sepwin
#define PREVIEW_BOX_HT ((int)(100. * widget_opts.scale))

/// (unexpanded) height of rows in treeviews
#define TREE_ROW_HEIGHT ((int)(60. * widget_opts.scale))

// a few GUI specific settings
#define GUI_SCREEN_WIDTH (mainw->mgeom[widget_opts.monitor].width)
#define GUI_SCREEN_HEIGHT (mainw->mgeom[widget_opts.monitor].height)
#define GUI_SCREEN_X (mainw->mgeom[widget_opts.monitor].x)
#define GUI_SCREEN_Y (mainw->mgeom[widget_opts.monitor].y)

// scaling limits
#define SCREEN_SCALE_DEF_WIDTH 1600
#define SCREEN_169_MIN_WIDTH 1440
#define SCREEN_43S_LIMIT_WIDTH 1024
#define SCREEN_43S_LIMIT_HEIGHT 720

/// default size for frames
#define DEF_FRAME_HSIZE_169_UNSCALED 1280
#define DEF_FRAME_VSIZE_169_UNSCALED 720

#define DEF_FRAME_HSIZE_43_UNSCALED 1024
#define DEF_FRAME_VSIZE_43_UNSCALED 768

#define DEF_FRAME_HSIZE_43S_UNSCALED 640.
#define DEF_FRAME_VSIZE_43S_UNSCALED 480.

#define DEF_FRAME_HSIZE_GUI (((int)(DEF_FRAME_HSIZE_43S_UNSCALED * widget_opts.scale) >> 2) << 1)
#define DEF_FRAME_VSIZE_GUI (((int)(DEF_FRAME_VSIZE_43S_UNSCALED * widget_opts.scale) >> 1) << 1)

// min screen height to show the message area
#define MIN_MSG_AREA_SCRNHEIGHT (DEF_FRAME_HSIZE_GUI + CE_TIMELINE_HSPACE)

#define DEF_FRAME_HSIZE_UNSCALED ((GUI_SCREEN_WIDTH >= SCREEN_169_MIN_WIDTH) ? DEF_FRAME_HSIZE_169_UNSCALED : (GUI_SCREEN_WIDTH >= SCREEN_43S_LIMIT_WIDTH && GUI_SCREEN_HEIGHT >= SCREEN_43S_LIMIT_HEIGHT) ? DEF_FRAME_HSIZE_43_UNSCALED : DEF_FRAME_HSIZE_43S_UNSCALED)

#define DEF_FRAME_VSIZE_UNSCALED ((GUI_SCREEN_WIDTH >= SCREEN_169_MIN_WIDTH) ? DEF_FRAME_VSIZE_169_UNSCALED : (GUI_SCREEN_WIDTH >= SCREEN_43S_LIMIT_WIDTH && GUI_SCREEN_HEIGHT >= SCREEN_43S_LIMIT_HEIGHT) ? DEF_FRAME_VSIZE_43_UNSCALED : DEF_FRAME_VSIZE_43S_UNSCALED)

#define DEF_GEN_WIDTH DEF_FRAME_HSIZE_UNSCALED
#define DEF_GEN_HEIGHT DEF_FRAME_VSIZE_UNSCALED

#define DEF_FRAME_HSIZE ((((int)((double)DEF_FRAME_HSIZE_UNSCALED * widget_opts.scale)) >> 2) << 2)
#define DEF_FRAME_VSIZE ((((int)((double)DEF_FRAME_VSIZE_UNSCALED * widget_opts.scale)) >> 1) << 1)

#define FRAMEBLANK_MIN_WIDTH ((int)(240. * widget_opts.scale))
#define FRAMEBLANK_MAX_WIDTH ((int)(600. * widget_opts.scale))

#define FRAMEBLANK_MIN_HEIGHT ((int)(180. * widget_opts.scale))
#define FRAMEBLANK_MAX_HEIGHT ((int)(400. * widget_opts.scale))

#define IMSEP_MAX_HEIGHT ((int)(64. * widget_opts.scale))
#define IMSEP_MAX_WIDTH (GUI_SCREEN_WIDTH - 20)

#define MAIN_SPIN_SPACER ((int)52. * widget_opts.scale) ///< pixel spacing for start/end spins for clip and multitrack editors

#define ENC_DETAILS_WIN_H ((int)(DEF_FRAME_HSIZE_43S_UNSCALED * widget_opts.scale)) ///< horizontal size in pixels of the encoder output window
#define ENC_DETAILS_WIN_V (((int)(DEF_FRAME_VSIZE_43S_UNSCALED * widget_opts.scale)) >> 1) ///< vertical size in pixels of the encoder output window

#define MIN_MSG_WIDTH_CHARS ((int)(40. * widget_opts.scale)) ///< min width of text on warning/error labels
#define MAX_MSG_WIDTH_CHARS ((int)(200. * widget_opts.scale)) ///< max width of text on warning/error labels

/// size of the fx dialog windows scrollwindow
#define RFX_WINSIZE_H ((int)(GUI_SCREEN_WIDTH >= SCREEN_SCALE_DEF_WIDTH ? 210. * (1. + widget_opts.scale) : DEF_FRAME_HSIZE_43S_UNSCALED))
#define RFX_WINSIZE_V ((int)(DEF_FRAME_VSIZE_43S_UNSCALED * widget_opts.scale))

#define DEF_BUTTON_WIDTH ((int)(80. * widget_opts.scale))

#define DEF_DIALOG_WIDTH RFX_WINSIZE_H
#define DEF_DIALOG_HEIGHT RFX_WINSIZE_V

#define LIVES_MAIN_WINDOW_WIDGET (mainw->LiVES)

////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ALL_USED -1

/// number of function keys
#define FN_KEYS 12

/// FX keys, 1 - 9 normally
#define FX_KEYS_PHYSICAL 9
#define FX_KEYS_PHYSICAL_EXTRA FX_KEYS_PHYSICAL + 2

/// must be >= FX_KEYS_PHYSICAL, and <=64 (number of bits in a 64bit int mask)
/// (max number of keys accesible through rte window or via OSC)
#define FX_KEYS_MAX_VIRTUAL 64 ///< the "metaphysical" keys

/// the rest of the keys are accessible through the multitrack renderer (must, be > FX_KEYS_MAX_VIRTUAL)
#define FX_KEYS_MAX 65536

#define EFFECT_NONE 0
#define GU641 ((uint64_t)1)

#define MAX_FX_THREADS 1024 ///< may be increased if necessary

#define LIVES_DCLICK_TIME 400 ///< double click time (milliseconds)

/// external control types
typedef enum {
  EXT_CNTL_NONE = -1, ///< not used
  EXT_CNTL_JS,
  EXT_CNTL_MIDI,
  MAX_EXT_CNTL
} lives_ext_cntl_t;

/// timebase sources
typedef enum {
  LIVES_TIME_SOURCE_NONE = 0,
  LIVES_TIME_SOURCE_SYSTEM,
  LIVES_TIME_SOURCE_SOUNDCARD,
  LIVES_TIME_SOURCE_EXTERNAL
} lives_time_source_t;

/// toy types
typedef enum {
  LIVES_TOY_NONE = 0,
  LIVES_TOY_MAD_FRAMES,
  LIVES_TOY_TV
} lives_toy_t;

typedef enum {
  LIVES_DIALOG_INFO,
  LIVES_DIALOG_ERROR,
  LIVES_DIALOG_WARN,
  LIVES_DIALOG_WARN_WITH_CANCEL,
  LIVES_DIALOG_YESNO,
  LIVES_DIALOG_QUESTION,
  LIVES_DIALOG_ABORT_OK,
  LIVES_DIALOG_ABORT_RETRY,
  LIVES_DIALOG_ABORT_CANCEL_RETRY,
  LIVES_DIALOG_CANCEL_RETRY_BROWSE
} lives_dialog_t;

/// various return conditions from rendering (multitrack or after recording)
typedef enum {
  LIVES_RENDER_ERROR_NONE = 0,
  LIVES_RENDER_READY,
  LIVES_RENDER_PROCESSING,
  LIVES_RENDER_EFFECTS_PAUSED,
  LIVES_RENDER_COMPLETE,
  LIVES_RENDER_WARNING,
  LIVES_RENDER_WARNING_READ_FRAME,
  LIVES_RENDER_ERROR,
  LIVES_RENDER_ERROR_READ_AUDIO,
  LIVES_RENDER_ERROR_WRITE_AUDIO,
  LIVES_RENDER_ERROR_WRITE_FRAME,
} lives_render_error_t;

#define DVD_AUDIO_CHAN_MIN 128
#define DVD_AUDIO_CHAN_DEFAULT 128
#define DVD_AUDIO_CHAN_MAX 159

enum {
  LIVES_DEVICE_CD = 0,
  LIVES_DEVICE_DVD, // 1
  LIVES_DEVICE_VCD, // 2
  LIVES_DEVICE_INTERNAL, // 2
  LIVES_DEVICE_TV_CARD, // 4
  LIVES_DEVICE_FW_CARD, // 5
};

#define THEME_DETAIL_NAME "theme_name"
#define THEME_DETAIL_STYLE "theme_style"
#define THEME_DETAIL_SEPWIN_IMAGE "sepwin_image"
#define THEME_DETAIL_FRAMEBLANK_IMAGE "frameblank_image"
#define THEME_DETAIL_NORMAL_FORE "normal_fore"
#define THEME_DETAIL_NORMAL_BACK "normal_back"
#define THEME_DETAIL_ALT_FORE "alt_fore"
#define THEME_DETAIL_ALT_BACK "alt_back"
#define THEME_DETAIL_INFO_TEXT "info_text"
#define THEME_DETAIL_INFO_BASE "info_base"

#define THEME_DETAIL_AUDCOL "audcol"
#define THEME_DETAIL_VIDCOL "vidcol"
#define THEME_DETAIL_FXCOL "fxcol"
#define THEME_DETAIL_MT_TLREG "mt_tlreg"
#define THEME_DETAIL_MT_MARK "mt_mark"
#define THEME_DETAIL_MT_EVBOX "mt_evbox"
#define THEME_DETAIL_MT_TCFG "mt_timecode_fg"
#define THEME_DETAIL_MT_TCBG "mt_timecode_bg"
#define THEME_DETAIL_FRAME_SURROUND "frame_surround"
#define THEME_DETAIL_CE_SEL "ce_sel"
#define THEME_DETAIL_CE_UNSEL "ce_unsel"

/// set in set_palette_colours()
typedef struct {
  int style;
#define STYLE_PLAIN 0 ///< no theme (theme 'none')
#define STYLE_1 (1<<0) ///< turn on theming if set
#define STYLE_2 (1<<1) ///< colour the spinbuttons on the front page if set
#define STYLE_3 (1<<2) ///< style is lightish - allow themeing of widgets with dark text, otherwise use menu bg
#define STYLE_4 (1<<3) ///< separator col. in mt

#define STYLE_LIGHT STYLE_3

  LiVESWidgetColor white;
  LiVESWidgetColor black;
  LiVESWidgetColor pink;
  LiVESWidgetColor light_red;
  LiVESWidgetColor light_green;
  LiVESWidgetColor dark_red;
  LiVESWidgetColor dark_orange;

  LiVESWidgetColor grey20;
  LiVESWidgetColor grey25;
  LiVESWidgetColor grey45;
  LiVESWidgetColor grey60;
  LiVESWidgetColor fade_colour;

  LiVESWidgetColor banner_fade_text;

  // set via theme API

  LiVESWidgetColor normal_back;
  LiVESWidgetColor normal_fore;

  LiVESWidgetColor menu_and_bars;
  LiVESWidgetColor menu_and_bars_fore;
  LiVESWidgetColor info_text;
  LiVESWidgetColor info_base;

  LiVESWidgetColor mt_timecode_bg;
  LiVESWidgetColor mt_timecode_fg;

  lives_colRGBA64_t audcol;
  lives_colRGBA64_t vidcol;
  lives_colRGBA64_t fxcol;
  lives_colRGBA64_t mt_timeline_reg;

  lives_colRGBA64_t frame_surround;
  lives_colRGBA64_t mt_mark;
  lives_colRGBA64_t mt_evbox;

  lives_colRGBA64_t ce_sel;
  lives_colRGBA64_t ce_unsel;
} _palette;

/// screen details
typedef struct {
  int x;
  int y;
  int width;
  int height;
  int phys_width;
  int phys_height;
  LiVESXDevice *mouse_device; ///< unused for gtk+ < 3.0.0
  LiVESXDisplay *disp;
  LiVESXScreen *screen;
  double dpi;
  double scale;
} lives_mgeometry_t;

/// constant strings
enum {
  LIVES_STRING_CONSTANT_ANY = 0,
  LIVES_STRING_CONSTANT_NONE,
  LIVES_STRING_CONSTANT_RECOMMENDED,
  LIVES_STRING_CONSTANT_DISABLED,
  LIVES_STRING_CONSTANT_CL,  ///< "the current layout"
  LIVES_STRING_CONSTANT_BUILTIN,
  LIVES_STRING_CONSTANT_CUSTOM,
  LIVES_STRING_CONSTANT_TEST,
  NUM_LIVES_STRING_CONSTANTS
};

// executables
#define EXEC_SMOGRIFY "smogrify"
#define EXEC_PERL "perl"
#define EXEC_PYTHON "python"
#define EXEC_YOUTUBE_DL "youtube-dl"
#define EXEC_MPLAYER "mplayer"
#define EXEC_MPLAYER2 "mplayer2"
#define EXEC_MPV "mpv"
#define EXEC_JACKD "jackd"
#define EXEC_PULSEAUDIO "pulseaudio"
#define EXEC_CONVERT "convert"
#define EXEC_COMPOSITE "composite"
#define EXEC_IDENTIFY "identify"
#define EXEC_PLAY "play"
#define EXEC_SOX "sox"
#define EXEC_DVGRAB "dvgrab"
#define EXEC_CDDA2WAV "cdda2wav"
#define EXEC_ICEDAX "icedax"
#define EXEC_GDB "gdb"
#define EXEC_SSH_ASKPASS "ssh-askpass"
#define EXEC_XWININFO "xwininfo"
#define EXEC_GCONFTOOL_2 "gconftool-2"
#define EXEC_XDG_SCREENSAVER "xdg-screensaver"
#define EXEC_MIDISTART "midistart"
#define EXEC_MIDISTOP "midistop"
#define EXEC_AUTOLIVES_PL "autolives.pl"
#define EXEC_MD5SUM "md5sum"
#define EXEC_WMCTRL "wmctrl"

// image types (string)
#define LIVES_IMAGE_TYPE_JPEG "jpeg"
#define LIVES_IMAGE_TYPE_PNG "png"

// audio types (string)
#define LIVES_AUDIO_TYPE_PCM "pcm"

// file extensions
#define LIVES_FILE_EXT_TMP "tmp"
#define LIVES_FILE_EXT_PNG "png"
#define LIVES_FILE_EXT_JPG "jpg"
#define LIVES_FILE_EXT_MGK "mgk"
#define LIVES_FILE_EXT_PRE "pre"
#define LIVES_FILE_EXT_NEW "new"
#define LIVES_FILE_EXT_MAP "map"
#define LIVES_FILE_EXT_SCRAP "scrap"
#define LIVES_FILE_EXT_TEXT "txt"
#define LIVES_FILE_EXT_BAK "bak"
#define LIVES_FILE_EXT_WEBM "webm"
#define LIVES_FILE_EXT_MP4 "mp4"

#define LIVES_FILE_EXT_BACKUP "lv1"
#define LIVES_FILE_EXT_PROJECT "lv2"

#define LIVES_FILE_EXT_TAR_GZ "tar.gz"

#define LIVES_FILE_EXT_SRT "srt"
#define LIVES_FILE_EXT_SUB "sub"

#define LIVES_FILE_EXT_PCM "pcm"
#define LIVES_FILE_EXT_WAV "wav"

#define LIVES_FILE_EXT_LAYOUT "lay"

#define LIVES_FILE_EXT_RFX_SCRIPT "script"

// file names
#define CLIP_AUDIO_FILENAME "audio"
#define CLIP_TEMP_AUDIO_FILENAME "audiodump." LIVES_FILE_EXT_PCM

#define WORKDIR_LITERAL "workdir"
#define WORKDIR_LITERAL_LEN 7

#define HEADER_LITERAL "header"

#define THEME_LITERAL "theme"
#define THEME_SEP_IMG_LITERAL "main"
#define THEME_FRAME_IMG_LITERAL "frame"
#define THEME_HEADER HEADER_LITERAL "." THEME_LITERAL
#define THEME_HEADER_2 THEME_HEADER "_gtk2"

#define LIVES_THEME_CAMERA "camera"

#define LIVES_CLIP_HEADER HEADER_LITERAL ".lives"
#define LIVES_CLIP_HEADER_NEW HEADER_LITERAL "." LIVES_FILE_EXT_NEW
#define LIVES_CLIP_HEADER_OLD HEADER_LITERAL
#define LIVES_CLIP_HEADER_OLD2 LIVES_CLIP_HEADER_OLD "2"

#define FX_DEFS_FILENAME "fxdefs"
#define FX_SIZES_FILENAME "fxsizes"

#define FX_DEFS_VERSIONSTRING_1_1 "LiVES filter defaults file version 1.1"
#define FX_SIZES_VERSIONSTRING_2 "LiVES generator default sizes file version 2"

#define SUBS_FILENAME "subs"

#define CLIP_ORDER_FILENAME "order"

#define SET_LOCK_FILENAME "lock"

#define LAYOUT_FILENAME "layout"
#define LAYOUT_MAP_FILENAME LAYOUT_FILENAME "."  LIVES_FILE_EXT_MAP
#define LAYOUT_NUMBERING_FILENAME LAYOUT_FILENAME "_numbering"

// directory names
#define LAYOUTS_DIRNAME "layouts"
#define CLIPS_DIRNAME "clips"
#define IMPORTS_DIRNAME "imports"

// special files / dirs

#define SET_LOCK_FILE(set_name, lockfile) lives_build_filename(prefs->workdir, set_name, lockfile, NULL);

#define SET_LOCK_FILES(set_name) SET_LOCK_FILE(set_name, SET_LOCK_FILENAME);

// directory where we store 1 clip / all clips if handle is NULL
#define MAKE_CLIPS_DIRNAME(set, handle) lives_build_filename(prefs->workdir, set, CLIPS_DIRNAME, handle, NULL);

// directory of a clip in the current set
#define CLIPDIR(handle) MAKE_CLIPS_DIRNAME(mainw->set_name, handle)

// directory for all clips in set
#define CLIPS_DIR(set) MAKE_CLIPS_DIRNAME(set, NULL)

// filters

#define LIVES_SUBS_FILTER  {"*.srt", "*.sub", NULL}
#define LIVES_AUDIO_LOAD_FILTER  {"*.it", "*.mp3", "*.wav", "*.ogg", "*.mod", "*.xm", "*.wma", "*.flac", NULL}
#define LIVES_TV_CARD_TYPES  {"v4l2", "v4l", "bsdbt848", "dummy", "*autodetect", "yv12", "*", "rgb32", "rgb24", "rgb16", \
      "rgb15", "uyvy", "yuy2", "i2420", NULL}

#define NUM_VOL_LIGHTS 10 ///< unused

/* actions */
#define UNMATCHED -1
#define START_PLAYBACK 0
#define STOP_PLAYBACK 1
#define CLIP_SELECT 2
#define PLAY_FORWARDS 3
#define PLAY_BACKWARDS 4
#define REVERSE_PLAYBACK 5
#define PLAY_FASTER 6
#define PLAY_SLOWER 7
#define TOGGLE_FREEZE 8
#define SET_FRAMERATE 9
#define START_RECORDING 10
#define STOP_RECORDING 11
#define TOGGLE_RECORDING 12
#define SWAP_FOREGROUND_BACKGROUND 13
#define RESET_EFFECT_KEYS 14
#define ENABLE_EFFECT_KEY 15
#define DISABLE_EFFECT_KEY 16
#define TOGGLE_EFFECT_KEY 17
#define SET_PARAMETER_VALUE 18
#define NEXT_CLIP_SELECT 19
#define PREV_CLIP_SELECT 20
#define SET_FPS_RATIO 21
#define RETRIGGER_CLIP 22
#define NEXT_MODE_CYCLE 23
#define PREV_MODE_CYCLE 24
#define SET_VPP_PARAMETER_VALUE 25
#define OSC_NOTIFY 26

typedef struct {
  double top;
  double left;
  double width;
  double height;
} lives_rect_t;

typedef struct {
  char *name;
  lives_rect_t *rects; // for future use
  int z_index; // for future use
} lives_screen_area_t;

typedef struct {
  ticks_t tleft;
  volatile ticks_t lastcheck;
}  lives_timeout_t;

typedef int lives_alarm_t;

/// where do we add the builtin tools in the tools menu
#define RFX_TOOL_MENU_POSN 2

/// mainw->msg bytesize
#define MAINW_MSG_SIZE 8192

typedef struct {
  char msg[MAINW_MSG_SIZE];

  // files
  int current_file;
  int first_free_file;
  lives_clip_t *files[MAX_FILES + 1]; ///< +1 for the clipboard
  char vid_load_dir[PATH_MAX];
  char vid_save_dir[PATH_MAX];
  char vid_dl_dir[PATH_MAX];
  char audio_dir[PATH_MAX];
  char image_dir[PATH_MAX];
  char proj_load_dir[PATH_MAX];
  char proj_save_dir[PATH_MAX];
  char recent_file[PATH_MAX];
  int untitled_number;
  int cap_number;
  int clips_available;

  /// hash table of clips in menu order
  LiVESList *cliplist;

  LiVESSList *clips_group;

  /// sets
#define MAX_SET_NAME_LEN 128
  char set_name[256];   // actually 128 is the (soft) limit now, filesystem encoding

  // playback
  boolean faded;
  boolean double_size;
  boolean sep_win;
  boolean fs;
  boolean loop;
  volatile boolean loop_cont;
  volatile boolean ping_pong;
  boolean loop_locked;
  boolean mute;
  int audio_start;
  int audio_end;

  boolean ext_playback; ///< using external video playback plugin
  volatile boolean ext_audio; ///< using external video playback plugin to stream audio

  int ptr_x;
  int ptr_y;

  double fps_measure; ///< show fps stats after playback
  frames_t fps_mini_measure; ///< show fps stats during playback
  ticks_t fps_mini_ticks;
  // flags
  boolean save_with_sound;
  boolean ccpd_with_sound;
  boolean selwidth_locked;
  boolean is_ready;
  boolean configured;
  boolean fatal; ///< got fatal signal
  boolean opening_loc;  ///< opening location (streaming)
  boolean dvgrab_preview;
  boolean switch_during_pb;
  boolean clip_switched; ///< for recording - did we switch clips ?
  volatile boolean record;

  boolean in_fs_preview;
  volatile lives_cancel_t cancelled;

  boolean error;

  lives_cancel_type_t cancel_type;

  weed_plant_t *event_list; ///< current event_list, for recording
  weed_plant_t *stored_event_list; ///< stored mt -> clip editor
  boolean stored_event_list_changed;
  boolean stored_event_list_auto_changed;
  boolean stored_layout_save_all_vals;
  char stored_layout_name[PATH_MAX];

  LiVESList *stored_layout_undos;
  size_t sl_undo_buffer_used;
  unsigned char *sl_undo_mem;
  int sl_undo_offset;

  LiVESList *new_lmap_errors;

  short endian;

  int pwidth; ///< playback width in RGB pixels
  int pheight; ///< playback height

  volatile lives_whentostop_t whentostop;

  int play_start;
  int play_end;
  boolean playing_sel;
  boolean preview;
  boolean preview_rendering;

  boolean is_processing;
  boolean is_rendering;
  boolean resizing;

  boolean foreign;  ///< for external window capture
  boolean record_foreign;
  boolean t_hidden;

  // recording from an external window
  uint32_t foreign_key;

#ifdef GUI_GTK
#if GTK_CHECK_VERSION(3, 0, 0)
  Window foreign_id;
#else
  GdkNativeWindow foreign_id;
  GdkColormap *foreign_cmap;
  GdkPixmap *foreign_map;
#endif
#else
  Window foreign_id;
#endif

  LiVESXWindow *foreign_window;
  int foreign_width;
  int foreign_height;
  int foreign_bpp;
  char *foreign_visual;

  /// some VJ effects
  boolean nervous;

  lives_rfx_t *rendered_fx;
  int num_rendered_effects_builtin;
  int num_rendered_effects_custom;
  int num_rendered_effects_test;

  // for the merge dialog
  int last_transition_idx;
  int last_transition_loops;
  boolean last_transition_loop_to_fit;
  boolean last_transition_align_start;
  boolean last_transition_ins_frames;

  volatile uint64_t rte; ///< current max for VJ mode == 64 effects on fg clip

  uint32_t last_grabbable_effect;
  int rte_keys; ///< which effect is bound to keyboard (m) modechange and ctrl-alt-up-arrow / ctrl-alt-down-arrow param changes
  int num_tr_applied; ///< number of transitions active
  double blend_factor; ///< keyboard control parameter

  int blend_file;
  int last_blend_file;
  volatile int blend_palette;
  int blend_width;
  int blend_height;
  int blend_clamping;
  int blend_sampling;
  int blend_subspace;
  int blend_gamma;

  int scrap_file; ///< we throw odd sized frames here when recording in real time; used if a source is a generator or stream

  int ascrap_file; ///< scrap file for recording audio scraps

  /// which number file we are playing (or -1)
  volatile int playing_file;

  int pre_src_file; ///< video file we were playing before any ext input started
  int pre_src_audio_file; ///< audio file we were playing before any ext input started

  int swapped_clip; ///< maintains the current cliplist postion even if we swap fg and bg clips

  lives_toy_t toy_type;
  lives_pgid_t alives_pgid; // 0, or procid for autolives

  boolean toy_go_wild;

  // copy/paste
  boolean insert_after;
  boolean with_sound;

  // selection
  int sel_start;
  short sel_move;

  /// which bar should we move ?
#define SEL_MOVE_START 1
#define SEL_MOVE_END 2
#define SEL_MOVE_AUTO 3
#define SEL_MOVE_SINGLE 4

  // prefs (Save on exit)
  int prefs_changed;

  boolean prefs_need_restart;

  /// default sizes for when no file is loaded
  int def_width;
  int def_height;

  /////////////////////////////////////////////////

  // end of static-ish info
  int old_scr_width;
  int old_scr_height;

  boolean leave_files;  ///< TRUE to leave clip files on disk even when closing (default FALSE)
  boolean was_set;

  /// extra parameters for opening special files
  char *file_open_params;
  boolean open_deint;

  int last_dprint_file;
  boolean no_switch_dprint;

  /// actual frame being displayed
  int actual_frame;

  /// and the audio 'frame' for when we are looping
  double aframeno;

  // ticks are measured in 1/TICKS_PER_SECOND_DBL of a second (by defalt a tick is 10 nano seconds)

  // for the internal player
  double period; ///< == 1./cfile->pb_fps (unless cfile->pb_fps is 0.)
  volatile ticks_t startticks; ///< effective ticks when last frame was (should have been) displayed
  ticks_t timeout_ticks; ///< incremented if effect/rendering is paused/previewed
  ticks_t origsecs; ///< playback start seconds - subtracted from all other ticks to keep numbers smaller
  ticks_t origusecs; ///< usecs at start of playback - ditto
  ticks_t offsetticks; ///< offset for multitrack playback start
  volatile ticks_t currticks; ///< current playback ticks (relative)
  ticks_t deltaticks; ///< deltaticks for scratching
  ticks_t adjticks; ///< used to equalise the timecode between alternate timer sources
  ticks_t firstticks; ///< ticks when audio started playing (for non-realtime audio plugins)
  ticks_t stream_ticks;  ///< ticks since first frame sent to playback plugin
  ticks_t last_display_ticks; /// currticks when last display was shown (used for fixed fps)
  boolean frame_layer_preload_final; /// frame loaded from cache has fx appliead already: don't re-apply
  int play_sequence; ///< incremented for each playback

  double audio_stretch;

  int size_warn; ///< warn the user that incorrectly sized frames were found

  /// set to TRUE during frame load/display operation. If TRUE we should not switch clips,
  /// close the current clip, or call load_frame_image()
  volatile boolean noswitch;
  int new_clip;

  int aud_file_to_kill; ///< # of audio file to kill on crash

  boolean reverse_pb; ///< used in osc.c

  /// TODO - make this a mutex and more finely grained : things we need to block are (clip switches, clip closure, effects on/off, etc)
  boolean osc_block;

  int osc_auto; ///< bypass user choices automatically

  /// encode width, height and fps set externally
  int osc_enc_width;
  int osc_enc_height;
  float osc_enc_fps;

  /// fixed fps playback; usually fixed_fpsd==0.
  int fixed_fps_numer;
  int fixed_fps_denom;
  double fixed_fpsd; ///< <=0. means free playback

  /// video playback plugin was updated; write settings to a file
  boolean write_vpp_file;

  volatile short scratch;
#define SCRATCH_NONE 0
#define SCRATCH_BACK -1
#define SCRATCH_FWD 1
#define SCRATCH_JUMP 2

  /// internal fx
  boolean internal_messaging;
  lives_render_error_t (*progress_fn)(boolean reset);

  volatile boolean threaded_dialog;

  // fx controls
  double fx1_val;
  double fx2_val;
  double fx3_val;
  double fx4_val;
  double fx5_val;
  double fx6_val;

  int fx1_start;
  int fx2_start;
  int fx3_start;
  int fx4_start;

  int fx1_step;
  int fx2_step;
  int fx3_step;
  int fx4_step;

  int fx1_end;
  int fx2_end;
  int fx3_end;
  int fx4_end;

  boolean fx1_bool;
  boolean fx2_bool;
  boolean fx3_bool;
  boolean fx4_bool;
  boolean fx5_bool;
  boolean fx6_bool;

  boolean effects_paused;
  boolean did_rfx_preview;

  uint32_t kb_timer;

  //function pointers
  ulong config_func;
  ulong pb_fps_func;
  ulong spin_start_func;
  ulong spin_end_func;
  ulong record_perf_func;
  ulong vidbar_func;
  ulong laudbar_func;
  ulong raudbar_func;
  ulong toy_func_none;
  ulong toy_func_random_frames;
  ulong toy_func_lives_tv;
  ulong hnd_id;
  ulong loop_cont_func;
  ulong mute_audio_func;
  ulong fullscreen_cb_func;
  ulong sepwin_cb_func;
  ulong fsp_func; ///< fileselector preview expose (for image thumbnails)
  ulong sw_func; ///< scrolledwindow expose func
  ulong vj_mode_func;

  lives_funcptr_t abort_hook_func;

  // for jack transport
  boolean jack_can_stop;
  boolean jack_can_start;

  volatile boolean video_seek_ready;

  // selection pointers
  ulong mouse_fn1;
  boolean mouse_blocked;

  /// stored clips
  int clipstore[FN_KEYS - 1][2];

  /// key function for autorepeat ctrl-arrows
  uint32_t ksnoop;

  lives_mt *multitrack;

  int new_blend_file;

  LiVESWidget *frame1;
  LiVESWidget *frame2;
  LiVESWidget *freventbox0;
  LiVESWidget *freventbox1;
  LiVESWidget *playframe;
  LiVESWidget *pl_eventbox;
  LiVESWidget *pf_grid;
  LiVESPixbuf *imframe;
  LiVESPixbuf *camframe;
  LiVESPixbuf *imsep;
  LiVESWidget *LiVES;
  LiVESWidget *open;
  LiVESWidget *open_sel;
  LiVESWidget *open_vcd_menu;
  LiVESWidget *open_vcd_submenu;
  LiVESWidget *open_vcd;
  LiVESWidget *open_dvd;
  LiVESWidget *open_loc;
  LiVESWidget *open_utube;
  LiVESWidget *open_loc_menu;
  LiVESWidget *open_loc_submenu;
  LiVESWidget *open_yuv4m;
  LiVESWidget *open_lives2lives;
  LiVESWidget *send_lives2lives;
  LiVESWidget *open_device_menu;
  LiVESWidget *open_device_submenu;
  LiVESWidget *open_firewire;
  LiVESWidget *open_hfirewire;
  LiVESWidget *unicap;
  LiVESWidget *firewire;
  LiVESWidget *tvdev;
  LiVESWidget *recent_menu;
  LiVESWidget *recent_submenu;
  LiVESWidget *recent[N_RECENT_FILES];
  LiVESWidget *save_as;
#ifdef LIBAV_TRANSCODE
  LiVESWidget *transcode;
#endif
  LiVESWidget *backup;
  LiVESWidget *restore;
  LiVESWidget *save_selection;
  LiVESWidget *close;
  LiVESWidget *import_proj;
  LiVESWidget *export_proj;
  LiVESWidget *import_theme;
  LiVESWidget *export_theme;
  LiVESWidget *sw_sound;
  LiVESWidget *clear_ds;
  LiVESWidget *ccpd_sound;
  LiVESWidget *quit;
  LiVESWidget *undo;
  LiVESWidget *redo;
  LiVESWidget *copy;
  LiVESWidget *cut;
  LiVESWidget *insert;
  LiVESWidget *paste_as_new;
  LiVESWidget *merge;
  LiVESWidget *xdelete;
  LiVESWidget *select_submenu;
  LiVESWidget *select_all;
  LiVESWidget *select_new;
  LiVESWidget *select_to_end;
  LiVESWidget *select_to_aend;
  LiVESWidget *select_from_start;
  LiVESWidget *select_start_only;
  LiVESWidget *select_end_only;
  LiVESWidget *select_last;
  LiVESWidget *select_invert;
  LiVESWidget *lock_selwidth;
  LiVESWidget *record_perf;
  LiVESWidget *playall;
  LiVESWidget *playsel;
  LiVESWidget *playclip;
  LiVESWidget *rev_clipboard;
  LiVESWidget *stop;
  LiVESWidget *rewind;
  LiVESWidget *full_screen;
  LiVESWidget *loop_video;
  LiVESWidget *loop_continue;
  LiVESWidget *loop_ping_pong;
  LiVESWidget *sepwin;
  LiVESWidget *mute_audio;
  LiVESWidget *sticky;
  LiVESWidget *showfct;
  LiVESWidget *showsubs;
  LiVESWidget *letter;
  LiVESWidget *aload_subs;
  LiVESWidget *load_subs;
  LiVESWidget *erase_subs;
  LiVESWidget *fade;
  LiVESWidget *dsize;

  LiVESWidget *midi_learn;
  LiVESWidget *midi_save;
  LiVESWidget *change_speed;
  LiVESWidget *capture;
  LiVESWidget *load_audio;
  LiVESWidget *load_cdtrack;
  LiVESWidget *eject_cd;
  LiVESWidget *recaudio_submenu;
  LiVESWidget *recaudio_clip;
  LiVESWidget *recaudio_sel;
  LiVESWidget *export_submenu;
  LiVESWidget *export_allaudio;
  LiVESWidget *export_selaudio;
  LiVESWidget *append_audio;
  LiVESWidget *trim_submenu;
  LiVESWidget *trim_audio;
  LiVESWidget *trim_to_pstart;
  LiVESWidget *delaudio_submenu;
  LiVESWidget *delsel_audio;
  LiVESWidget *delall_audio;
  LiVESWidget *ins_silence;
  LiVESWidget *fade_aud_in;
  LiVESWidget *fade_aud_out;
  LiVESWidget *resample_audio;
  LiVESWidget *adj_audio_sync;
  LiVESWidget *resample_video;
  LiVESWidget *preferences;
  LiVESWidget *rename;
  LiVESWidget *toy_none;
  LiVESWidget *toy_random_frames;
  LiVESWidget *toy_tv;
  LiVESWidget *autolives;
  LiVESWidget *show_file_info;
  LiVESWidget *show_file_comments;
  LiVESWidget *show_clipboard_info;
  LiVESWidget *show_messages;
  LiVESWidget *show_layout_errors;
  LiVESWidget *sel_label;
  LiVESAccelGroup *accel_group;
  LiVESWidget *sep_image;
  LiVESWidget *hruler;
  LiVESWidget *vj_save_set;
  LiVESWidget *vj_load_set;
  LiVESWidget *vj_show_keys;
  LiVESWidget *rte_defs_menu;
  LiVESWidget *rte_defs;
  LiVESWidget *save_rte_defs;
  LiVESWidget *vj_reset;
  LiVESWidget *vj_realize;
  LiVESWidget *vj_mode;
  LiVESWidget *show_devopts;
  LiVESWidget *mt_menu;
  LiVESWidget *troubleshoot;
  LiVESWidget *export_custom_rfx;
  LiVESWidget *delete_custom_rfx;
  LiVESWidget *edit_test_rfx;
  LiVESWidget *rename_test_rfx;
  LiVESWidget *delete_test_rfx;
  LiVESWidget *promote_test_rfx;

  LiVESWidget *vol_checkbuttons[NUM_VOL_LIGHTS][2];

  /// for the fileselection preview
  LiVESWidget *fs_playarea;
  LiVESWidget *fs_playalign;
  LiVESWidget *fs_playframe;
  LiVESWidget *fs_playimg;

  /// for the framedraw special widget - TODO - use a sub-struct
  LiVESWidget *framedraw; ///< the eventbox
  LiVESWidget *framedraw_reset; ///< the 'redraw' button
  LiVESWidget *framedraw_preview; ///< the 'redraw' button
  LiVESWidget *framedraw_spinbutton; ///< the frame number button
  LiVESWidget *framedraw_scale; ///< the slider
  LiVESWidget *framedraw_maskbox; ///< box for opacity controls
  LiVESWidget *framedraw_opscale; ///< opacity
  LiVESWidget *framedraw_cbutton; ///< colour for mask
  LiVESWidget *fd_frame; ///< surrounding frame widget

  weed_plant_t *fd_layer_orig; ///< original layer uneffected
  weed_plant_t *fd_layer; ///< framedraw preview layer

  int framedraw_frame; ///< current displayed frame
  int fd_max_frame; ///< max effected / generated frame

  ulong fd_spin_func;

  LiVESWidget *hbox3;  ///< hbox with start / end spins and selection label (C.E.)

  // bars here -> actually text above bars
  LiVESWidget *vidbar;
  LiVESWidget *laudbar;
  LiVESWidget *raudbar;

  LiVESWidget *spinbutton_end;
  LiVESWidget *spinbutton_start;

  LiVESWidget *sa_button;
  LiVESWidget *sa_hbox;
  LiVESWidget *sa_toolbar;

  LiVESWidget *arrow1;
  LiVESWidget *arrow2;

  lives_cursor_t cursor_style;

  weed_plant_t *filter_map; // the video filter map for rendering
  weed_plant_t *afilter_map; // the audio filter map for renering
  weed_plant_t *audio_event; // event for audio render tracking
  void ** *pchains; // parameter value chains for interpolation

  // for the internal player
  LiVESWidget *play_image;
  LiVESWidget *play_window;
  weed_plant_t *frame_layer;
  weed_plant_t *frame_layer_preload;
  int64_t pred_frame;
  int pred_clip;
  weed_plant_t *blend_layer;
  LiVESWidget *plug;

  // frame preview in the separate window
  LiVESWidget *preview_box;
  LiVESWidget *preview_image;
  LiVESWidget *preview_spinbutton;
  LiVESWidget *preview_scale;
  LiVESWidget *preview_hbox;
  int preview_frame;
  ulong preview_spin_func;
  int prv_link;
#define PRV_FREE 0
#define PRV_START 1
#define PRV_END 2
#define PRV_PTR 3
#define PRV_DEFAULT PRV_PTR

  LiVESWidget *start_image;
  LiVESWidget *end_image;
  LiVESWidget *playarea;
  LiVESWidget *hseparator;
  LiVESWidget *message_box;
  LiVESWidget *msg_area;
  LiVESWidget *msg_scrollbar;
  LiVESAdjustment *msg_adj;

  LiVESWidget *clipsmenu;
  LiVESWidget *eventbox;
  LiVESWidget *eventbox2;
  LiVESWidget *eventbox3;
  LiVESWidget *eventbox4;
  LiVESWidget *eventbox5;

  // toolbar buttons
  LiVESWidget *t_stopbutton;
  LiVESWidget *t_bckground;
  LiVESWidget *t_fullscreen;
  LiVESWidget *t_sepwin;
  LiVESWidget *t_double;
  LiVESWidget *t_infobutton;

  LiVESWidget *t_slower;
  LiVESWidget *t_faster;
  LiVESWidget *t_forward;
  LiVESWidget *t_back;

  LiVESWidget *t_hide;

  LiVESWidget *toolbar;
  LiVESWidget *tb_hbox;
  LiVESWidget *fs1;
  LiVESWidget *top_vbox;

  LiVESWidget *l1_tb;
  LiVESWidget *l2_tb;
  LiVESWidget *l3_tb;

  LiVESWidget *int_audio_checkbutton;
  LiVESWidget *ext_audio_checkbutton;

  ulong int_audio_func;
  ulong ext_audio_func;

  LiVESWidget *volume_scale;
  LiVESWidget *vol_toolitem;
  LiVESWidget *vol_label;

  // menubar buttons
  LiVESWidget *btoolbar; ///< button toolbar - clip editor
  LiVESWidget *m_sepwinbutton;
  LiVESWidget *m_playbutton;
  LiVESWidget *m_stopbutton;
  LiVESWidget *m_playselbutton;
  LiVESWidget *m_rewindbutton;
  LiVESWidget *m_loopbutton;
  LiVESWidget *m_mutebutton;
  LiVESWidget *menu_hbox;
  LiVESWidget *menubar;

  // separate window
  int opwx;
  int opwy;

  // sepwin buttons
  LiVESWidget *preview_controls;
  LiVESWidget *p_playbutton;
  LiVESWidget *p_playselbutton;
  LiVESWidget *p_rewindbutton;
  LiVESWidget *p_loopbutton;
  LiVESWidget *p_mutebutton;
  LiVESWidget *p_mute_img;

  // timer bars
  LiVESWidget *video_draw;
  LiVESWidget *laudio_draw;
  LiVESWidget *raudio_draw;

  lives_painter_surface_t *video_drawable;
  lives_painter_surface_t *laudio_drawable;
  lives_painter_surface_t *raudio_drawable;
  lives_painter_surface_t *blank_laudio_drawable;
  lives_painter_surface_t *blank_raudio_drawable;

  // framecounter
  LiVESWidget *framebar;
  LiVESWidget *framecounter;
  LiVESWidget *spinbutton_pb_fps;
  LiVESWidget *vps_label;
  LiVESWidget *banner;

  // (sub)menus
  LiVESWidget *files_menu;
  LiVESWidget *edit_menu;
  LiVESWidget *play_menu;
  LiVESWidget *effects_menu;
  LiVESWidget *tools_menu;
  LiVESWidget *audio_menu;
  LiVESWidget *info_menu;
  LiVESWidget *advanced_menu;
  LiVESWidget *vj_menu;
  LiVESWidget *toys_menu;
  LiVESWidget *help_menu;

  // rendered effects
  LiVESWidget *utilities_menu;
  LiVESWidget *utilities_submenu;
  LiVESWidget *gens_menu;
  LiVESWidget *gens_submenu;
  LiVESWidget *run_test_rfx_submenu;
  LiVESWidget *run_test_rfx_menu;
  LiVESWidget *custom_effects_menu;
  LiVESWidget *custom_effects_submenu;
  LiVESWidget *custom_effects_separator;
  LiVESWidget *custom_tools_menu;
  LiVESWidget *custom_tools_submenu;
  LiVESWidget *custom_tools_separator;
  LiVESWidget *custom_gens_menu;
  LiVESWidget *custom_gens_submenu;
  LiVESWidget *custom_utilities_menu;
  LiVESWidget *custom_utilities_submenu;
  LiVESWidget *custom_utilities_separator;
  LiVESWidget *rte_separator;

  int num_tracks;
  int *clip_index;
  int64_t *frame_index;

  LiVESWidget *resize_menuitem;

  boolean close_keep_frames; ///< special value for when generating to clipboard
  boolean only_close; ///< only close clips - do not exit
  volatile boolean is_exiting; ///< set during shutdown (inverse of only_close then)

  ulong pw_scroll_func;
  boolean msg_area_configed;

#ifdef ENABLE_JACK
  jack_driver_t *jackd; ///< jack audio playback device
  jack_driver_t *jackd_read; ///< jack audio recorder device
  boolean jack_inited;
#define RT_AUDIO
#else
  void *jackd;  ///< dummy
  void *jackd_read; ///< dummy
#endif

#ifdef HAVE_PULSE_AUDIO
  pulse_driver_t *pulsed; ///< pulse audio playback device
  pulse_driver_t *pulsed_read; ///< pulse audio recorder device
#define RT_AUDIO
#else
  void *pulsed;
  void *pulsed_read;
#endif

  // layouts
  LiVESTextBuffer *layout_textbuffer; ///< stores layout errors
  LiVESList *affected_layouts_map; ///< map of layouts with errors
  LiVESList *current_layouts_map; ///< map of all layouts for set

  /// list of pairs of marks in affected_layouts_map, text between them should be deleted when
  /// stored_layout is deleted
  LiVESList *affected_layout_marks;

  /// immediately (to be) affected layout maps
  LiVESList *xlays;

  LiVESList *recovery_list;
  char *recovery_file;  ///< the filename of our recover file
  boolean leave_recovery;

  boolean unordered_blocks; ///< are we recording unordered blocks ?

  boolean no_exit; ///< if TRUE, do not exit after saving set

  mt_opts multi_opts; ///< some multitrack options that survive between mt calls

  volatile int rec_aclip;
  volatile double rec_avel;
  volatile double rec_aseek;

  LiVESMemVTable alt_vtable;

  pthread_mutex_t gtk_mutex;  ///< gtk drawing mutex - no longer used

  pthread_mutex_t abuf_mutex;  ///< used to synch audio buffer request count - shared between audio and video threads
  pthread_mutex_t abuf_frame_mutex;  ///< used to synch audio buffer for generators
  pthread_mutex_t fx_mutex[FX_KEYS_MAX];  ///< used to prevent fx processing when it is scheduled for deinit
  pthread_mutex_t fxd_active_mutex; ///< prevent simultaneous writing to active_dummy by audio and video threads
  pthread_mutex_t event_list_mutex; ///< prevent simultaneous writing to event_list by audio and video threads
  pthread_mutex_t clip_list_mutex; ///< prevent adding/removing to cliplist while another thread could be reading it
  pthread_mutex_t vpp_stream_mutex; ///< prevent from writing audio when stream is closing
  pthread_mutex_t cache_buffer_mutex; ///< sync for jack playback termination
  pthread_mutex_t audio_filewriteend_mutex; ///< sync for ending writing audio to file
  pthread_mutex_t instance_ref_mutex; ///< refcounting for instances
  pthread_mutex_t exit_mutex; ///< prevent multiple threads trying to run cleanup
  pthread_rwlock_t mallopt_lock; ///< write locked to allow mallopt updates (may be uneccessary)
  pthread_mutex_t fbuffer_mutex; /// append / remove wirh file_buffer list

  volatile lives_rfx_t *vrfx_update;

  lives_fx_candidate_t
  ///< effects which can have candidates from which a delegate is selected (current examples are: audio_volume, resize)
  fx_candidates[MAX_FX_CANDIDATE_TYPES];

  LiVESList *cached_list;  ///< cache of preferences or file header file (or NULL)
  FILE *clip_header;

  LiVESList *file_buffers;

  int aud_rec_fd; ///< fd of file we are recording audio to
  double rec_end_time;
  int64_t rec_samples;
  double rec_fps;
  int rec_vid_frames;
  int rec_arate;
  int rec_achans;
  int rec_asamps;
  int rec_signed_endian;

  boolean suppress_dprint; ///< tidy up, e.g. by blocking "switched to file..." and "closed file..." messages

  char *string_constants[NUM_LIVES_STRING_CONSTANTS];
  char *any_string;  ///< localised text saying "Any", for encoder and output format
  char *none_string;  ///< localised text saying "None", for playback plugin name, etc.
  char *recommended_string;  ///< localised text saying "recommended", for encoder and output format, etc.
  char *disabled_string;  ///< localised text saying "disabled !", for playback plugin name, etc.
  char *cl_string; ///< localised text saying "*The current layout*", for layout warnings

  int opening_frames; ///< count of frames so far opened, updated after preview (currently)

  boolean show_procd; ///< override showing of "processing..." dialog

  boolean block_param_updates; ///< block visual param changes from updating real values
  boolean no_interp; ///< block interpolation (for single frame previews)

  ticks_t cevent_tc; ///< timecode of currently processing event

  boolean opening_multi; ///< flag to indicate multiple file selection

  volatile boolean record_paused; ///< pause during recording

  boolean record_starting; ///< start recording at next frame

  int img_concat_clip;  ///< when opening multiple, image files can get concatenated here (prefs->concat_images)

  /// rendered generators
  boolean gen_to_clipboard;
  boolean is_generating;

  boolean keep_pre;

  LiVESWidget *textwidget_focus;

  _vid_playback_plugin *vpp;

  /// multi-head support
  lives_mgeometry_t *mgeom;

  /// external control inputs
  boolean ext_cntl[MAX_EXT_CNTL];

#ifdef ALSA_MIDI
  snd_seq_t *seq_handle;
  int alsa_midi_port;
  int alsa_midi_dummy;
#endif

  boolean midi_channel_lock;

  weed_plant_t *rte_textparm; ///< send keyboard input to this paramter (usually NULL)

  int write_abuf; ///< audio buffer number to write to (for multitrack)
  volatile int abufs_to_fill;

  LiVESWidget *splash_window;
  LiVESWidget *splash_label;
  LiVESWidget *splash_progress;

#define SPLASH_LEVEL_BEGIN .0
#define SPLASH_LEVEL_START_GUI .2
#define SPLASH_LEVEL_LOAD_RTE .4
#define SPLASH_LEVEL_LOAD_APLAYER .6
#define SPLASH_LEVEL_LOAD_RFX .8
#define SPLASH_LEVEL_COMPLETE 1.

  boolean recoverable_layout;
  boolean invalid_clips;

  /// encoder text output
  LiVESIOChannel *iochan;
  LiVESTextView *optextview;

  boolean has_custom_effects;
  boolean has_custom_tools;
  boolean has_custom_gens;
  boolean has_custom_utilities;

  /// decoders
  boolean decoders_loaded;
  LiVESList *decoder_list;

  boolean go_away;
  boolean debug; ///< debug crashes and asserts

  char *subt_save_file; ///< name of file to save subtitles to

  char **fonts_array;
  int nfonts;

  LiVESTargetEntry *target_table; ///< drag and drop target table

  LiVESList *videodevs;

  char vpp_defs_file[PATH_MAX];

  int log_fd;

  boolean jack_trans_poll;

#define LIVES_MAX_ALARMS 1024
#define LIVES_MAX_USER_ALARMS 512

#define LIVES_URGENCY_ALARM LIVES_MAX_ALARMS // this is fine since we will subtract 1
#define URGENCY_MSG_TIMEOUT 10. // seconds

  lives_timeout_t alarms[LIVES_MAX_ALARMS]; ///< reserve 1 for emergency msgs
  int next_free_alarm;

  char *urgency_msg;

  // stuff specific to audio gens (will be extended to all rt audio fx)
  volatile int agen_key; ///< which fx key is generating audio [1 based] (or 0 for none)
  volatile boolean agen_needs_reinit;
  uint64_t agen_samps_count; ///< count of samples since init

  boolean aplayer_broken;

  /// TODO ***: these all need to be per thread
  boolean com_failed;
  int write_failed;
  int read_failed;
  boolean chdir_failed;

  boolean add_clear_ds_button;
  boolean add_clear_ds_adv;
  boolean tried_ds_recover;

  boolean has_session_workdir;
  boolean startup_error;

  boolean kb_timer_end;

  boolean draw_blocked; // block drawing of timeline bars : prevents an infinite loop

  int ce_frame_height;
  int ce_frame_width;

  /// TODO: need to be per-thread
  char *read_failed_file;
  char *write_failed_file;
  char *bad_aud_file;

  lives_render_error_t render_error;

  uint64_t next_ds_warn_level; ///< current disk space warning level for the tempdir

  float sepwin_scale;

  lives_pconnect_t *pconx; ///< list of out -> in param connections
  lives_cconnect_t *cconx; ///< list of out -> in alpha channel connections

  /// TOD: need to be per thread
  int rowstride_alignment;   // used to align the rowstride bytesize in create_empty_pixel_data
  int rowstride_alignment_hint;

  int sepwin_minwidth;
  int sepwin_minheight;

  uint32_t signal_caught;
  boolean signals_deferred;

  boolean ce_thumbs;
  boolean ce_upd_clip;

#define SCREEN_AREA_NONE -1
#define SCREEN_AREA_FOREGROUND 0
#define SCREEN_AREA_BACKGROUND 1
#define SCREEN_AREA_USER_DEFINED1 2

  int n_screen_areas; // number of screen areas
  int active_sa_fx; // active screen area for effects
  int active_sa_clips; // active screen area for clips
  lives_screen_area_t *screen_areas; // array of screen areas

  int active_track_list[MAX_TRACKS];
  boolean ext_src_used[MAX_FILES];
  lives_decoder_t *track_decoders[MAX_TRACKS];
  int old_active_track_list[MAX_TRACKS];

  boolean gen_started_play;
  boolean fx_is_auto;

  volatile lives_audio_buf_t *audio_frame_buffer; ///< used for buffering / feeding audio to video generators
  lives_audio_buf_t *afb[2]; ///< used for buffering / feeding audio to video generators
  int afbuffer_clients; /// # of registered clients for the audio frame buffer
  int afbuffer_clients_read; /// current read count. When this reaches abuffer_clients, we swap the read / write buffers

  pthread_t *libthread;  /// GUI thread for liblives
  ulong id; /// ???

  boolean interactive; /// if set to FALSE then interaction via the GUI (mouse / keypresses) should be disabled. Mainly for liblives.

  int fc_buttonresponse;

  char frameblank_path[PATH_MAX];
  char sepimg_path[PATH_MAX];

  uint64_t aud_data_written;

  int crash_possible; // TODO - check this

  LiVESPixbuf *scrap_pixbuf;

  volatile LiVESWidget *stop_emmission; /// handling for GUI exposure signals

  boolean no_context_update; ///< may be set temporarily to block wodget context updates

  weed_plant_t *msg_list;
  int n_messages;
  weed_plant_t *ref_message; // weak ref
  int ref_message_n;

  boolean recovering_files;

  ticks_t flush_audio_tc;

  // main window resizing, no longer very important
  int assumed_width;
  int assumed_height;
#define DEF_IDLE_MAX 1
  int idlemax;

  boolean reconfig; ///< set to TRUE if a monitor / screen size change is detected

  boolean ignore_screen_size;

  boolean recording_recovered;

  int def_trans_idx;

  // disk space in workdir
  lives_storage_status_t ds_status;
  uint64_t dsval;

  char *version_hash;
  char *old_vhash;

  /// experimental values, primarily for testing
  volatile uint32_t loadmeasure;
  volatile int uflow_count;

  boolean force_show; /// if set to TRUE during playback then a new frame (or possibly the current one) will be displayed ASAP

  boolean gui_fooey; ///< set to TRUE if we expect heavy interface updates (when not playing, please) so we can increase GUI iterations

  ///< a roughly calibrated value that ranges from -64 (lightly loaded) -> +64 (heavily loaded)
  /// (currently only active during playback), i.e higher values represent more machine load
  /// some functions may change their behaviour according to this value if prefs->pbq_adaptive is FALSE then such changes
  /// should be minimal; if TRUE then more profound changes are permitted
#define EFFORT_RANGE_MAX 64
#define EFFORT_LIMIT_LOW (EFFORT_RANGE_MAX >> 3)    ///< default 8
#define EFFORT_LIMIT_MED (EFFORT_RANGE_MAX >> 2)  ///< default 32

  boolean lockstats;

  int effort;

  boolean memok; ///< set to FALSE if a segfault is received

  /// this is not really used yet, but the idea is that in future the clipboard may be reproduced in various
  /// sizes / palettes / gamma functions, so instead of having to transform it each time we can cache various versions
#define MAX_CBSTORES 8
  int ncbstores;
  lives_clip_t *cbstores[8];
  ////////////////////
} mainwindow;

// weed plants with type >= 512 are reserved for custom use, so let's take advantage of that
#define WEED_PLANT_LIVES 31337

#define WEED_LEAF_LIVES_SUBTYPE "subtype"
#define WEED_LEAF_LIVES_MESSAGE_STRING "message_string"

#define LIVES_WEED_SUBTYPE_MESSAGE 1
#define LIVES_WEED_SUBTYPE_WIDGET 2

extern _palette *palette;

typedef struct {
  ulong ins_frame_function;

  LiVESWidget *merge_dialog;
  LiVESWidget *ins_frame_button;
  LiVESWidget *drop_frame_button;
  LiVESWidget *param_vbox;
  LiVESWidget *spinbutton_loops;

  boolean loop_to_fit;
  boolean align_start;
  boolean ins_frames;

  int *list_to_rfx_index;
  LiVESList *trans_list;
} _merge_opts;


typedef struct {
  LiVESWidget *dialog;
  LiVESWidget *cancelbutton;
  LiVESWidget *okbutton;
  LiVESWidget *resetbutton;
  lives_rfx_t *rfx;
  int key;
  int mode;
} _fx_dialog;


_merge_opts *merge_opts;

_fx_dialog *fx_dialog[2];

#ifndef IS_MINGW
#define LIVES_SIGKILL SIGKILL
#define LIVES_SIGINT  SIGINT
#define LIVES_SIGPIPE SIGPIPE
#define LIVES_SIGTRAP SIGTRAP
#define LIVES_SIGUSR1 SIGUSR1
#define LIVES_SIGABRT SIGABRT
#define LIVES_SIGSEGV SIGSEGV
#define LIVES_SIGHUP  SIGHUP
#define LIVES_SIGTERM SIGTERM
#define LIVES_SIGQUIT SIGQUIT
#else
#define LIVES_SIGKILL SIGTERM
#define LIVES_SIGINT  SIGINT
#define LIVES_SIGPIPE SIGPIPE
#define LIVES_SIGTRAP SIGTRAP
#define LIVES_SIGUSR1 SIGUSR1
#define LIVES_SIGABRT SIGABRT
#define LIVES_SIGSEGV SIGSEGV
#define LIVES_SIGHUP  SIGINT
#define LIVES_SIGTERM SIGTERM
#define LIVES_SIGQUIT SIGQUIT
#endif

#ifdef ENABLE_JACK
volatile aserver_message_t jack_message;
volatile aserver_message_t jack_message2;
#endif

#ifdef HAVE_PULSE_AUDIO
volatile aserver_message_t pulse_message;
volatile aserver_message_t pulse_message2;
#endif

#endif // HAS_LIVES_MAINWINDOW_H
