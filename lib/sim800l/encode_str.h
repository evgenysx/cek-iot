#ifndef _CEK_ENCODE_STR_H_
#define _CEK_ENCODE_STR_H_

#include <Arduino.h>

//
// https://codius.ru/articles/GSM_модуль_SIM800L_часть_3

unsigned int getCharSize(unsigned char b);
unsigned int symbolToUInt(const String& bytes);
String byteToHexString(byte i);

unsigned char HexSymbolToChar(char c);

String UCS2ToString(String s);


// =================================== Блок кодирования строки в представление UCS2 =================================
String StringToUCS2(String s);

unsigned int getCharSize(unsigned char b);

unsigned int symbolToUInt(const String& bytes);

String byteToHexString(byte i);


String revertBytes(const String& bytes);

#endif