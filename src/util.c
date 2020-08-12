#include "util.h"

SEXP get_var_from_env(const char* var_name, SEXP env)
{
    SEXP var = PROTECT(Rf_findVar(Rf_install(var_name), env));
    if(var == R_UnboundValue)
        Rf_error("variable '%s' not found", var_name);
    if(TYPEOF(var) == PROMSXP)
    {
        var = Rf_eval(var, env);
        /* Unprotect the old var */
        UNPROTECT(1);
        /* Protect the new var */
        PROTECT(var);
    }
    UNPROTECT(1);

    return var;
}

SEXP get_pkg_env(const char* pkg_name)
{
    /* See grDevices/src/devPS getFontDB() */
    SEXP pkg_namespace, pkg_env;
    pkg_namespace = PROTECT(R_FindNamespace(Rf_mkString(pkg_name)));
    pkg_env = PROTECT(get_var_from_env(".pkg.env", pkg_namespace));
    UNPROTECT(2);

    return pkg_env;
}

SEXP get_var_from_pkg_env(const char* var_name, const char* pkg_name)
{
    SEXP pkg_env, var;

    pkg_env = PROTECT(get_pkg_env(pkg_name));
    var = PROTECT(get_var_from_env(var_name, pkg_env));
    UNPROTECT(2);

    return var;
}

FT_Outline_Funcs* get_ft_outline_funcs()
{
    FT_Outline_Funcs* funs;
    SEXP ext_ptr = PROTECT(get_var_from_pkg_env(".outline_funs", "showtext"));
    funs = (FT_Outline_Funcs*) R_ExternalPtrAddr(ext_ptr);
    UNPROTECT(1);

    return funs;
}

int get_num_segments()
{
    SEXP nseg = PROTECT(get_var_from_pkg_env(".nseg", "showtext"));
    int res = INTEGER(nseg)[0];
    UNPROTECT(1);

    return res;
}

void get_device_id(pGEDevDesc gdd, char* id)
{
    /* Theoretically we can just use the pointer address of gdd as the ID,
       but sometimes the address will be reused after the current device
       is closed and destroyed. To reduce the possibility of duplication,
       we also add addresses of some fields of gdd. */
    strcpy(id, "dev_");
    snprintf(id + 4, 16, "%p", (void*) gdd);
    id[20] = '_';
    snprintf(id + 21, 16, "%p", (void*) gdd->dev);
    id[37] = '_';
    snprintf(id + 38, 20, "%p", (void*) gdd->gesd);
}

SEXP get_device_data(pGEDevDesc gdd)
{
    SEXP devs_env, dev_data;
    char dev_id[60];
    get_device_id(gdd, dev_id);

    devs_env = PROTECT(get_var_from_pkg_env(".devs", "showtext"));
    dev_data = PROTECT(get_var_from_env(dev_id, devs_env));
    UNPROTECT(2);

    return dev_data;
}
