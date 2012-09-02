/* header file for utilities */

#ifndef UTILS_H
#define UTILS_H

/* debug prints */
#ifdef DEBUG
#include <stdio.h>
#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define _DEBUGPRINT_(...)       fprintf(stderr, __VA_ARGS__)
#define DEBUGPRINT(_fmt, ...)  _DEBUGPRINT_(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
#define DEBUGPRINT2(_fmt)       DEBUGPRINT(_fmt "%s", "")
#else /* do nothing :) */
#define WHERESTR     
#define WHEREARG     
#define _DEBUGPRINT_(...)       
#define DEBUGPRINT(_fmt, ...)        
#define DEBUGPRINT2(_fmt)       
#endif /* DEBUG */

/* automated check for NULL.
 * Prints a message if DEBUG is set, and returns returnval
 */
#include <stdlib.h>
#define CHECKNULL(variable, returnval) \
    if (variable == NULL){ \
        DEBUGPRINT2(#variable " is NULL\n"); \
        return returnval; \
    }

/*
 * 64bits detection
 */ 
#include <limits.h>

#if LONG_MAX == 2147483647
#define SIZEOFLONG 32
#elif LONG_MAX == 9223372036854775807
#define SIZEOFLONG 64
#endif

#endif /* UTILS_H */
