#include "util.h"

void showtextMetricInfo(int c, const pGEcontext gc, double* ascent, double* descent, double* width, pDevDesc dd);
double showtextStrWidthUTF8(const char *str, const pGEcontext gc, pDevDesc dd);
void showtextTextUTF8(double x, double y, const char *str, double rot, double hadj, const pGEcontext gc, pDevDesc dd);

SEXP showtextNullPointer()
{
    SEXP extPtr;

    extPtr = PROTECT(R_MakeExternalPtr(NULL, R_NilValue, R_NilValue));
    UNPROTECT(1);
    return extPtr;
}

SEXP showtextNewDevDesc()
{
    pDevDesc dd_save = (pDevDesc) calloc(1, sizeof(DevDesc));
    SEXP extPtr;

    extPtr = PROTECT(R_MakeExternalPtr(dd_save, R_NilValue, R_NilValue));
    UNPROTECT(1);
    return extPtr;
}

SEXP showtextFreeDevDesc(SEXP extPtr)
{
    pDevDesc dd_saved = (pDevDesc) R_ExternalPtrAddr(extPtr);
    if(dd_saved) free(dd_saved);
    
    return R_NilValue;
}

SEXP showtextBegin()
{
    /* currDev is an integer assigned to the current graphics device.
       If currDev == 1, then there is no active device, since the null
       device is always assigned the number 0. */
    int currDev = curDevice();
    SEXP extPtr;
    /* gdd serves as the identifier of a graphics device. We need an id
       for each device since showtext.begin() and showtext.end() must
       work on the same graphics device.
    
       When calling showtext.begin(), we save the gdd of the current device
       to the package database(showtext:::.pkg.env), and in the call of
       showtext.end(), we compare it with the active device at that time. */
    pGEDevDesc gdd;
    /* The device structure that we want to modify */
    pDevDesc dd;
    
    if(currDev == 0)
        Rf_error("no active graphics device");

    /* Save the current gdd to showtext:::.pkg.env */
    gdd = GEgetDevice(currDev);
    extPtr = PROTECT(R_MakeExternalPtr(gdd, R_NilValue, R_NilValue));
    Rf_setVar(install(".device_id"), extPtr, GetPkgEnv("showtext"));
    UNPROTECT(1);
    
    /* Save the current dd */
    dd = gdd->dev;
    *(GetSavedDevDesc()) = *dd;
    
    /* Replace the text functions */
    dd->canHAdj = TRUE;
    dd->metricInfo = showtextMetricInfo;
    dd->hasTextUTF8 = TRUE;
    dd->text = showtextTextUTF8;
    dd->textUTF8 = showtextTextUTF8;
    dd->strWidth = showtextStrWidthUTF8;
    dd->strWidthUTF8 = showtextStrWidthUTF8;
    dd->wantSymbolUTF8 = TRUE;

    return R_NilValue;
}

SEXP showtextEnd()
{
    int currDev = curDevice();
    pGEDevDesc gdd;
    pDevDesc dd;
    
    if(currDev == 0)
        Rf_error("no active graphics device");
        
    gdd = GEgetDevice(currDev);
    if(gdd != GetSavedDeviceID())
    {
        Rf_error("current device does not match the one that uses showtext.begin()");
    }
    
    /* Restore dd */
    dd = gdd->dev;
    *dd = *(GetSavedDevDesc());

    return R_NilValue;
}

