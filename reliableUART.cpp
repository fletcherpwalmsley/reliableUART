//#include "Arduino.h"
#include "reliableUART.h"
#include "RUV.h"

#include "../../../BoatControl2/srcPC/Boost-Serial-Port-Lib/Boost-Serial-Port/BoostSerial.h"

#include <stdio.h>


// MSVC doesn't like sscanf being used. At will throw an error as it considers it deprecated
// But, Arduino doesn't have sscanf_s. So sscanf is still used to keep things as close as possible
#pragma warning(disable : 4996)


BoostSerial Serial;


void(*mH[MESSAGE_HANDLER_ARRAY_SIZE]) (char* inData);

int findHandlerIndex(char* inStr) {
   
    int handlerIndex = -1;
    int i = 0;
    // Find opening delimeter "["
    while (int(inStr[i]) != 91) // ASCII [ == 91
    {
        i++;
    }
    i++;

    // The first field is the handler index

    sscanf(inStr + i, "%d", &handlerIndex);

    return handlerIndex;
}

bool sumAndLengthCheck(int& handlerIndex, int& dataLength, int& givenCheckSum, char* dataStr, int& dataIndexStart, int& dataIndexEnd){
    // Make sure all writen to ints aren't still set as their default value
    if (handlerIndex == -1 ||
        dataLength == -1 ||
        givenCheckSum == -1 ||
        dataIndexStart == -1 ||
        dataIndexEnd == -1) {
        return false;
    }

    // Make sure the dataLenght variable aligns to what we got
    if (dataLength != (dataIndexEnd - dataIndexStart + 1))
        return false;

    // Finally, calulate a local checksum and compare to the given value
    int localCheckSum = handlerIndex + dataLength;

    for (int i = dataIndexStart; i <= dataIndexEnd; i++) {
        localCheckSum += int(dataStr[i]);
    }

    if (localCheckSum != givenCheckSum)
        return false;

    return true;
}

bool genHeaderPro(char* inData, int HI, int& handlerIndex, int& dataLength, int& i){
    // Find opening delimeter "["
    while (int(inData[i]) != 91) // ASCII [ == 91
    {
        i++;
    }
    i++;

    // The first field is the handler index
    sscanf(inData + i, "%d", &handlerIndex);
   
    if (handlerIndex != HI)
    {
        Serial.println("Handler index miss-match, aborting processing");
        return false;
    }

    // Find first ","
    while (int(inData[i]) != 44) // ASCII , == 44
    {
        i++;
    }
    i++;

    // The second field is the data length
    sscanf(inData + i, "%d", &dataLength);

    // Find next ",'
    while (int(inData[i]) != 44) // ASCII , == 44
    {
        i++;
    }
    i++;

    return true;
}

bool genSerialIntPro(char* inData, int HI, int &intVal){
    int handlerIndex = -1;
    int dataLength = -1;
    int givenCheckSum = -1;
    int dataIndexStart = -1;
    int dataIndexEnd = -1;
    int i = 0;

    if (!genHeaderPro(inData, HI, handlerIndex, dataLength, i)) {
        return false;
    }

    dataIndexStart = i; // Need to know this index for checksum calculation

    // The third field is the our data
    sscanf(inData + i, "%d", &intVal);

    // Find next ",'
    while (int(inData[i]) != 44) // ASCII , == 44
    {
        i++;
    }
    dataIndexEnd = i - 1;
    i++;

    // Final field is the checksum
    sscanf(inData + i, "%d", &givenCheckSum);

    // Finally, run checks to validate the data
    return sumAndLengthCheck(handlerIndex, dataLength, givenCheckSum, inData, dataIndexStart, dataIndexEnd);
}

void addToInt(char* inData){
    int tempInt;
    if (genSerialIntPro(inData, 0, tempInt)){
      globalRUV.testInt = tempInt;
    }
    else{
      Serial.println("inData processing error, data rejected");
    }
}

void processBuffer(char* inData){
    // Main buffer processing
    int handlerIndex = findHandlerIndex(inData);
    if ((mH[handlerIndex] == NULL) || (handlerIndex > MESSAGE_HANDLER_ARRAY_SIZE))
    {
        Serial.println("No message handlers exist for this message identifer");
    }
    else
    {
        (*mH[handlerIndex]) (inData);
    }
}

void createVaildMessageHandlers(){
    mH[0] = addToInt;
}



