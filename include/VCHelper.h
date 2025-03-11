//Betty Vuong
//1271673
#ifndef _HELPER_PARSER
#define _HELPER_PARSER

#include <strings.h>
#include <ctype.h>

char * propertyWrite(void * prop);
char * dateTimeWrite(void * DT, bool bday);
bool propValidate(void * prop, char * id);
bool paramValidate(void * param);
bool dtValidate(void * prop);

#endif