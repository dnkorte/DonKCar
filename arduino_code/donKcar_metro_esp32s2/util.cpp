/*
 * Summary: openMV + esp32 based autonomous racer
 * 
 * Author(s):  Don Korte
 * Repository: https://github.com/dnkorte/DonKCar
 *
 * MIT License
 * Copyright (c) 2020 Don Korte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. * 
 */
#include "util.h"

/* 
 *  Note these functions currently bracket all calls to SD card and TFT driver
 *  but it turns out the actual hardware drivers manage the CS (chip select) lines
 *  ok by themselves and these functions are not needed.
 *  
 *  left here as empty stubs though just in case i later add a different SPI
 *  device that does not have in-built CS line management
 */

void util_disable_all_spi() {  
    //digitalWrite(PIN_TFT_CS, HIGH);    
    //digitalWrite(PIN_SD_CS, HIGH);
    
}

void util_enable_my_spi(int pin_number) {
  //util_disable_all_spi();  
  //digitalWrite(pin_number, LOW);
}

/*
 * test if String is numeric
 * from: https://forum.processing.org/two/discussion/23275/determine-if-a-string-is-numeric-only-int-not-float-but-minus-sign-allowed.html
 * call: bool status = isNumeric("-1234");
 * 
 * if it is proper numeric, can convert to int by   myIntVar = myStringVar.toInt();
 */
boolean isNumeric(String testWord) {
  // for int numbers, not float  
  String acceptableChars = "0123456789-";
  String numsNoSign = "0123456789";

  // first we check if a wrong char is present 
  for (int i=0; i<testWord.length(); i++) {
    if (acceptableChars.indexOf(testWord.charAt(i))<0) {
      return false;  // abort with false
    }//if
  }//for

  // second: to avoid that testWord might be "---"
  // we need to find one number at last 
  boolean foundOne = false; 
  for (int i=0; i<testWord.length(); i++) {
    if (numsNoSign.indexOf(testWord.charAt(i))>=0) {
      foundOne = true;
    }//if
  }//for

  if (!foundOne) 
    return false; // abort with false

  // do we have a minus?
  if (testWord.indexOf("-") >= 0) {
    // only one minus allowed 
    int countMinus=0;
    for (int i=0; i<testWord.length(); i++) 
      if (testWord.charAt(i)=='-')
        countMinus++;
    if  (countMinus>1) 
      return false;
    // -------------------
    if (testWord.indexOf('-')!=0) // - must be first char like in -12 
      return false;  // abort with false
  }
  return true; // success 
} //func

/*
 * test if String is reasonable float value
 * from: https://forum.pjrc.com/threads/34705-Arduino-String-How-to-identify-valid-float
 * call: bool status = isNumeric("-1234");
 * 
 * if it is reasonable float, can convert to float by   myFloatVar = myStringVar.toFloat();
 */
 boolean isFloat(String testString) {
    int signCount = 0;
    int decPointCount = 0;
    int digitCount = 0;
    int otherCount = 0;
    for (int j=0; j<testString.length(); j++) {
      char ch = testString.charAt(j);
      if ( j==0 && ( ch=='-' || ch=='+' ) ) signCount++; //leading sign
      else if ( ch>='0' && ch<='9' ) digitCount++; //digit
      else if ( ch=='.' ) decPointCount++; //decimal point
      else otherCount++; //that's bad :(
    }
    if (signCount>1 || decPointCount>1 || digitCount<1 || otherCount>0) {
      return false;
    }
    else {
      return true;
    }
 }
 
/*
 * test if String is reasonable integer value
 * modified from isFloat() (above)
 * from: https://forum.pjrc.com/threads/34705-Arduino-String-How-to-identify-valid-float
 * call: bool status = isNumeric("-1234");
 * 
 * if it is reasonable float, can convert to float by   myFloatVar = myStringVar.toFloat();
 */
 boolean isInteger(String testString) {
    int signCount = 0;
    int decPointCount = 0;
    int digitCount = 0;
    int otherCount = 0;
    for (int j=0; j<testString.length(); j++) {
      char ch = testString.charAt(j);
      if ( j==0 && ( ch=='-' || ch=='+' ) ) signCount++; //leading sign
      else if ( ch>='0' && ch<='9' ) digitCount++; //digit
      else if ( ch=='.' ) decPointCount++; //decimal point
      else otherCount++; //that's bad :(
    }
    if (signCount>1 || decPointCount>0 || digitCount<1 || otherCount>0) {
      return false;
    }
    else {
      return true;
    }
 }

 int readAnalog5xAveraged(int pin) {
  int sum_of_readings = 0;
  int average;
  
    for (int i=0; i<5; i++) {
      sum_of_readings += analogRead(pin);
    }
    average = sum_of_readings / 5;
    return average;
 }
    
