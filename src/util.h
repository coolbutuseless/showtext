#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

/* Freetype 2.6 introduces a macro TYPEOF that conflicts with R */
#ifdef TYPEOF
#undef TYPEOF
#endif

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#ifndef R_USE_PROTOTYPES
  #define R_USE_PROTOTYPES 1
#endif
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>


enum {
    DEV_DATA_USE_RASTER = 0,
    DEV_DATA_DEV_UNITS_PER_POINT = 1,
    DEV_DATA_DD_SAVED = 2
};

/* Get a variable by name from an environment */
SEXP get_var_from_env(const char* var_name, SEXP env);

/* Get the .pkg.env variable (an environment) defined in one package */
/* The sysfonts and showtext packages use this way to store variables */
SEXP get_pkg_env(const char* pkg_name);

/* Get a variable from .pkg.env */
SEXP get_var_from_pkg_env(const char* var_name, const char* pkg_name);

/* Get .pkg.env$.outline_funs */
FT_Outline_Funcs* get_ft_outline_funcs();

/* Get .pkg.env$.nseg */
int get_num_segments();

/* Get device data from .pkg.env$.devs */
SEXP get_device_data(pGEDevDesc gdd);


#endif /* UTIL_H_INCLUDED */
