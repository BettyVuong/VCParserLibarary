// Betty Vuong
// Student ID: 1271673
#include "LinkedListAPI.h"
#include "VCParser.h"
#include "VCHelper.h"

char *valueToString(void *toBePrinted)
{
    if (toBePrinted == NULL)
    {
        return NULL;
    }
    // List * temp = (List*) toBePrinted;
    // ListIterator iter = createIterator(temp->values);
    char *str = (char *)toBePrinted;
    char *tmpStr = malloc(sizeof(char) * strlen(str) + 20);
    // strcpy(tmpStr, str);
    sprintf(tmpStr, "%s", str);
    return tmpStr;
}

void deleteValue(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
    {
        return;
    }
    if (toBeDeleted != NULL)
    {
        free(toBeDeleted);
    }
}

int compareValues(const void *first, const void *second)
{
    if (first == NULL || second == NULL)
    {
        return 0; // return false
    }
    char *temp1 = (char *)first;
    char *temp2 = (char *)second;

    return strcmp(temp1, temp2);
}

void deleteParameter(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
    {
        return;
    }
    Parameter *tmp = (Parameter *)toBeDeleted;
    if (tmp->name)
    {
        free(tmp->name);
    }
    if (tmp->value)
    {
        free(tmp->value);
    }
    free(tmp);
}
int compareParameters(const void *first, const void *second)
{
    if (first == NULL || second == NULL)
    {
        return 0; // return false
    }
    Parameter *temp1 = (Parameter *)first;
    Parameter *temp2 = (Parameter *)second;
    // compare
    if (strcmp(temp1->name, temp2->name) == 0 && strcmp(temp1->value, temp2->value) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    return 0;
}

char *parameterToString(void *param)
{
    if (param == NULL)
    {
        return NULL;
    }

    Parameter *tmp = (Parameter *)param;
    int len = strlen(tmp->name) + strlen(tmp->value) + 100; // for the space
    char *str = (char *)malloc(sizeof(char) * len + 50);
    sprintf(str, "%s\n%s\n", tmp->name, tmp->value);
    return str;
}
void deleteProperty(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
    {
        return;
    }

    Property *temp = (Property *)toBeDeleted;
    if (temp->name)
    {
        free(temp->name);
    }
    if (temp->group)
    {
        free(temp->group);
    }
    if (temp->parameters != NULL)
    {
        freeList(temp->parameters);
    }
    if (temp->values != NULL)
    {
        freeList(temp->values);
    }
    free(temp);
}
int compareProperties(const void *first, const void *second)
{
    if (first == NULL || second == NULL)
    {
        return 0; // return false
    }
    Property *tmp1 = (Property *)first;
    Property *tmp2 = (Property *)second;
    // compare names
    if (strcmp(tmp1->name, tmp2->name) != 0)
    {
        return 0;
    }

    // compare groups
    if (strcmp(tmp1->group, tmp2->group) != 0)
    {
        return 0;
    }

    // compare values
    List *val1 = tmp1->values;
    List *val2 = tmp2->values;
    ListIterator iter1 = createIterator(val1);
    ListIterator iter2 = createIterator(val2);
    void *elem1;
    void *elem2;
    while ((elem1 = nextElement(&iter1)) != NULL && (elem2 = nextElement(&iter2)) != NULL)
    {
        char *valCmp1 = (char *)elem1;
        char *valCmp2 = (char *)elem2;
        if (strcmp(valCmp1, valCmp2) != 0)
        {
            return 0;
        }
    }

    // compare parameters
    List *par1 = tmp1->parameters;
    List *par2 = tmp2->parameters;
    ListIterator trav1 = createIterator(par1);
    ListIterator trav2 = createIterator(par2);
    while ((elem1 = nextElement(&trav1)) != NULL && (elem2 = nextElement(&trav2)) != NULL)
    {
        char *valCmp1 = (char *)elem1;
        char *valCmp2 = (char *)elem2;
        if (strcmp(valCmp1, valCmp2) != 0)
        {
            return 0;
        }
    }

    return 1;
}

char *propertyToString(void *prop)
{
    if (prop == NULL)
    {
        return NULL;
    }
    Property *temp = (Property *)prop;
    // find the size for each LL
    char *vallist = toString(temp->values);
    char *paramList = toString(temp->parameters);
    int len = strlen(temp->name) + strlen(temp->group) + strlen(paramList) + strlen(vallist) + 100;
    char *str = malloc(sizeof(char) * len);
    sprintf(str, "\nName:%s\nGroup:%s\nParams:%s\nValues:%s", temp->name, temp->group, paramList, vallist);
    free(vallist);
    free(paramList);
    return str;
}
void deleteDate(void *toBeDeleted)
{
    if (toBeDeleted == NULL)
    {
        return;
    }
    if (toBeDeleted != NULL)
    {
        DateTime *temp = (DateTime *)toBeDeleted;
        if (temp->date)
        {
            free(temp->date);
        }
        if (temp->time)
        {
            free(temp->time);
        }
        if (temp->text)
        {
            free(temp->text);
        }
        free(toBeDeleted);
    }
}
int compareDates(const void *first, const void *second)
{
    return 0; // stubbed function for now
}
char *dateToString(void *date)
{
    DateTime *temp = (DateTime *)date;
    // account for the spaces and bool in formatting
    int len = strlen(temp->date) + strlen(temp->time) + strlen(temp->text) + 4;
    char *dateStr = (char *)malloc(sizeof(char) * len);
    // copy to string
    sprintf(dateStr, "%s\n%s\n%s", temp->date, temp->time, temp->text);
    return dateStr;
}

char * propertyWrite(void * prop){
    //create a string to parse everything into
    if(prop == NULL){
        // char * temp = malloc(sizeof(char)*4);
        // strcpy(temp, "-1");
        return NULL;
    }
    Property * obj = (Property*) prop;
    if(obj == NULL||obj->name == NULL || obj->group == NULL||obj -> parameters == NULL ||obj -> values == NULL){
        // char * temp = malloc(sizeof(char)*4);
        // strcpy(temp, "-1");
        return NULL;
    }
    //get name
    char * strToFile  = malloc(sizeof(char)*strlen(obj->name) + 4);
    sprintf(strToFile, "%s", obj->name);

    //get group if it exists
    int groupLen = strlen(obj->group);
    if(groupLen != 0){ //group exists
        int totalLen = groupLen + strlen(strToFile); //get total length of the name + group
        char * strGrpName = malloc(sizeof(char)*totalLen +4);
        sprintf(strGrpName, "%s.%s", strToFile, obj->group);
        free(strToFile);
        strToFile = malloc(sizeof(char)*strlen(strGrpName)+4);
        strcpy(strToFile, strGrpName);
        free(strGrpName);
    }

    //now get the parameters
    //get the length to iterate the list
    int paramLLlen = getLength(obj->parameters);
    void * elem;
    char * params;
    int curSize = 1000;
    if(paramLLlen != 0){
        //left here, create an iterator and loop to concat the params
        ListIterator iter = createIterator(obj->parameters);
        params = malloc(curSize);
        sprintf(params, ";");
        while((elem = nextElement(&iter)) != NULL){
            Parameter * tempParam = (Parameter *) elem;
            char * paramName = tempParam -> name;
            char * paramVal = tempParam -> value;

            int paramLen = strlen(paramName) + strlen(paramVal);
            char * paramStr = malloc(sizeof(char) * paramLen + 40);

            sprintf(paramStr, "%s=%s;", paramName, paramVal);
            int len = strlen(paramStr) + strlen(params) + 40;
            //not enough space realloc
            if(curSize <= len){
                curSize = len + 1000;
                params = (char*)realloc(params, curSize);
                //params = temp;
            }
            strcat(params, paramStr);
            free(paramStr);
        }
        int lastInd = strlen(params);
        params[lastInd-1] = ':'; //remove the ';' at the end for a ':'

        //concat to the str
        int totalLen = strlen(strToFile)+strlen(params)+100;
        strToFile = realloc(strToFile, totalLen);
        strcat(strToFile, params);
        free(params);
    } else {
        strcat(strToFile, ":");
    }

    //get the values, there's always atleast one so, create the iterator
    ListIterator valTrav = createIterator(obj->values);
    void * trav;
    char * valtoStr = toString(obj->values);
    int valLLLen = (strlen(valtoStr)*2)+1000;
    free(valtoStr);
    //realloc the string to create space for the values
    strToFile  = realloc(strToFile, valLLLen);
    while((trav = nextElement(&valTrav)) != NULL){
        char * val = (char*) trav;
        int len = strlen(strToFile) + strlen(val) + 40;
        if(valLLLen <=  len){
            valLLLen = len + 1000;
            strToFile = realloc(strToFile, valLLLen);
        }
        strcat(strToFile, val);
        strcat(strToFile, ";");
    }
    int finalLen = strlen(strToFile);
    strToFile[finalLen-1] = '\0'; //remove the last ";"

    return strToFile;
}

char * dateTimeWrite(void * DT, bool bday){
    if(DT == NULL){
        return NULL;
    }
    DateTime * dateProp = (DateTime*) DT;
    if(dateProp == NULL||dateProp->date == NULL || dateProp->time ==NULL||dateProp->text == NULL){
        return NULL;
    }
    int len = strlen(dateProp -> date) + strlen(dateProp -> time) + strlen(dateProp -> text) + 200;
    char * str = malloc(sizeof(char)*len);

    //getting the right format
    if(bday == true){
        sprintf(str, "BDAY"); 
    } else {
        sprintf(str, "ANNIVERSARY");
    }

    //getting the str
    if(dateProp -> isText == true){
        strcat(str, ";VALUE=text:");
        strcat(str, dateProp -> text);
    } else {
        strcat(str, ":");
        char * date = dateProp -> date;
        int datelen = strlen(date);

        if(datelen != 0){ //there is a date
            strcat(str, date);
        }

        char * time = dateProp -> time;
        int timelen = strlen(time);
        if(timelen != 0){
            strcat(str, "T");
            strcat(str, time);
        }
    }

    //adding utc indicator if needed
    if(dateProp -> UTC == true){
        strcat(str, "Z");
    }

    return str;
}

bool propValidate(void * prop, char * id){
    if(prop == NULL){
        return false;
    }

    Property * validprop = (Property *)prop;
    //validate the specifics, values must have one minimum
    ListIterator iter = createIterator(validprop->values);
    void *elem;
    char * val;
    if ((elem = nextElement(&iter)) != NULL){
        val = (char *)elem;
    }
    //checking if theres the value req
    if(getLength(validprop->values) == 0 && strcmp(val, "") == 0){
        return false;
    }

    if(strcmp(id, "SOURCE") == 0){

    } else if(strcmp(id, "KIND") == 0){

    }else if(strcmp(id, "XML") == 0){

    }else if(strcmp(id, "FN") == 0){

    }else if(strcmp(id, "N") == 0){
        //N must have a cardinality of 5
        int listLen = getLength(validprop->values);
        if(listLen != 5){
            return false;
        }

    }else if(strcmp(id, "NICKNAME") == 0){

    }else if(strcmp(id, "PHOTO") == 0){

    }else if(strcmp(id, "GENDER") == 0){
        //N must have a cardinality of 1-2
        int listLen = getLength(validprop->values);
        if(listLen < 1 || listLen > 2){
            return false;
        }

    }else if(strcmp(id, "ADR") == 0){
        //ADR must have a cardinality of 7
        int listLen = getLength(validprop->values);
        if(listLen != 7){
            return false;
        }

    }else if(strcmp(id, "TEL") == 0){

    }else if(strcmp(id, "EMAIL") == 0){

    }else if(strcmp(id, "IMPP") == 0){

    }else if(strcmp(id, "LANG") == 0){

    }else if(strcmp(id, "TZ") == 0){

    }else if(strcmp(id, "GEO") == 0){

    }else if(strcmp(id, "TITLE") == 0){

    }else if(strcmp(id, "ROLE") == 0){

    }else if(strcmp(id, "LOGO") == 0){

    }else if(strcmp(id, "ORG") == 0){

    }else if(strcmp(id, "MEMBER") == 0){

    }else if(strcmp(id, "RELATED") == 0){

    }else if(strcmp(id, "CATEGORIES") == 0){

    }else if(strcmp(id, "NOTE") == 0){

    }else if(strcmp(id, "PRODID") == 0){

    }else if(strcmp(id, "REV") == 0){

    }else if(strcmp(id, "SOUND") == 0){

    }else if(strcmp(id, "UID") == 0){

    }else if(strcmp(id, "CLIENTPIDMAP") == 0){
        //CLIENTPIDMAP must have a cardinality of 2
        int listLen = getLength(validprop->values);
        if(listLen != 2){
            return false;
        }
    }else if(strcmp(id, "URL") == 0){

    }else if(strcmp(id, "KEY") == 0){

    }else if(strcmp(id, "FBURL") == 0){

    }else if(strcmp(id, "CALADBURI") == 0){

    }else if(strcmp(id, "CALURI") == 0){

    }else {
        return false;
    }

    //validate params
    int len = getLength(validprop->parameters);
    if (len == 0){
        return false;
    }
    ListIterator trav = createIterator(validprop->parameters);
    //void *elem;
    while ((elem = nextElement(&trav)) != NULL)
    {
        Parameter * param = (Parameter*)elem;
        bool paramValid = paramValidate(param); //call to check that the parameter is valid
        if(paramValid == false){
            return false;
        }
    }

    return true;
}

bool paramValidate(void * param){
    if(param == NULL){
        return false;
    }
    Parameter * temp = (Parameter *) param;

    //check for parameter node
    if(temp-> name == NULL || temp -> value == NULL || strcmp(temp-> name, "") == 0 || strcmp(temp-> value, "") == 0){
        return false;
    }

    return true;
}

bool dtValidate(void * prop){
    if(prop == NULL){
        return false;
    }
    DateTime* dt = (DateTime *) prop;
    bool utc = dt -> UTC;
    bool text = dt ->isText;

    // check if the values are filled accordingly
    if(utc == true && text == true){
        return false;
    }

    //text is true but there are values in date time and none for text
    if(text == true && (strlen(dt->date) != 0 || strlen(dt->time) != 0)){
        return false;
    } else if (text == false && strlen(dt->text) != 0){
        return false;
    }

    //checking for the correct length and digits or -
    //this is only checked when text is false
    if(text == false){
        //check for date
        if(strlen(dt->date) != 0){
            char * date =  dt->date;
            if(strlen(date) != 8){
                return false;
            }
            //find if theres anything but - or 1-9
            for(int i = 0; i < strlen(date); i++){
                if(date[i] != '-' && (isdigit(date[i]) == false)){
                    return false;
                }
            }
        }

        //check for time
        if(strlen(dt->time) != 0){
            char * time =  dt->time;
            if(strlen(time) != 6){
                return false;
            }
            //find if theres anything but - or 1-9
            for(int i = 0; i < strlen(time); i++){
                if(time[i] != '-' && (isdigit(time[i]) == false)){
                    return false;
                }
            }
        }

    }

    return true;
}