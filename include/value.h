#ifndef _VALUE_H_
#define _VALUE_H_

#include "configType.h"
#include "list.h"

union valUnion {
    const char* string;
    long integer;
    float decimal;
    list array;
};

typedef struct _value {
    config_type type;
    union valUnion value;
} value;

#endif

