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

/* (C) Gabriel "Salsaman" Finch, 2005 - 2019 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define __LIBWEED__

#ifdef HAVE_SYSTEM_WEED
#include <weed/weed.h>
#include <weed/weed-effects.h>
#include <weed/weed-utils.h>
#else
#include "weed.h"
#include "weed-effects.h"
#include "weed-utils.h"
#endif

static void *host_bootstrap_callback = NULL;
static int host_weed_api_version = WEED_API_VERSION;

/////////////////////////////////////////////////////////////////

int weed_plant_has_leaf(weed_plant_t *plant, const char *key) {
  if (weed_leaf_get(plant, key, 0, NULL) == WEED_ERROR_NOSUCH_LEAF) return WEED_FALSE;
  return WEED_TRUE;
}


/////////////////////////////////////////////////////////////////
// leaf setters

int weed_set_int_value(weed_plant_t *plant, const char *key, int value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_INT, 1, &value);
}


int weed_set_double_value(weed_plant_t *plant, const char *key, double value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_DOUBLE, 1, &value);
}


int weed_set_boolean_value(weed_plant_t *plant, const char *key, int value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_BOOLEAN, 1, &value);
}


int weed_set_int64_value(weed_plant_t *plant, const char *key, int64_t value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_INT64, 1, &value);
}


int weed_set_string_value(weed_plant_t *plant, const char *key, const char *value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_STRING, 1, &value);
}


int weed_set_plantptr_value(weed_plant_t *plant, const char *key, weed_plant_t *value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_PLANTPTR, 1, &value);
}


int weed_set_voidptr_value(weed_plant_t *plant, const char *key, void *value) {
  // returns a WEED_ERROR
  return weed_leaf_set(plant, key, WEED_SEED_VOIDPTR, 1, &value);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// general leaf getter

static inline int weed_get_value(weed_plant_t *plant, const char *key, void *value) {
  // returns a WEED_ERROR
  return weed_leaf_get(plant, key, 0, value);
}


////////////////////////////////////////////////////////////

int weed_get_int_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int retval = 0;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_INT) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return retval;
  } else *error = weed_get_value(plant, key, &retval);
  return retval;
}


double weed_get_double_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  double retval = 0.;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_DOUBLE) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return retval;
  }
  *error = weed_get_value(plant, key, &retval);
  return retval;
}


int weed_get_boolean_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int retval = 0;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_BOOLEAN) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return retval;
  }
  *error = weed_get_value(plant, key, &retval);
  return retval;
}


int64_t weed_get_int64_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int64_t retval = 0;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_INT64) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return retval;
  }
  *error = weed_get_value(plant, key, &retval);
  return retval;
}


char *weed_get_string_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  size_t size;
  char *retval = NULL;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_STRING) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }
  if ((retval = (char *)malloc((size = weed_leaf_element_size(plant, key, 0)) + 1)) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }
  if ((*error = weed_get_value(plant, key, &retval)) != WEED_NO_ERROR) {
    free(retval);
    return NULL;
  }
  memset(retval + size, 0, 1);
  return retval;
}


void *weed_get_voidptr_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  void *retval = NULL;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_VOIDPTR) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return retval;
  }
  *error = weed_get_value(plant, key, &retval);
  return retval;
}


weed_plant_t *weed_get_plantptr_value(weed_plant_t *plant, const char *key, weed_error_t *error) {
  weed_plant_t *retval = NULL;
  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_PLANTPTR) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return retval;
  }
  *error = weed_get_value(plant, key, &retval);
  return retval;
}


////////////////////////////////////////////////////////////

int *weed_get_int_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  int *retval;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_INT) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }

  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (int *)malloc(num_elems * 4)) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      free(retval);
      return NULL;
    }
  }
  return retval;
}


double *weed_get_double_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  double *retval;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_DOUBLE) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }
  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (double *)malloc(num_elems * sizeof(double))) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      free(retval);
      return NULL;
    }
  }
  return retval;
}


int *weed_get_boolean_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  int *retval;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_BOOLEAN) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }

  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (int *)malloc(num_elems * 4)) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      free(retval);
      return NULL;
    }
  }
  return retval;
}


int64_t *weed_get_int64_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  int64_t *retval;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_INT64) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }

  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (int64_t *)malloc(num_elems * 8)) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      free(retval);
      return NULL;
    }
  }
  return retval;
}


char **weed_get_string_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  char **retval;
  size_t size;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_STRING) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }

  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (char **)malloc(num_elems * sizeof(char *))) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((retval[i] = (char *)malloc((size = weed_leaf_element_size(plant, key, i)) + 1)) == NULL) {
      for (--i; i >= 0; i--) free(retval[i]);
      *error = WEED_ERROR_MEMORY_ALLOCATION;
      free(retval);
      return NULL;
    }
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      for (--i; i >= 0; i--) free(retval[i]);
      free(retval);
      return NULL;
    }
    memset(retval[i] + size, 0, 1);
  }
  return retval;
}


void **weed_get_voidptr_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  void **retval;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_VOIDPTR) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }

  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (void **)malloc(num_elems * sizeof(void *))) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      free(retval);
      return NULL;
    }
  }
  return retval;
}


weed_plant_t **weed_get_plantptr_array(weed_plant_t *plant, const char *key, weed_error_t *error) {
  int i;
  int num_elems;
  weed_plant_t **retval;

  if (weed_plant_has_leaf(plant, key) && weed_leaf_seed_type(plant, key) != WEED_SEED_PLANTPTR) {
    *error = WEED_ERROR_WRONG_SEED_TYPE;
    return NULL;
  }

  if ((num_elems = weed_leaf_num_elements(plant, key)) == 0) return NULL;

  if ((retval = (weed_plant_t **)malloc(num_elems * sizeof(weed_plant_t *))) == NULL) {
    *error = WEED_ERROR_MEMORY_ALLOCATION;
    return NULL;
  }

  for (i = 0; i < num_elems; i++) {
    if ((*error = weed_leaf_get(plant, key, i, &retval[i])) != WEED_NO_ERROR) {
      free(retval);
      return NULL;
    }
  }
  return retval;
}


/////////////////////////////////////////////////////

/* int weed_set_int_array(weed_plant_t *plant, const char *key, int num_elems, int *values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_INT, num_elems, values); */
/* } */


/* int weed_set_double_array(weed_plant_t *plant, const char *key, int num_elems, double *values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_DOUBLE, num_elems, values); */
/* } */


/* int weed_set_boolean_array(weed_plant_t *plant, const char *key, int num_elems, int *values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_BOOLEAN, num_elems, values); */
/* } */


/* int weed_set_int64_array(weed_plant_t *plant, const char *key, int num_elems, int64_t *values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_INT64, num_elems, values); */
/* } */


/* int weed_set_string_array(weed_plant_t *plant, const char *key, int num_elems, char **values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_STRING, num_elems, values); */
/* } */


/* int weed_set_voidptr_array(weed_plant_t *plant, const char *key, int num_elems, void **values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_VOIDPTR, num_elems, values); */
/* } */


/* int weed_set_plantptr_array(weed_plant_t *plant, const char *key, int num_elems, weed_plant_t **values) { */
/*   return weed_leaf_set(plant, key, WEED_SEED_PLANTPTR, num_elems, values); */
/* } */




weed_error_t weed_set_int_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, int32_t *values) {
  return weed_leaf_set(plant, key, WEED_SEED_INT, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_double_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, double *values) {
  return weed_leaf_set(plant, key, WEED_SEED_DOUBLE, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_boolean_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, int32_t *values) {
  return weed_leaf_set(plant, key, WEED_SEED_BOOLEAN, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_int64_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, int64_t *values) {
  return weed_leaf_set(plant, key, WEED_SEED_INT64, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_string_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, char **values) {
  return weed_leaf_set(plant, key, WEED_SEED_STRING, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_voidptr_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, weed_voidptr_t *values) {
  return weed_leaf_set(plant, key, WEED_SEED_VOIDPTR, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_plantptr_array(weed_plant_t *plant, const char *key, weed_size_t num_elems, weed_plant_t **values) {
  return weed_leaf_set(plant, key, WEED_SEED_PLANTPTR, num_elems, (weed_voidptr_t)values);
}


weed_error_t weed_set_custom_array(weed_plant_t *plant, const char *key, int32_t seed_type, weed_size_t num_elems, weed_voidptr_t *values) {
  return weed_leaf_set(plant, key, seed_type, num_elems, (weed_voidptr_t)values);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t weed_get_plant_type(weed_plant_t *plant) {
  weed_error_t error;
  return weed_get_int_value(plant, WEED_LEAF_TYPE, &error);
}


weed_error_t weed_leaf_copy(weed_plant_t *dst, const char *keyt, weed_plant_t *src, const char *keyf) {
  weed_size_t num;
  weed_error_t err;
  int32_t seed_type;
  int i;

  if (dst == NULL || src == NULL) return WEED_ERROR_NOSUCH_PLANT;

  if ((err = weed_check_leaf(src, keyf)) == WEED_ERROR_NOSUCH_LEAF) return WEED_ERROR_NOSUCH_LEAF;

  seed_type = weed_leaf_seed_type(src, keyf);

  if (err == WEED_ERROR_NOSUCH_ELEMENT) weed_leaf_set(dst, keyt, seed_type, 0, NULL);
  else {
    num = weed_leaf_num_elements(src, keyf);
    switch (seed_type) {
    case WEED_SEED_INT: {
      int32_t *datai = weed_get_int_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_int_array(dst, keyt, num, datai);
      if (datai != NULL) free(datai);
    }
      break;
    case WEED_SEED_INT64: {
      int64_t *datai64 = weed_get_int64_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_int64_array(dst, keyt, num, datai64);
      if (datai64 != NULL) free(datai64);
    }
      break;
    case WEED_SEED_BOOLEAN: {
      int32_t *datai = weed_get_boolean_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_boolean_array(dst, keyt, num, datai);
      if (datai != NULL) free(datai);
    }
      break;
    case WEED_SEED_DOUBLE: {
      double *datad = weed_get_double_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_double_array(dst, keyt, num, datad);
      if (datad != NULL) free(datad);
    }
      break;
    case WEED_SEED_VOIDPTR: {
      weed_voidptr_t *datav = weed_get_voidptr_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_voidptr_array(dst, keyt, num, datav);
      if (datav != NULL) free(datav);
    }
      break;
    case WEED_SEED_PLANTPTR: {
      weed_plant_t **datap = weed_get_plantptr_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_plantptr_array(dst, keyt, num, datap);
	if (datap != NULL) free(datap);
      }
      break;
    case WEED_SEED_STRING: {
      char **datac = weed_get_string_array(src, keyf, &err);
      if (err == WEED_SUCCESS)
	err = weed_set_string_array(dst, keyt, num, datac);
      for (i = 0; i < num; i++) {
	free(datac[i]);
      }
      free(datac);
    }
      break;
    }
  }
  return err;
}


weed_plant_t *weed_plant_copy(weed_plant_t *src) {
  int i = 0;
  weed_error_t err;
  char **proplist;
  char *prop;
  weed_plant_t *plant;

  if (src == NULL) return NULL;

  plant = weed_plant_new(weed_get_int_value(src, WEED_LEAF_TYPE, &err));
  if (plant == NULL) return NULL;

  proplist = weed_plant_list_leaves(src);
  for (prop = proplist[0]; (prop = proplist[i]) != NULL && err == WEED_SUCCESS; i++) {
    if (err == WEED_SUCCESS) {
      if (strcmp(prop, WEED_LEAF_TYPE)) {
	err = weed_leaf_copy(plant, prop, src, prop);
      }
    }
    free(prop);
  }
  free(proplist);

  if (err == WEED_ERROR_MEMORY_ALLOCATION) {
    //if (plant!=NULL) weed_plant_free(plant); // well, we should free the plant, but the plugins don't have this function...
    return NULL;
  }

  return plant;
}

/// broken


/* int32_t weed_get_plant_type(weed_plant_t *plant) { */
/*   if (plant == NULL) return WEED_PLANT_UNKNOWN; */
/*   return weed_get_int_value(plant, WEED_LEAF_TYPE, NULL); */
/* } */


void weed_add_plant_flags(weed_plant_t *plant, int32_t flags) {
  // TODO
}


void weed_clear_plant_flags(weed_plant_t *plant, int32_t flags) {
  char **leaves = weed_plant_list_leaves(plant);
  int i;

  for (i = 0; leaves[i] != NULL; i++) {
    weed_leaf_set_flags(plant, leaves[i], (weed_leaf_get_flags(plant, leaves[i]) | flags) ^ flags);
    free(leaves[i]);
  }
  if (leaves != NULL) free(leaves);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


#define weed_seed_is_ptr(seed_type) (seed_type >= 64 ? 1 : 0)

static inline uint64_t weed_seed_get_size(int32_t seed, void *value) {
  return weed_seed_is_ptr(seed) ? (sizeof(void *)) : \
         (seed == WEED_SEED_BOOLEAN || seed == WEED_SEED_INT) ? 4 : \
         (seed == WEED_SEED_DOUBLE) ? 8 : \
         (seed == WEED_SEED_INT64) ? 8 : \
         (seed == WEED_SEED_STRING) ? strlen((const char *)value) : 0;
}


static inline weed_leaf_t *weed_find_leaf(weed_plant_t *leaf, const char *key) {
  while (leaf != NULL) {
    if (!strcmp((char *)leaf->key, (char *)key)) return leaf;
    leaf = leaf->next;
  }
  return NULL;
}


static weed_error_t _weed_default_get(weed_plant_t *plant, const char *key, weed_function_f *value) {
  // we pass a pointer to this function back to the plugin so that it can bootstrap its real functions

  // here we must assume that the plugin does not yet have its (static) memory functions, so we can only
  // use the standard ones

  weed_leaf_t *leaf = weed_find_leaf(plant, key);
  if (leaf == NULL) return WEED_ERROR_NOSUCH_LEAF;
  if (value == NULL) return WEED_NO_ERROR; // value can be NULL to check if leaf exists

  if (weed_seed_is_ptr(leaf->seed_type)) memcpy(value, (weed_voidptr_t)&leaf->data[0]->value, WEED_VOIDPTR_SIZE);
  else {
    if (leaf->seed_type == WEED_SEED_STRING) {
      weed_size_t size = ((weed_data_t *)leaf->data)->size;
      char **valuecharptrptr = (char **)value;
      if (size > 0) memcpy(*valuecharptrptr, ((weed_data_t *)leaf->data)->value, size);
      memset(*valuecharptrptr + size, 0, 1);
    } else memcpy(value, ((weed_data_t *)leaf->data)->value, weed_seed_get_size(leaf->seed_type, ((weed_data_t *)leaf->data)->value));
  }
  return WEED_NO_ERROR;
}


static inline int check_weed_api_compat(int32_t hostv, int32_t filterv) {
  // hostv always > filterv
  if (filterv < 200) return 0; // API 200 made breaking changes
  return 1;
}


static inline int check_filter_api_compat(int32_t hostv, int32_t filterv) {
  return 1;
}


static int check_version_compat(int host_weed_api_version,
			 int plugin_weed_api_min_version,
			 int plugin_weed_api_max_version,
			 int host_filter_api_version,
			 int plugin_filter_api_min_version,
			 int plugin_filter_api_max_version) {

  fprintf(stderr, "VALOO %d %d %d %d %d %d\n", host_weed_api_version,
			 plugin_weed_api_min_version,
			 plugin_weed_api_max_version,
			 host_filter_api_version,
			 plugin_filter_api_min_version,
	  plugin_filter_api_max_version);

  
  if (plugin_weed_api_min_version > host_weed_api_version || plugin_filter_api_min_version > host_filter_api_version)
    return 0;

  if (host_weed_api_version > plugin_weed_api_max_version) {
    if (check_weed_api_compat(host_weed_api_version, plugin_weed_api_max_version) == 0) return 0;
  }

  if (host_filter_api_version > plugin_filter_api_max_version) {
    return check_filter_api_compat(host_filter_api_version, plugin_filter_api_max_version);
  }
  return 1;
}
							     

weed_plant_t *weed_bootstrap_func(weed_default_getter_f *value,
				  int32_t plugin_min_weed_api_version,
				  int32_t plugin_max_weed_api_version,
				  int32_t plugin_min_filter_api_version,
				  int32_t plugin_max_filter_api_version) {
  // function is called from weed_setup() in the plugin, using the fn ptr passed by the host

  // here is where we define the functions for the plugin to use
  // the host is free to implement its own version and then pass a pointer to that function in weed_setup() for the plugin

  void *host_bootstrap_callback = NULL; // TODO
  
  static weed_leaf_get_f wlg;
  static weed_plant_new_f wpn;
  static weed_plant_list_leaves_f wpll;
  static weed_leaf_num_elements_f wlne;
  static weed_leaf_element_size_f wles;
  static weed_leaf_seed_type_f wlst;
  static weed_leaf_get_flags_f wlgf;
  static weed_leaf_set_f wls;
  static weed_malloc_f weedmalloc;
  static weed_realloc_f weedrealloc;
  static weed_free_f weedfree;
  static weed_memcpy_f weedmemcpy;
  static weed_memset_f weedmemset;
  static weed_plant_free_f wpf;

  int32_t host_weed_api_version = WEED_API_VERSION;
  int32_t host_filter_api_version = WEED_FILTER_API_VERSION;

  weed_plant_t *host_info = weed_plant_new(WEED_PLANT_HOST_INFO);

  // set pointers to the functions the plugin will use

  wpn = weed_plant_new;
  wpll = weed_plant_list_leaves;
  wlne = weed_leaf_num_elements;
  wles = weed_leaf_element_size;
  wlst = weed_leaf_seed_type;
  wlgf = weed_leaf_get_flags;
  wls = weed_leaf_set;
  wlg = weed_leaf_get;
  wpf = weed_plant_free;
  
  weedmalloc = malloc;
  weedrealloc = realloc;
  weedfree = free;
  weedmemcpy = memcpy;
  weedmemset = memset;

  *value = _weed_default_get; // value is a pointer to fn. ptr

  weed_set_voidptr_value(host_info, WEED_LEAF_MALLOC_FUNC, weedmalloc);
  weed_set_voidptr_value(host_info, WEED_LEAF_FREE_FUNC, weedfree);
  weed_set_voidptr_value(host_info, WEED_LEAF_MEMSET_FUNC, weedmemset);
  weed_set_voidptr_value(host_info, WEED_LEAF_MEMCPY_FUNC, weedmemcpy);
  weed_set_voidptr_value(host_info, WEED_LEAF_REALLOC_FUNC, weedrealloc);

  weed_set_voidptr_value(host_info, WEED_LEAF_GET_FUNC, wlg);

  weed_set_voidptr_value(host_info, WEED_PLANT_NEW_FUNC, wpn);
  weed_set_voidptr_value(host_info, WEED_LEAF_SET_FUNC, wls);
  weed_set_voidptr_value(host_info, WEED_LEAF_SEED_TYPE_FUNC, wlst);
  weed_set_voidptr_value(host_info, WEED_LEAF_NUM_ELEMENTS_FUNC, wlne);
  weed_set_voidptr_value(host_info, WEED_LEAF_ELEMENT_SIZE_FUNC, wles);
  weed_set_voidptr_value(host_info, WEED_PLANT_LIST_LEAVES_FUNC, wpll);
  weed_set_voidptr_value(host_info, WEED_LEAF_GET_FLAGS_FUNC, wlgf);
  weed_set_voidptr_value(host_info, WEED_PLANT_FREE_FUNC, wpf);

  weed_set_int_value(host_info, WEED_LEAF_WEED_API_VERSION, host_weed_api_version);
  weed_set_int_value(host_info, WEED_LEAF_FILTER_API_VERSION, host_filter_api_version);

  weed_set_int_value(host_info, WEED_LEAF_MIN_WEED_API_VERSION, plugin_min_weed_api_version);
  weed_set_int_value(host_info, WEED_LEAF_MAX_WEED_API_VERSION, plugin_max_weed_api_version);
  weed_set_int_value(host_info, WEED_LEAF_MIN_FILTER_API_VERSION, plugin_min_filter_api_version);
  weed_set_int_value(host_info, WEED_LEAF_MAX_FILTER_API_VERSION, plugin_max_filter_api_version);
  
  if (host_bootstrap_callback != NULL) {
    // TODO
     //host_bootstrap_callback(host_info);
    if (weed_plant_has_leaf(host_info, WEED_LEAF_WEED_API_VERSION))
      host_weed_api_version = weed_get_int_value(host_info, WEED_LEAF_WEED_API_VERSION, NULL);
    if (weed_plant_has_leaf(host_info, WEED_LEAF_FILTER_API_VERSION))
      host_filter_api_version = weed_get_int_value(host_info, WEED_LEAF_FILTER_API_VERSION, NULL);
  }

  if (!check_version_compat(host_weed_api_version, plugin_min_weed_api_version, plugin_max_weed_api_version, host_filter_api_version,
			    plugin_min_filter_api_version, plugin_max_filter_api_version)) {
    weed_plant_free(host_info);
    return NULL;
  }

  if (host_weed_api_version < 200) {
    weed_leaf_delete(host_info, WEED_LEAF_REALLOC_FUNC);
    weedrealloc = NULL;
  }
  
  // TODO
  //weed_add_plant_flags(host_info, WEED_LEAF_READONLY_PLUGIN);

  return host_info;
}

