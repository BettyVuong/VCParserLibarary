// Betty Vuong
// Student ID: 1271673
#include "LinkedListAPI.h"
#include "VCParser.h"
#include "VCHelper.h"

VCardErrorCode createCard(char *fileName, Card **obj)
{
    char *version = malloc(sizeof(char) * 100); // version syntax
    char *str;
    char *nextLine;
    int count = 0;
    // int buffLen = 0;
    int maxLen = 0; // max buffer size
    bool FNFound = false;
    bool bdayFound = false;
    bool anniFound = false;
    bool fold = true;
    bool FNOptional = false;
    bool endFound = false;

    // file input validity
    // int fileLen = strlen(fileName);
    if (fileName == NULL)
    {
        free(version);
        return INV_FILE;
    }

    if (strstr(fileName, ".vcf") == NULL && strstr(fileName, ".vcard") == NULL)
    {
        free(version);
        return INV_FILE;
    }

    // open stream and file buffer
    FILE *fp = fopen(fileName, "r");
    // file error handling
    if (fp == NULL)
    {
        free(version);
        //fclose(fp);
        return INV_FILE;
    }

    // open stream and buffer for line unfolding
    FILE *unfold = fopen(fileName, "r");
    // file error handling
    if (unfold == NULL)
    {
        free(version);
        fclose(fp);
        return INV_FILE;
    }

    // finding the buffer size
    while (!feof(fp))
    {
        count = fgetc(fp);
        maxLen++;
    }
    maxLen += 100;
    count = 0;

    // reset fp to the start of the file
    rewind(fp);

    // find end card
    char *findLine = malloc(sizeof(char) * maxLen + 1);
    while (!feof(fp))
    {
        if (fgets(findLine, maxLen, fp) == NULL)
        {
            char *delim = strstr(findLine, "\r\n");
            if (delim != NULL)
            {
                *delim = '\0'; // removing the vcard formatting
            }
            if (strcmp(findLine, "END:VCARD") == 0)
            {
                endFound = true;
            }
            free(findLine);
        }
    }

    if (endFound == false)
    {
        free(version);
        fclose(fp);
        fclose(unfold);
        return INV_CARD;
    }

    rewind(fp);

    *obj = malloc(sizeof(Card));
    if (*obj == NULL)
    {
        free(version);
        fclose(unfold);
        fclose(fp);
        return OTHER_ERROR;
    }

    // start reading file to parse
    while (!feof(fp))
    {
        str = malloc((sizeof(char) * maxLen) + 1);
        if (fgets(str, maxLen, fp) == NULL)
        {
            free(str);
            break;
        }
        char *delim = strstr(str, "\r\n");
        if (delim != NULL)
        {
            *delim = '\0'; // removing the vcard formatting
        }

        int check = 0;
        long memPos = 0;
        // code for line unfolding
        fold = true; // resets fold for the next iteration
        while (fold == true && !feof(unfold))
        {
            if (check == 0)
            {
                nextLine = malloc(sizeof(char) * maxLen); // to read the next line
                fgets(nextLine, maxLen, unfold);          // goes ahead by one
                free(nextLine);
                check++;
            }

            int skips = 0;
            do
            {
                if (!feof(unfold))
                {
                    nextLine = malloc(sizeof(char) * maxLen); // to read the next line
                    // fgets(nextLine, maxLen, unfold);          // goes ahead by one
                    // save memory position before the next line read
                    memPos = ftell(unfold);
                    if (fgets(nextLine, maxLen, unfold) == NULL)
                    {
                        free(nextLine);
                        break;
                    }
                    // printf("trace %s", nextLine);
                }
                else
                {
                    break;
                }

                // finding for space at the start of the line
                if (nextLine[0] == ' ' || nextLine[0] == '\t')
                {
                    // finding the CRLF
                    char *clear = strstr(nextLine, "\r\n");
                    if (clear != NULL)
                    {
                        *clear = '\0'; // removing the vcard formatting
                    }
                    memmove(nextLine, nextLine + 1, strlen(nextLine));
                    // printf("\nmoved: %s", nextLine);
                    strcat(str, nextLine); // concats to make a line
                    free(nextLine);
                    skips++;
                }
                else
                {                 // line folding stops
                    fold = false; // stop the looping
                                  // printf("\ncat: %s", str);
                    free(nextLine);
                    // revert back a line
                    fseek(unfold, memPos, SEEK_SET);
                }

            } while (fold); // keep iterating and cat to the str until there's no longer any folding aka ' '
            for (int i = 0; i < skips; i++)
            {
                nextLine = malloc(sizeof(char) * maxLen); // to read the next line
                fgets(nextLine, maxLen, fp);              // goes ahead by one
                                                          // printf("\nrewind%s", nextLine);
                free(nextLine);
            }
        }

        // add spaces between consecutive delimeters
        int lenParse = strlen(str);
        char *strdupe = malloc(sizeof(char) * lenParse + 100);
        int j = 0;

        for (int i = 0; i < lenParse; i++)
        {
            strdupe[j++] = str[i];
            if ((str[i] == ':' || str[i] == ';') && (str[i + 1] == ';'))
            {
                strdupe[j++] = ' ';
            }
        }
        strdupe[j] = '\0';

        // copy over
        int dupeLen = j + 1;
        free(str);
        str = malloc(sizeof(char) * dupeLen + 100);
        strcpy(str, strdupe);
        free(strdupe);

        // printf("\nfinal: %s", str);

        // ensuring that the form follows with begin vcard
        if (count == 0)
        {
            if (strcmp(str, "BEGIN:VCARD") != 0)
            { // checks if the format is right
                free(str);
                free(version);
                // free(name);
                fclose(fp);
                fclose(unfold);
                return INV_CARD;
            }
        }
        else if (count == 1)
        { // checking if the vcard format follows with the version spec
            strcpy(version, str);

            if (strcmp(version, "VERSION:4.0") != 0)
            { // checks if the format is right
                free(str);
                free(version);
                fclose(fp);
                fclose(unfold);
                return INV_CARD;
            }
            // create the LL
            (*obj)->fn = malloc(sizeof(Property));
            (*obj)->birthday = NULL;
            (*obj)->anniversary = NULL;
            (*obj)->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
            (*obj)->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
            (*obj)->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);
        }
        else
        {
            if (strcmp(str, "END:VCARD") == 0)
            { // end v card

                free(str);
                str = malloc((sizeof(char) * maxLen) + 1);

                if (fgets(str, maxLen, fp) != NULL)
                {
                    if (strcmp(str, "\n") != 0 && FNFound == false)
                    {
                        deleteCard((*obj)); // invalid card text format
                        // printf("%s", str);
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        // printf("fail");
                        return INV_CARD;
                    }
                }
                if (FNFound == false)
                {
                    deleteCard((*obj)); // invalid card text format
                    // printf("%s", str);
                    free(str);
                    free(version);
                    fclose(fp);
                    fclose(unfold);
                    // printf("fail");
                    return INV_CARD;
                }
            }
            else if (strstr(str, "BDAY") != NULL && !bdayFound)
            {
                // this case is for dates struct type
                if (!bdayFound)
                { // there's only one BDAY
                    bdayFound = true;
                }
                char *propName = strstr(str, ";");
                char *valStart = strstr(str, ":"); // finds the value start

                // each request must have atleast one value, the lack of : means format is wrong
                if (valStart == NULL)
                {
                    deleteCard((*obj)); // invalid card text format
                    free(str);
                    free(version);
                    fclose(fp);
                    fclose(unfold);
                    return INV_PROP;
                }

                // initialize values
                (*obj)->birthday = malloc(sizeof(DateTime));
                (*obj)->birthday->UTC = false;
                (*obj)->birthday->isText = false;

                // case for only values no param
                if (propName == NULL || propName > valStart)
                {
                    (*obj)->birthday->text = malloc(2);
                    strcpy((*obj)->birthday->text, ""); // assume empty because of parameter

                    // cut to just the values
                    int len = (valStart - str);

                    // get the value or values
                    memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value

                    if (strlen(str) == 0)
                    {
                        (*obj)->birthday->time = NULL;
                        (*obj)->birthday->date = NULL;
                        deleteCard((*obj)); // invalid card text format
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        // free(datestr);
                        return INV_PROP;
                    }

                    int end = strlen(str);
                    if (str[end - 1] == 'Z')
                    {
                        (*obj)->birthday->UTC = true;
                        // remove the z
                        str[end - 1] = '\0';
                    }
                    // printf("%s", str);

                    // start finding if its date time etc
                    char *findTime = strstr(str, "T");
                    // there is a date and time
                    if (findTime != NULL)
                    {
                        // split the date and time
                        int dateLen = findTime - str;
                        //  there's a date
                        if (dateLen > 0)
                        {
                            char *datestr = malloc(sizeof(char) * dateLen + 1);
                            strncpy(datestr, str, dateLen);
                            datestr[dateLen] = '\0';
                            // printf("%s", datestr);
                            //  place into date
                            (*obj)->birthday->date = malloc(sizeof(char) * strlen(datestr) + 1);
                            strcpy((*obj)->birthday->date, datestr);
                            free(datestr);
                            // get just the time
                            memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                            // no time
                            if (strlen(str) == 0)
                            {
                                deleteCard((*obj)); // invalid card text format
                                free(str);
                                free(version);
                                fclose(fp);
                                fclose(unfold);
                                return INV_PROP;
                            }
                            // copy str
                            char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                            strcpy(timeCpy, str);
                            (*obj)->birthday->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                            strcpy((*obj)->birthday->time, timeCpy);
                            free(timeCpy);
                        }
                        else
                        {
                            (*obj)->birthday->date = malloc((2));
                            strcpy((*obj)->birthday->date, "");
                            // get just the time
                            memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                            // no time
                            if (strlen(str) == 0)
                            {
                                deleteCard((*obj)); // invalid card text format
                                free(str);
                                free(version);
                                fclose(fp);
                                fclose(unfold);
                                return INV_PROP;
                            }
                            // copy str
                            char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                            strcpy(timeCpy, str);
                            (*obj)->birthday->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                            strcpy((*obj)->birthday->time, timeCpy);
                            free(timeCpy);

                        }
                    }
                    else
                    { // just date no time
                        // findTime = '\0';
                        (*obj)->birthday->time = malloc(sizeof(char) * (2));
                        strcpy((*obj)->birthday->time, "");
                        int dateLen = strlen(str) + 1;
                        char *dateCpy = malloc(sizeof(char) * dateLen + 1);
                        strcpy(dateCpy, str);
                        (*obj)->birthday->date = malloc(sizeof(char) * strlen(dateCpy) + 1);
                        strcpy((*obj)->birthday->date, dateCpy);
                        free(dateCpy);
                    }
                }
                else if (propName < valStart)
                { // this case is checks for parameter

                    int len = (valStart - str);
                    char *nameParam = malloc(sizeof(char) * len + 1);
                    int nameLen = (propName - str); // get the name, possibly group, and params

                    // getting just the name and possibly group
                    char *pName = malloc(sizeof(char) * nameLen + 1);
                    strncpy(nameParam, str + nameLen + 1, len - nameLen - 1); // copy
                    nameParam[len - nameLen - 1] = '\0';
                    strncpy(pName, str, nameLen); // copy
                    pName[nameLen] = '\0';

                    // if there's no name it violates the vcard specs
                    if (strlen(pName) == 0)
                    {
                        deleteCard((*obj)); // invalid card text format
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        return INV_PROP;
                    }
                    free(pName);

                    // getting property
                    char *paramSemiCol = strtok(nameParam, ";");
                    if (paramSemiCol == NULL)
                    { // assume default date and time parsing
                      // insert code above here
                    }
                    else
                    {
                        char *seperate = strstr(paramSemiCol, "=");
                        // violates vcard spec for param
                        if (seperate == NULL)
                        {
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            return INV_PROP;
                        }

                        // finding the param name
                        int paramLabelLen = seperate - paramSemiCol;
                        char *paramLabel = malloc(sizeof(char) * paramLabelLen + 1);
                        strncpy(paramLabel, paramSemiCol, paramLabelLen);
                        paramLabel[paramLabelLen] = '\0';

                        // finding the param value
                        char *paramValCpy = seperate + 1;
                        int paramValLen = strlen(paramValCpy);
                        char *paramVal = malloc(sizeof(char) * paramValLen + 1);
                        strcpy(paramVal, paramValCpy);
                        paramVal[paramValLen] = '\0';

                        paramSemiCol = strtok(NULL, ";");

                        // violates the vcard spec for parameters
                        if (strlen(paramVal) == 0 || strlen(paramLabel) == 0)
                        {
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            free(paramLabel);
                            free(paramVal);
                            // free(pName);
                            return INV_PROP;
                        }

                        // checking which parameter request this is
                        // if its text change other specs in the struct
                        if (strcmp(paramVal, "text") == 0)
                        {
                            // set text to true
                            (*obj)->birthday->isText = true;
                            // set the values when test is true to empty
                            (*obj)->birthday->time = malloc(sizeof(char) * (2));
                            strcpy((*obj)->birthday->time, "");
                            (*obj)->birthday->date = malloc((2));
                            strcpy((*obj)->birthday->date, "");

                            int end = strlen(str);
                            // detect for utc
                            if (str[end - 1] == 'Z')
                            {
                                (*obj)->birthday->UTC = true;
                                // remove the z
                                str[end - 1] = '\0';
                            }

                            // taking the value
                            //  get the value or values
                            memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value
                            char *txt = malloc(sizeof(char) * strlen(str) + 1);
                            strcpy(txt, str);
                            (*obj)->birthday->text = malloc(sizeof(char) * strlen(txt) + 1);
                            strcpy((*obj)->birthday->text, txt);
                            free(txt);
                        }
                        else if (strcmp(paramVal, "date-and-or-time") == 0)
                        {
                            (*obj)->birthday->text = malloc(2);
                            strcpy((*obj)->birthday->text, ""); // assume empty because of parameter

                            // cut to just the values
                            int len = (valStart - str);

                            // get the value or values
                            memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value
                            // TEST
                            // printf("parsed %s", str);

                            if (strlen(str) == 0)
                            {
                                (*obj)->birthday->time = NULL;
                                (*obj)->birthday->date = NULL;
                                deleteCard((*obj)); // invalid card text format
                                free(str);
                                free(version);
                                fclose(fp);
                                fclose(unfold);
                                free(paramLabel);
                                free(paramVal);
                                free(nameParam);
                                return INV_PROP;
                            }

                            int end = strlen(str);
                            if (str[end - 1] == 'Z')
                            {
                                (*obj)->birthday->UTC = true;
                                // remove the z
                                str[end - 1] = '\0';
                            }
                             //printf("%s", str);

                            // start finding if its date time etc
                            char *findTime = strstr(str, "T");
                            // there is a date and time
                            if (findTime != NULL)
                            {
                                // split the date and time
                                int dateLen = findTime - str;
                                //printf("hi%d", dateLen);
                                //  there's a date
                                if (dateLen > 0)
                                {
                                    char *datestr = malloc(sizeof(char) * dateLen + 1);
                                    strncpy(datestr, str, dateLen);
                                    datestr[dateLen] = '\0';
                                    // printf("%s", datestr);
                                    //  place into date
                                    (*obj)->birthday->date = malloc(sizeof(char) * strlen(datestr) + 1);
                                    strcpy((*obj)->birthday->date, datestr);
                                    free(datestr);
                                    // get just the time
                                    memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                                    // no time
                                    if (strlen(str) == 0)
                                    {
                                        deleteCard((*obj)); // invalid card text format
                                        free(str);
                                        free(version);
                                        fclose(fp);
                                        fclose(unfold);
                                        return INV_PROP;
                                    }
                                    // copy str
                                    char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                                    strcpy(timeCpy, str);
                                    //printf("%s\n", str);
                                    (*obj)->birthday->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                                    strcpy((*obj)->birthday->time, timeCpy);
                                    //printf("%s\n", timeCpy);
                                    free(timeCpy);

                                    //printf("%s", (*obj)->birthday->time);
                                }
                                else
                                {
                                    (*obj)->birthday->date = malloc((2));
                                    strcpy((*obj)->birthday->date, "");
                                    // get just the time
                                    memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                                    // no time
                                    if (strlen(str) == 0)
                                    {
                                        deleteCard((*obj)); // invalid card text format
                                        free(str);
                                        free(version);
                                        fclose(fp);
                                        fclose(unfold);
                                        return INV_PROP;
                                    }
                                    // copy str
                                    char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                                    strcpy(timeCpy, str);
                                    (*obj)->birthday->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                                    strcpy((*obj)->birthday->time, timeCpy);
                                    free(timeCpy);
                                }
                            }
                            else
                            { // just date no time
                                // findTime = '\0';
                                (*obj)->birthday->time = malloc(sizeof(char) * (2));
                                strcpy((*obj)->birthday->time, "");
                                int dateLen = strlen(str) + 1;
                                char *dateCpy = malloc(sizeof(char) * dateLen + 1);
                                strcpy(dateCpy, str);
                                (*obj)->birthday->date = malloc(sizeof(char) * strlen(dateCpy) + 1);
                                strcpy((*obj)->birthday->date, dateCpy);
                                free(dateCpy);
                            }
                        }
                        else
                        {                       // violation of structure
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            free(paramVal);
                            return INV_PROP;
                        }
                        free(nameParam);
                        free(paramLabel);
                        free(paramVal);
                    }
                }

                //printf("%s", (*obj)->birthday->date);
            }
            else if (strstr(str, "ANNIVERSARY") != NULL && !anniFound)
            {
                // flags to ensure that theres only one instance of struct type datetime
                 //printf("hi%s", str);
                if (!anniFound)
                {
                    anniFound = true;
                }
                char *propName = strstr(str, ";");
                char *valStart = strstr(str, ":"); // finds the value start

                // each request must have atleast one value, the lack of : means format is wrong
                if (valStart == NULL)
                {
                    deleteCard((*obj)); // invalid card text format
                    // printf("hi%s", str);
                    free(str);
                    free(version);
                    fclose(fp);
                    fclose(unfold);
                    return INV_PROP;
                }

                // initialize values
                (*obj)->anniversary = malloc(sizeof(DateTime));
                (*obj)->anniversary->UTC = false;
                (*obj)->anniversary->isText = false;

                // case for only values no param
                if (propName == NULL || propName > valStart)
                {
                    (*obj)->anniversary->text = malloc(2);
                    strcpy((*obj)->anniversary->text, ""); // assume empty because of parameter

                    // cut to just the values
                    int len = (valStart - str);

                    // get the value or values
                    memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value

                    if (strlen(str) == 0)
                    {
                        (*obj)->anniversary->time = NULL;
                        (*obj)->anniversary->date = NULL;
                        deleteCard((*obj)); // invalid card text format
                        // printf("hi%s", str);
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        return INV_PROP;
                    }

                    int end = strlen(str);
                    if (str[end - 1] == 'Z')
                    {
                        (*obj)->anniversary->UTC = true;
                        // remove the z
                        str[end - 1] = '\0';
                    }
                    // printf("%s", str);

                    // start finding if its date time etc
                    char *findTime = strstr(str, "T");
                    // there is a date and time
                    if (findTime != NULL)
                    {
                        // split the date and time
                        int dateLen = findTime - str;
                        // printf("len %d ", dateLen);
                        //   there's a date
                        if (dateLen > 0)
                        {
                            char *datestr = malloc(sizeof(char) * dateLen + 1);
                            strncpy(datestr, str, dateLen);
                            datestr[dateLen] = '\0';
                            // printf("%s", datestr);
                            //  place into date
                            (*obj)->anniversary->date = malloc(sizeof(char) * strlen(datestr) + 1);
                            strcpy((*obj)->anniversary->date, datestr);
                            free(datestr);
                            // get just the time
                            memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                            // no time
                            if (strlen(str) == 0)
                            {
                                deleteCard((*obj)); // invalid card text format
                                // printf("hi%s", str);
                                free(str);
                                free(version);
                                fclose(fp);
                                fclose(unfold);
                                return INV_PROP;
                            }
                            // copy str
                            char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                            strcpy(timeCpy, str);
                            // printf("lol");
                            (*obj)->anniversary->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                            strcpy((*obj)->anniversary->time, timeCpy);
                            free(timeCpy);
                        }
                        else
                        {
                            (*obj)->anniversary->date = malloc((2));
                            strcpy((*obj)->anniversary->date, "");
                            // get just the time
                            memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                            // no time
                            if (strlen(str) == 0)
                            {
                                deleteCard((*obj)); // invalid card text format
                                // printf("hi%s", str);
                                free(str);
                                free(version);
                                fclose(fp);
                                fclose(unfold);
                                // free(datestr);
                                return INV_PROP;
                            }
                            // copy str
                            char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                            strcpy(timeCpy, str);
                            (*obj)->anniversary->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                            strcpy((*obj)->anniversary->time, timeCpy);
                            free(timeCpy);
                        }
                    }
                    else
                    { // just date no time
                        // findTime = '\0';
                        (*obj)->anniversary->time = malloc(sizeof(char) * (2));
                        strcpy((*obj)->anniversary->time, "");
                        int dateLen = strlen(str) + 1;
                        char *dateCpy = malloc(sizeof(char) * dateLen + 1);
                        strcpy(dateCpy, str);
                        (*obj)->anniversary->date = malloc(sizeof(char) * strlen(dateCpy) + 1);
                        strcpy((*obj)->anniversary->date, dateCpy);
                        free(dateCpy);
                    }
                }
                else if (propName < valStart)
                { // this case is checks for parameter

                    int len = (valStart - str);
                    char *nameParam = malloc(sizeof(char) * len + 1);
                    int nameLen = (propName - str); // get the name, possibly group, and params

                    // getting just the name and possibly group
                    char *pName = malloc(sizeof(char) * nameLen + 1);
                    strncpy(nameParam, str + nameLen + 1, len - nameLen - 1); // copy
                    nameParam[len - nameLen - 1] = '\0';
                    strncpy(pName, str, nameLen); // copy
                    pName[nameLen] = '\0';

                    // if there's no name it violates the vcard specs
                    if (strlen(pName) == 0)
                    {
                        deleteCard((*obj)); // invalid card text format
                        // printf("hi%s", str);
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        return INV_PROP;
                    }
                    free(pName);

                    // getting property
                    char *paramSemiCol = strtok(nameParam, ";");
                    if (paramSemiCol == NULL)
                    { // assume default date and time parsing
                      // insert code above here
                    }
                    else
                    {
                        char *seperate = strstr(paramSemiCol, "=");
                        // violates vcard spec for param
                        if (seperate == NULL)
                        {
                            deleteCard((*obj)); // invalid card text format
                            // printf("hi%s", str);
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            return INV_PROP;
                        }

                        // finding the param name
                        int paramLabelLen = seperate - paramSemiCol;
                        char *paramLabel = malloc(sizeof(char) * paramLabelLen + 1);
                        strncpy(paramLabel, paramSemiCol, paramLabelLen);
                        paramLabel[paramLabelLen] = '\0';

                        // finding the param value
                        char *paramValCpy = seperate + 1;
                        int paramValLen = strlen(paramValCpy);
                        char *paramVal = malloc(sizeof(char) * paramValLen + 1);
                        strcpy(paramVal, paramValCpy);
                        paramVal[paramValLen] = '\0';

                        paramSemiCol = strtok(NULL, ";");

                        // violates the vcard spec for parameters
                        if (strlen(paramVal) == 0 || strlen(paramLabel) == 0)
                        {
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            free(paramLabel);
                            free(paramVal);
                            // free(pName);
                            return INV_PROP;
                        }

                        // checking which parameter request this is
                        // if its text change other specs in the struct
                        if (strcmp(paramVal, "text") == 0)
                        {
                            // set text to true
                            (*obj)->anniversary->isText = true;
                            // set the values when test is true to empty
                            (*obj)->anniversary->time = malloc(sizeof(char) * (2));
                            strcpy((*obj)->anniversary->time, "");
                            (*obj)->anniversary->date = malloc((2));
                            strcpy((*obj)->anniversary->date, "");

                            int end = strlen(str);
                            // detect for utc
                            if (str[end - 1] == 'Z')
                            {
                                (*obj)->anniversary->UTC = true;
                                // remove the z
                                str[end - 1] = '\0';
                            }

                            // taking the value
                            //  get the value or values
                            memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value
                            char *txt = malloc(sizeof(char) * strlen(str) + 1);
                            strcpy(txt, str);
                            (*obj)->anniversary->text = malloc(sizeof(char) * strlen(txt) + 1);
                            strcpy((*obj)->anniversary->text, txt);
                            free(txt);
                        }
                        else if (strcmp(paramVal, "date-and-or-time") == 0)
                        {
                            (*obj)->anniversary->text = malloc(2);
                            strcpy((*obj)->anniversary->text, ""); // assume empty because of parameter

                            // cut to just the values
                            int len = (valStart - str);

                            // get the value or values
                            memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value
                            // TEST
                            // printf("parsed %s", str);

                            if (strlen(str) == 0)
                            {
                                (*obj)->anniversary->time = NULL;
                                (*obj)->anniversary->date = NULL;
                                deleteCard((*obj)); // invalid card text format
                                free(str);
                                free(version);
                                fclose(fp);
                                fclose(unfold);
                                free(paramLabel);
                                free(paramVal);
                                free(nameParam);
                                return INV_PROP;
                            }

                            int end = strlen(str);
                            if (str[end - 1] == 'Z')
                            {
                                (*obj)->anniversary->UTC = true;
                                // remove the z
                                str[end - 1] = '\0';
                            }
                            // printf("%s", str);

                            // start finding if its date time etc
                            char *findTime = strstr(str, "T");
                            // there is a date and time
                            if (findTime != NULL)
                            {
                                // split the date and time
                                int dateLen = findTime - str;
                                // printf("len %d ", dateLen);
                                //  there's a date
                                if (dateLen > 0)
                                {
                                    char *datestr = malloc(sizeof(char) * dateLen + 1);
                                    strncpy(datestr, str, dateLen);
                                    datestr[dateLen] = '\0';
                                    // printf("%s", datestr);
                                    //  place into date
                                    (*obj)->anniversary->date = malloc(sizeof(char) * strlen(datestr) + 1);
                                    strcpy((*obj)->anniversary->date, datestr);
                                    free(datestr);
                                    // get just the time
                                    memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                                    // no time
                                    if (strlen(str) == 0)
                                    {
                                        deleteCard((*obj)); // invalid card text format
                                        free(str);
                                        free(version);
                                        fclose(fp);
                                        fclose(unfold);
                                        return INV_PROP;
                                    }
                                    // copy str
                                    char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                                    strcpy(timeCpy, str);
                                    (*obj)->anniversary->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                                    strcpy((*obj)->anniversary->time, timeCpy);
                                    free(timeCpy);
                                }
                                else
                                {
                                    (*obj)->anniversary->date = malloc((2));
                                    strcpy((*obj)->anniversary->date, "");
                                    // get just the time
                                    memmove(str, str + dateLen + 1, strlen(str + dateLen + 1) + 1); // this is enough if theres only one value
                                    // no time
                                    if (strlen(str) == 0)
                                    {
                                        deleteCard((*obj)); // invalid card text format
                                        free(str);
                                        free(version);
                                        fclose(fp);
                                        fclose(unfold);
                                        return INV_PROP;
                                    }
                                    // copy str
                                    char *timeCpy = malloc(sizeof(char) * strlen(str) + 1);
                                    strcpy(timeCpy, str);
                                    (*obj)->anniversary->time = malloc(sizeof(char) * strlen(timeCpy) + 1);
                                    strcpy((*obj)->anniversary->time, timeCpy);
                                    free(timeCpy);
                                }
                            }
                            else
                            { // just date no time
                                // findTime = '\0';
                                (*obj)->anniversary->time = malloc(sizeof(char) * (2));
                                strcpy((*obj)->anniversary->time, "");
                                int dateLen = strlen(str) + 1;
                                char *dateCpy = malloc(sizeof(char) * dateLen + 1);
                                strcpy(dateCpy, str);
                                (*obj)->anniversary->date = malloc(sizeof(char) * strlen(dateCpy) + 1);
                                strcpy((*obj)->anniversary->date, dateCpy);
                                free(dateCpy);
                            }
                        }
                        else
                        {                       // violation of structure
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            free(paramVal);
                            return INV_PROP;
                        }
                        free(nameParam);
                        free(paramLabel);
                        free(paramVal);
                    }
                }
                //printf("%s", (*obj)->anniversary->date);
            }
            else
            { // parsing all other types
                // find the general cases which are a required property name, value, and optional params
                // this finds prop name
                char *propName = strstr(str, ";");
                char *valStart = strstr(str, ":"); // finds the value start

                // each request must have atleast one value, the lack of : means format is wrong
                if (valStart == NULL)
                {
                    deleteCard((*obj)); // invalid card text format
                    free(str);
                    free(version);
                    fclose(fp);
                    fclose(unfold);
                    return INV_PROP;
                }

                // create the instance of the object to parse
                Property *newProp = malloc(sizeof(Property));

                // no parameter just property or group parse
                if (propName == NULL || propName > valStart)
                {
                    // get the property name
                    int len = (valStart - str);
                    char *pName = malloc(sizeof(char) * len + 1);
                    strncpy(pName, str, len); // copy
                    pName[len] = '\0';

                    // if there's no name it violates the vcard specs
                    if (strlen(pName) == 0)
                    {
                        deleteCard((*obj)); // invalid card text format
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        free(newProp);
                        free(pName);
                        return INV_PROP;
                    }
                    // TEST
                    // printf("\n%s", pName);

                    // parse for fn into fn property in the struct
                    char *upperName = malloc(sizeof(char) * strlen(pName) + 1);
                    strcpy(upperName, pName);
                    for (int i = 0; i < strlen(upperName); i++)
                    {
                        upperName[i] = toupper(upperName[i]);
                    }
                    if (strstr(upperName, "FN") != NULL && !FNFound)
                    {
                        FNFound = true;
                        free(newProp);
                    }
                    else
                    {
                        FNOptional = true;
                        newProp->values = initializeList(&valueToString, &deleteValue, &compareValues);
                        newProp->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                    }
                    free(upperName);

                    // check if theres a group
                    char *isGroup = strstr(str, ".");
                    // this means there is a group as the period is before the seperator
                    if (isGroup != NULL && isGroup < valStart)
                    {
                        int groupLen = isGroup - str;
                        char *groupStr = malloc(sizeof(char) * groupLen + 1);
                        strncpy(groupStr, pName, groupLen);
                        groupStr[groupLen] = '\0';
                        // removes group from name
                        memmove(pName, pName + groupLen + 1, strlen(pName + groupLen + 1) + 1); // copies the name after group
                        // insert group and name into struct
                        if (FNFound && !FNOptional)
                        {
                            (*obj)->fn->name = malloc(sizeof(char) * strlen(pName) + 1);
                            (*obj)->fn->group = malloc(sizeof(char) * strlen(groupStr) + 1);
                            strcpy((*obj)->fn->group, groupStr);
                            strcpy((*obj)->fn->name, pName);
                            free(groupStr);
                            free(pName);
                            // printf("hi");
                        }
                        else
                        {
                            newProp->name = malloc(sizeof(char) * strlen(pName) + 1);
                            newProp->group = malloc(sizeof(char) * strlen(groupStr) + 1);
                            strcpy(newProp->group, groupStr);
                            strcpy(newProp->name, pName);
                            free(groupStr);
                            free(pName);
                        }
                    }
                    else
                    { // no group
                        if (FNFound && !FNOptional)
                        {
                            (*obj)->fn->name = malloc(sizeof(char) * strlen(pName) + 1);
                            strcpy((*obj)->fn->name, pName);
                            (*obj)->fn->group = malloc(2);
                            strcpy((*obj)->fn->group, "");
                            free(pName);
                        }
                        else
                        {
                            newProp->name = malloc(sizeof(char) * strlen(pName) + 1);
                            strcpy(newProp->name, pName);
                            newProp->group = malloc(2);
                            strcpy(newProp->group, ""); // so that it can be empty
                            free(pName);
                        }
                    }

                    // get the value or values
                    memmove(str, str + len + 1, strlen(str + len + 1) + 1); // this is enough if theres only one value
                    // TEST
                    // printf("\n%s", str);

                    if (strlen(str) == 0)
                    {
                        deleteCard((*obj)); // invalid card text format
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        // free(propName);
                        // free(pName);
                        if (FNFound && FNOptional)
                        {
                            deleteProperty(newProp);
                        }
                        return INV_PROP;
                    }

                    // getting the values if theres more than one
                    if (propName != NULL)
                    {
                        char *semiCol = strtok(str, ";");
                        // traverse and get the values
                        while (semiCol != NULL)
                        {
                            char *insVal;
                            if (strcmp(semiCol, " ") == 0)
                            {
                                // printf("space");
                                insVal = malloc(sizeof(char) * (2));
                                strcpy(insVal, "");
                            }
                            else
                            {
                                insVal = malloc(sizeof(char) * strlen(semiCol) + 1);
                                strcpy(insVal, semiCol);
                            }
                            // insert to list
                            if (FNFound && !FNOptional)
                            {
                                insertBack((*obj)->fn->values, insVal);
                            }
                            else
                            {
                                insertBack(newProp->values, insVal);
                            }
                            semiCol = strtok(NULL, ";");
                        }
                    }
                    else
                    { // only one value
                        // make copy
                        char *singleVal = malloc(sizeof(char) * strlen(str) + 2);
                        strcpy(singleVal, str);
                        if (FNFound && !FNOptional)
                        {
                            insertBack((*obj)->fn->values, singleVal);
                        }
                        else
                        {
                            insertBack(newProp->values, singleVal);
                        }
                    }

                    // insert struct to LL
                    if (FNFound && FNOptional)
                    {
                        insertBack((*obj)->optionalProperties, newProp);
                    }
                }
                else if (propName < valStart)
                { // has both parameters and values
                    // get the property(s) and name
                    // first get the section before values
                    int len = (valStart - str);
                    char *nameParam = malloc(sizeof(char) * len + 1);
                    int nameLen = (propName - str); // get the name, possibly group, and params

                    // getting just the name and possibly group
                    char *pName = malloc(sizeof(char) * nameLen + 1);
                    strncpy(nameParam, str + nameLen + 1, len - nameLen - 1); // copy
                    nameParam[len - nameLen - 1] = '\0';
                    strncpy(pName, str, nameLen); // copy
                    pName[nameLen] = '\0';

                    // if there's no name it violates the vcard specs
                    if (strlen(pName) == 0)
                    {
                        deleteCard((*obj)); // invalid card text format
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        free(newProp);
                        return INV_PROP;
                    }

                    // parse for fn into fn property in the struct
                    char *upperName = malloc(sizeof(char) * strlen(pName) + 1);
                    strcpy(upperName, pName);
                    for (int i = 0; i < strlen(upperName); i++)
                    {
                        upperName[i] = toupper(upperName[i]);
                    }
                    if (strstr(upperName, "FN") != NULL && !FNFound)
                    {
                        FNFound = true;
                        free(newProp);
                    }
                    else
                    {
                        FNOptional = true;
                        newProp->values = initializeList(&valueToString, &deleteValue, &compareValues);
                        newProp->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                    }
                    free(upperName);

                    // check if theres a group
                    char *isGroup = strstr(str, ".");
                    // this means there is a group as the period is before the seperator
                    if (isGroup != NULL && isGroup < valStart)
                    {
                        int groupLen = isGroup - str;
                        char *groupStr = malloc(sizeof(char) * groupLen + 1);
                        strncpy(groupStr, pName, groupLen);
                        groupStr[groupLen] = '\0';
                        memmove(pName, pName + groupLen + 1, strlen(pName + groupLen + 1) + 1); // copies the name after group
                        // insert group and name into struct
                        if (FNFound && !FNOptional)
                        {
                            (*obj)->fn->name = malloc(sizeof(char) * strlen(pName) + 1);
                            (*obj)->fn->group = malloc(sizeof(char) * strlen(groupStr) + 1);
                            strcpy((*obj)->fn->group, groupStr);
                            strcpy((*obj)->fn->name, pName);
                            free(groupStr);
                            free(pName);
                        }
                        else
                        {
                            newProp->name = malloc(sizeof(char) * strlen(pName) + 1);
                            newProp->group = malloc(sizeof(char) * strlen(groupStr) + 1);
                            strcpy(newProp->group, groupStr);
                            strcpy(newProp->name, pName);
                            free(groupStr);
                            free(pName);
                        }
                    }
                    else
                    { // no group
                        if (FNFound && !FNOptional)
                        {
                            (*obj)->fn->name = malloc(sizeof(char) * strlen(pName) + 1);
                            strcpy((*obj)->fn->name, pName);
                            (*obj)->fn->group = malloc(2);
                            strcpy((*obj)->fn->group, ""); // so that it can be empty
                            free(pName);
                        }
                        else
                        {
                            newProp->name = malloc(sizeof(char) * strlen(pName) + 1);
                            strcpy(newProp->name, pName);
                            newProp->group = malloc(2);
                            strcpy(newProp->group, ""); // so that it can be empty
                            free(pName);
                        }
                    }

                    // parsing parameters
                    char *paramSemiCol = strtok(nameParam, ";"); // token for delimiters
                    while (paramSemiCol != NULL)
                    {
                        char *seperate = strstr(paramSemiCol, "=");

                        // violates vcard spec for param
                        if (seperate == NULL)
                        {
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            if (FNFound && FNOptional)
                            {
                                deleteProperty(newProp);
                            }
                            // free(pName);
                            return INV_PROP;
                        }
                        // finding the param name
                        int paramLabelLen = seperate - paramSemiCol;
                        char *paramLabel = malloc(sizeof(char) * paramLabelLen + 1);
                        strncpy(paramLabel, paramSemiCol, paramLabelLen);
                        paramLabel[paramLabelLen] = '\0';

                        // finding the param value
                        char *paramValCpy = seperate + 1;
                        int paramValLen = strlen(paramValCpy);
                        char *paramVal = malloc(sizeof(char) * paramValLen + 1);
                        strcpy(paramVal, paramValCpy);
                        paramVal[paramValLen] = '\0';

                        paramSemiCol = strtok(NULL, ";");

                        // violates the vcard spec for parameters
                        if (strlen(paramVal) == 0 || strlen(paramLabel) == 0)
                        {
                            deleteCard((*obj)); // invalid card text format
                            free(str);
                            free(version);
                            fclose(fp);
                            fclose(unfold);
                            free(nameParam);
                            if (FNFound && FNOptional)
                            {
                                deleteProperty(newProp);
                            }
                            free(paramLabel);
                            free(paramVal);
                            // free(pName);
                            return INV_PROP;
                        }

                        // adding to struct LL for param
                        // create param struct
                        Parameter *newParam = malloc(sizeof(Parameter));
                        newParam->name = malloc(sizeof(char) * strlen(paramLabel) + 1);
                        newParam->value = malloc(sizeof(char) * strlen(paramVal) + 1);
                        strcpy(newParam->name, paramLabel);
                        strcpy(newParam->value, paramVal);
                        free(paramLabel);
                        free(paramVal);
                        //  adding the struct to LL
                        if (FNFound && !FNOptional)
                        {
                            insertBack((*obj)->fn->parameters, newParam);
                        }
                        else
                        {
                            insertBack(newProp->parameters, newParam);
                        }
                    }

                    // getting the values remainder
                    memmove(str, str + len + 1, strlen(str + len + 1) + 1);
                    // TEST
                    // printf("\n%s", str);

                    if (strlen(str) == 0)
                    {
                        free(str);
                        free(version);
                        fclose(fp);
                        fclose(unfold);
                        free(nameParam);
                        if (FNFound && FNOptional)
                        {
                            deleteProperty(newProp);
                        }
                        deleteCard((*obj)); // invalid card text format
                        return INV_PROP;
                    }

                    // getting the values if theres more than one
                    if (propName != NULL)
                    {
                        char *semiCol = strtok(str, ";");
                        // traverse and get the values
                        while (semiCol != NULL)
                        {
                            char *insVal = malloc(sizeof(char) * strlen(semiCol) + 1);
                            strcpy(insVal, semiCol);
                            // insert to list
                            if (FNFound && !FNOptional)
                            {
                                insertBack((*obj)->fn->values, insVal);
                            }
                            else
                            {
                                insertBack(newProp->values, insVal);
                            }
                            semiCol = strtok(NULL, ";");
                        }
                    }
                    else
                    { // only one value
                        // make copy
                        char *singleVal = malloc(sizeof(char) * strlen(str) + 2);
                        strcpy(singleVal, str);
                        if (FNFound && !FNOptional)
                        {
                            insertBack((*obj)->fn->values, singleVal);
                        }
                        else
                        {
                            insertBack(newProp->values, singleVal);
                        }
                    }

                    // insert struct into optional LL
                    if (FNFound && FNOptional)
                    {
                        insertBack((*obj)->optionalProperties, newProp);
                    }
                    free(nameParam);
                    // free(pName);
                }
            }
            // free(temp);
        }
        // FNOptional = false; //reset
        count++; // increment to track
        free(str);
        // free(strdupe);
    }
    // free pointers
    free(version);
    fclose(fp);
    fclose(unfold);
    return OK;
}

void deleteCard(Card *obj)
{
    if (obj == NULL)
    {
        return;
    }
    // start accessing each
    Property *fnFree = obj->fn;
    List *propFree = obj->optionalProperties;
    DateTime *bdayFree = obj->birthday;
    DateTime *anniFree = obj->anniversary;

    // start freeing within each subcomponent of card
    // first free the values for FN component
    if (fnFree != NULL)
    {
        deleteProperty(fnFree);
    }

    // freeing optional properties list
    if (propFree != NULL)
    {
        freeList(propFree);
    }

    // freeing bday
    if (bdayFree != NULL)
    {
        deleteDate(bdayFree);
    }
    if (anniFree != NULL)
    {
        deleteDate(anniFree);
    }

    free(obj);
}

char *cardToString(const Card *obj)
{
    if (obj == NULL)
    {
        return "NULL";
    }
    char *strprop;
    char *stroption;
    Property *fnStr = (Property *)obj->fn;
    if (fnStr != NULL)
    {
        // left off here
        strprop = propertyToString(fnStr);
    }
    List *optional = (List *)obj->optionalProperties;
    if (optional != NULL)
    {
        stroption = toString(optional);
    }

    int len = strlen(strprop) + strlen(stroption) + 100;
    char *str = malloc(sizeof(char) * len);
    sprintf(str, "%s\n%s", strprop, stroption);
    free(strprop);
    free(stroption);

    return (str);
    // return ("hello World");
}

char *errorToString(VCardErrorCode err)
{
    switch (err)
    {
    case OK:
        return "VCard parse sucessfully";
    case INV_FILE:
        return "There's a problem with the file";
    case INV_CARD:
        return "VCard file is missing required specs";
    case INV_PROP:
        return "A line being read is missing specs for parsing";
    case OTHER_ERROR:
        return "System error occured while parsing";
    case INV_DT:
        return "Invalid date or time formatting";
    case WRITE_ERROR:
        return "There's a problem writing to the file";
    default:
        return "Invalid error code";
    }
}

VCardErrorCode writeCard(const char *fileName, const Card *obj)
{
    // object validity, never assume its not null
    if (obj == NULL)
    {
        return WRITE_ERROR;
    }
    // file name validation
    if (fileName == NULL)
    {
        // deleteCard(obj); //free object otherwise
        return WRITE_ERROR;
    }

    if (strstr(fileName, ".vcf") == NULL && strstr(fileName, ".vcard") == NULL)
    {
        // deleteCard(obj); //free object otherwise
        return WRITE_ERROR;
    }

    // start file writing
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL)
    {
        return WRITE_ERROR;
    }

    // write the standard components of the file format
    fprintf(fp, "BEGIN:VCARD\r\nVERSION:4.0\r\n");

    // start traversing the struct and creating formatted strings for file writing

    // look into the property FN
    char *propStr = propertyWrite(obj->fn);
    if (propStr == NULL)
    {
        fclose(fp);
        return WRITE_ERROR;
    }
    // } else if(strcmp(propStr, '-1') == 0){ //invalid str
    //     free(propStr);
    //     return WRITE_ERROR;
    // }

    fprintf(fp, "%s\r\n", propStr);
    free(propStr);

    // iterate and repeat this code for the list of optional props
    ListIterator iter = createIterator(obj->optionalProperties);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL)
    {
        Property *prop = (Property *)elem;
        propStr = propertyWrite(prop);
        if (propStr == NULL)
        {
            fclose(fp);
            return WRITE_ERROR;
        }
        fprintf(fp, "%s\r\n", propStr);
        free(propStr);
    }

    // same with date and time
    bool dateType = true;
    // start with bday first
    if (obj->birthday != NULL)
    {
        char *dateStr = dateTimeWrite(obj->birthday, dateType);
        if (dateStr == NULL)
        {
            fclose(fp);
            return WRITE_ERROR;
        }
        fprintf(fp, "%s\r\n", dateStr);
        free(dateStr);
    }

    // start with and anniversary first
    if (obj->anniversary != NULL)
    {
        dateType = false;
        char *dateStr = dateTimeWrite(obj->anniversary, dateType);
        if (dateStr == NULL)
        {
            fclose(fp);
            return WRITE_ERROR;
        }
        fprintf(fp, "%s\r\n", dateStr);
        free(dateStr);
    }
    // writing the standard final components of the file
    fprintf(fp, "END:VCARD\r\n");
    // fprintf(fp, "\n"); //this is possible, unsure of the format right now

    // final code for the function
    fclose(fp);
    return OK;
}

VCardErrorCode validateCard(const Card *obj)
{
    if(obj == NULL){
        return INV_CARD;
    }
    //cardinality of one in the list
    int kind = 0, n = 0, gender= 0, prodid= 0, rev= 0, uid= 0;

    // check the object is valid
    Card * temp = (Card *) obj;

    //check for fn prop
    if(temp->fn == NULL){
        return INV_CARD;
    }
    //check for optional props
    if(temp->optionalProperties == NULL){
        return INV_CARD;
    }

    //validate fn
    if(strcmp(temp->fn->name, "FN") != 0){
        return INV_CARD;
    }
    propValidate(temp->fn, "FN");

    //start looping list to validate
    ListIterator iter = createIterator(temp->optionalProperties);
    void *elem;
    while ((elem = nextElement(&iter)) != NULL)
    {
        Property *prop = (Property *)elem;
        char * propName = prop-> name;

        //checking name length
        if(strlen(propName) == 0){
            return INV_CARD;
        }

        //checking none of these attributes are null
        if(prop == NULL || prop ->name == NULL||prop->group == NULL||prop->parameters == NULL ||prop->values == NULL){
            return INV_CARD;
        }

        //checking for those with a cardinality of one
        if(strcmp(propName, "KIND") == 0){
            if(kind == 1){
                return INV_PROP;
            }
            kind++;

        } else if(strcmp(propName, "N") == 0){
            if(n == 1){
                return INV_PROP;
            }
            n++;

        }else if(strcmp(propName, "GENDER") == 0){
            if(gender == 1){
                return INV_PROP;
            }
            gender++;
        }else if(strcmp(propName, "PRODID") == 0){
            if(prodid == 1){
                return INV_PROP;
            }
            prodid++;
        }else if(strcmp(propName, "REV") == 0){
            if(rev == 1){
                return INV_PROP;
            }
            rev++;
        }else if(strcmp(propName, "UID") == 0){
            if(uid == 1){
                return INV_PROP;
            }
            uid++;
        }else if(strcmp(propName, "BDAY") == 0){
            return INV_DT;
        }else if(strcmp(propName, "ANNIVERSARY") == 0){
            return INV_DT;
        }if(strcmp(propName, "VERSION") == 0){
            return INV_CARD;
        }

        //call to validate property
        bool validation = propValidate(prop, propName);

        //if validation is wrong
        if(validation == false){
            return INV_PROP;
        }
    }

    //validate DT for bday and anniversary
    if(temp->birthday != NULL){
        bool bday = dtValidate(temp->birthday);
        if(bday == false){
            return INV_DT;
        }
    }

    if(temp->anniversary != NULL){
        bool anni = dtValidate(temp->anniversary);
        if(anni == false){
            return INV_DT;
        }
    }
    return OK;
}