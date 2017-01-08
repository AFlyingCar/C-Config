#ifndef _LIST_H_
#define _LIST_H_

#include "configType.h"
#include "value.h"

struct _list;

union _ptr_union {
    int* iptr;
    const char** sptr;
    struct _list* lptr;
    float* fptr;
};

typedef struct _list {
    config_type      type;
    struct _value**           array;
    int              size;
} list;


#endif

