# VCard Parser Library

## What is it?
VCards are a standardized file format for contacts. I've developed a library to parse through the vcard and store it in a struct containing multiple linked lists for the specs. 
The format is follows this documentation https://datatracker.ietf.org/doc/html/rfc6350. There are many applications that use VCard files and this library is an API that handles the parsing 
so that you can utilize VCards in your application. Note that to view a VCard, a normal click on the file will direct the file to a contact id in your computers contacts application. 
Thus, to access the file format, view it in a text editor or IDE. I've also listed a sample VCard file below.

## How to use the library
Access the folder and within the command line type "make" or "make parser", you will then see a library "libvcparser.so". Insert this shared library into the file that you need the library 
for. To parse the file, a good example would be my ```mainTst.c``` file. You must create a pointer of the struct and pass it onto ```createCard("testCardMin.vcf", &test);``` along with the 
file name you would like to parse. Note that since this is an API, you'll have enum values to communicate your API requests. The enum values are 
```typedef enum ers {OK, INV_FILE, INV_CARD, INV_PROP, INV_DT, WRITE_ERROR, OTHER_ERROR } VCardErrorCode;```. Take a look into the header files to find out what functions could be performed 
with the VCards. To safely manage memory, be sure to call ```void deleteCard(Card* obj);``` if the VCard is sucessfully parsed.

## Sample VCard file
```BEGIN:VCARD
VERSION:4.0
FN:Simon Perreault
N:Perreault;Simon;;;ing. jr,M.Sc.
GENDER:M
LANG;PREF=1:fr
LANG;PREF=2:en
ORG;TYPE=work:Viagenie
ADR;TYPE=work:;Suite D2-630;2875 Laurier;Quebec;QC;G1V 2M2;Canada
TEL;VALUE=uri;TYPE="work,voice";PREF=1:tel:+1-418-656-9254;ext=102
TEL;VALUE=uri;TYPE="work,cell,voice,video,text":tel:+1-418-262-6501
EMAIL;TYPE=work:simon.perreault@viagenie.ca
GEO;TYPE=work:geo:46.772673,-71.282945
KEY;TYPE=work;VALUE=uri:http://www.viagenie.ca/simon.perreault/simon.asc
TZ:-0500
URL;TYPE=home:http://nomis80.org
END:VCARD```
