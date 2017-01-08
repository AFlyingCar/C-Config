#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "value.h"

typedef struct _config_struct config;

const char* getFileName(config*);
value* getValue(config*, const char*);
const char** getAllVariables(config*);
value* getAllValues(config*);

config* parseFile(const char* filename);

#endif

