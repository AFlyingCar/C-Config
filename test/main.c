#include "config.h"
#include "stdlib.h"
#include "stdio.h"

int main(void) {
    config* myConfig = parseFile("basic.cfg");
    config* configInt;
    config* configFloat;
    config* configString;
    config* configList;

    printf("1\n");

    if(myConfig == NULL) {
        printf("parseFile returned NULL\n");
        return 1;
    }

    freeConfig(myConfig);

    /*******************************/

    printf("2\n");
    configInt = parseFile("int.cfg");
    if(configInt == NULL) {
        printf("parseFile returned NULL\n");
        return 1;
    }

    printf("num1=%d\n", getValue(configInt, "num1")->value.integer);
    printf("num2=%d\n", getValue(configInt, "num2")->value.integer);
    printf("num3=%d\n", getValue(configInt, "num3")->value.integer);

    freeConfig(configInt);

    return 0;
}

