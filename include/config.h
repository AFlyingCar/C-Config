#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "value.h"
#include "configType.h"
#include "list.h"
#include "variable.h"

typedef struct _config_struct config;

const char* getFileName(config*);
value* getValue(config*, const char*);
variable* getAllVariables(config*);

config* parseFile(const char* filename);

void freeConfig(config*);

#endif

