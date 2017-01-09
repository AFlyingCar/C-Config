#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "value.h"

typedef struct _variable_struct {
    const char* name;
    value val;
} variable;

#endif

