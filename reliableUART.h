#ifndef UARTClass_h
#define UARTClass_h
//#include "Arduino.h" 
// class MyClass {
// public:
// 	MyClass(int pin);
// 	void myFunction(int blinkRate);
// private:
// 	int _pin;
// };
#define MESSAGE_HANDLER_ARRAY_SIZE 3


int findHandlerIndex(char* inStr);
void createVaildMessageHandlers();
void addToInt(char* inData);
bool genSerialIntPro(char* inData, int HI, int &intVal);
bool sumAndLengthCheck(int& handlerIndex, int& dataLength, int& givenCheckSum, char* dataStr, int& dataIndexStart, int& dataIndexEnd);
void processBuffer(char* inData);

#endif