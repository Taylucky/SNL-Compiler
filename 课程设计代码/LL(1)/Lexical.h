#pragma once
//词法分析头文件

#include "globals.h" 

void DisplayToken();
void AddChain(int LineNumber, string type, string value);
bool FindReserved(string TarS);
void InputHandle();
void CharHandle(char &c);
void DigitHandle(char &c);
void ColonHandle(char &c);
void SingleDivideCHandle(char &c);
void AnnotationHandle(char &c);
void PeriodHandle(char &c);
void QuoteHandle(char &c);



