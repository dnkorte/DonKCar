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
#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>
#include "config.h"

void util_disable_all_spi();
void util_enable_my_spi(int pin_number);

void set_status(String newStatus, int color);
void indicateActivityForStatus();

int readAnalog5xAveraged(int pin);

/*
 * test if String is numeric
 * from: https://forum.processing.org/two/discussion/23275/determine-if-a-string-is-numeric-only-int-not-float-but-minus-sign-allowed.html
 * call: bool status = isNumeric("-1234");
 * 
 * if it is proper numeric, can convert to int by   myIntVar = myStringVar.toInt();
 */
boolean isNumeric(String testWord);

/*
 * test if String is reasonable float value
 * from: https://forum.pjrc.com/threads/34705-Arduino-String-How-to-identify-valid-float
 * call: bool status = isNumeric("-1234");
 * 
 * if it is reasonable float, can convert to float by   myFloatVar = myStringVar.toFloat();
 */
boolean isFloat(String testString);

#endif   // UTIL_H
