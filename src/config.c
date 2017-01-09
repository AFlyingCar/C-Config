#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "configType.h"
#include "list.h"
#include "value.h"
#include "variable.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define MAX_CONFIG_LINE 256 /* TODO: Change this value. */
#define MAX_VAR_LENGTH 128
#define STARTING_VAR_NAME_MEM 128 /* The starting number of variables to
                                     allocate. */

struct _config_struct {
    const char* fileName;

    variable* array;

    int size;
    int numVars;
};

const char* getFileName(struct _config_struct* cfg) {
    return cfg->fileName;
}

value* getValue(struct _config_struct* cfg, const char* var) {
    int i;

    for(i = 0; i < cfg->numVars; i++) {
        if(strcmp(cfg->array[i].name, var) == 0) {
            return &cfg->array[i].val;
        }
    }

    return NULL;
}

variable* getAllVariables(struct _config_struct* cfg) {
    return cfg->array;
}

config_type determineType(const char* raw) {
    if(raw[0] == '"') {
        return STRING;
    } else if(raw[0] == '[') {
        return LIST;
    } else if(strchr(raw, '.') != NULL) {
        return FLOAT;
    } else if(strchr(raw, '0') != NULL ||
              strchr(raw, '1') != NULL ||
              strchr(raw, '2') != NULL ||
              strchr(raw, '3') != NULL ||
              strchr(raw, '4') != NULL ||
              strchr(raw, '5') != NULL ||
              strchr(raw, '6') != NULL ||
              strchr(raw, '7') != NULL ||
              strchr(raw, '8') != NULL ||
              strchr(raw, '9') != NULL)
    {
        return INTEGER;
    } else {
        return STRING;
    }
}

int chrToInt(char c) {
    assert(c >= '0' && c <= '9');
    return c - '0';
}

int rawToInt(const char* raw) {
    int sign = 1;
    int total = 0;
    const char* c;

    if(raw[0] == '-') {
        sign = -1;
    } else if(raw[0] == '+') {}

    for(c = raw; *c != '\0'; c++) {
        total *= 10;
        total += chrToInt(*c);
    }

    return sign * total;
}

float rawToFloat(const char* raw) {
    char* point = strchr(raw, '.');

    if(point == NULL) {
        return (float)rawToInt(raw);
    } else {
        char* tmp = malloc(strlen(raw));
        int front;
        int back;

        strcpy(tmp, raw);

        tmp[(int)(point - raw)] = '\0';
        front = rawToInt(tmp);
        back = rawToInt(point + 1);
        free(tmp);
        return front + (((float)back) / strlen(point + 1));
    }
}

/* NOTE: SHUT UP YOU STUPID COMPILER. */
static value* parseValue(const char*);

list rawToList(const char* raw) {
    const char* nextComma;
    const char* currVal = raw + 1;
    list ret;
    int numElems = 0;
    const char* c;
    int next = 0;

    for(c = raw + 1; *c != ']'; c++) {
        /* If the second char isn't immediately a ], then there is at least one
         *  element.
         */
        if(numElems == 0) {
            numElems++;
        }

        /* Count each comma. */
        if(*c == ',') {
            numElems++;
        }
    }

    ret.array = (value**)malloc(sizeof(value*) * numElems);

    while((nextComma = strchr(raw, ',')) != NULL) {
        value* nextValue;
        char* temp = (char*)malloc(nextComma - currVal);

        /* NOTE: May need to add a "- 1" to the number of chars to copy. */
        strncpy(temp, currVal, nextComma - currVal);

        nextValue = parseValue(temp);
        free(temp);

        ret.array[next] = nextValue;

        currVal = nextComma + 1;
    }

    return ret;
}

const char* rawToString(const char* raw) {
    char* newStr = (char*)malloc(strlen(raw) - 2);
    return (const char*)strncpy(newStr, raw + 1, strlen(raw) - 2);
}

/*
 * Parses a given string to be some value. Returns a pointer to the value struct
 *  upon success, or NULL upon failure.
 */
static value* parseValue(const char* rawValue) {
    value* ret = NULL;

    if(rawValue == NULL) {
        return NULL;
    }

    ret = (value*)malloc(sizeof(value));

    if(ret == NULL) {
        return NULL;
    }

    ret->type = determineType(rawValue);

    switch(ret->type) {
        case INTEGER:
            ret->value.integer = rawToInt(rawValue);
            break;
        case FLOAT:
            ret->value.decimal = rawToFloat(rawValue);
            break;
        case LIST:
            ret->value.array = rawToList(rawValue);
            break;
        case STRING:
            ret->value.string = rawToString(rawValue);
            break;
        default:
            ret->value.string = rawValue;
            break;
    }

    return ret;
}

/*
 * Expands the amount of memory available to a config struct. Returns 0 upon
 *  success and non-zero upon failure.
 */
static int expandCfgMemory(struct _config_struct* cfg) {
    int i;
    int newSize = cfg->size * 2;
    variable* newArray = (variable*)malloc(sizeof(variable) * newSize);

    for(i = 0; i < cfg->numVars; i++) {
        newArray[i] = cfg->array[i];
    }

    return 0;
}

struct _config_struct* parseFile(const char* filename) {
    struct _config_struct* cfg;
    char line[MAX_CONFIG_LINE];
    FILE* file;

    cfg = malloc(sizeof(struct _config_struct));

    if(cfg == NULL) {
        return NULL;
    }

    if((file = fopen(filename, "rt")) == NULL) {
        free(cfg);
        return NULL;
    }

    cfg->array = (variable*)malloc(sizeof(variable*) *
                                          STARTING_VAR_NAME_MEM);
    if(cfg->array == NULL) {
        free(cfg);
        fclose(file);
        return NULL;
    }

    cfg->numVars = 0;
    cfg->size = STARTING_VAR_NAME_MEM;

    while(fgets(line, MAX_CONFIG_LINE, file) != NULL) {
        char* c;
        char varName[MAX_VAR_LENGTH] = { 0 };
        char rawValue[MAX_VAR_LENGTH] = { 0 }; /*
                                                * Every value can be this many
                                                * chars as well.
                                                */
        int checkingVal = 0; /* are we currently checking the value or var? */
        int i;
        value* value;

        /* If the line is a comment or completely empty, then skip it. */
        if(line[0] == '@' || line[0] == '\n') {
            continue;
        }

        for(c = line; *c != '\0'; c++) {
            if(*c == ' ' || *c == '\t') {
                continue;
            } else if(*c == '=') {
                checkingVal = 1;
            } else {
                int i = 0;
                /* Keep reading characters */
                while(*c != ' ' && *c != '\t') {
                    if(checkingVal) {
                        if(*c == '\n') {
                            break;
                        }
                        rawValue[i++] = *c;
                    } else {
                        /*
                         * Did we reach the end of the line before we found a
                         *  value?
                         */
                        if(*c == '\n') {
                            fclose(file);
                            free(cfg);
                            return NULL;
                        }

                        varName[i++] = *c;
                    }

                    c++; /* Make sure to keep incrementing c. */
                }

                /*c++; *//* Increment c by 1 so that we skip the whitespace */
            }
        }

        value = parseValue(rawValue);

        /*
         * Lets now try to figure out where to place the new variable and value.
         */
        for(i = 0; i < cfg->numVars; i++) {
            if(strcmp(cfg->array[i].name, varName) == 0) {
                cfg->array[i].val = *value; /* Only store a copy of the value, not
                                               a pointer. This makes it easier to
                                               free up memory later.
                                              */
                cfg->numVars++;
            }
        }


        if(i == cfg->numVars) {
            int oldNum = cfg->numVars;

            if(cfg->numVars == cfg->size) {
                /* Make sure we have enough memory still. */
                if(expandCfgMemory(cfg)) {
                    fclose(file);
                    freeConfig(cfg);
                    return NULL;
                }
            }

            cfg->array[oldNum].name = varName;
            cfg->array[oldNum].val = *value;
            cfg->numVars++;
        }

        free(value); /* We need to make sure we free the memory */
    }

    return cfg;
}

void freeConfig(struct _config_struct* cfg) {
    if(cfg == NULL || cfg->array == NULL) {
        return;
    }

    free(cfg->array);
    free(cfg);
}

