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
#ifndef WEBAP_CORE_H
#define WEBAP_CORE_H

/*
 * **********************************************************
 * **********************************************************
 * NOTE this is totally unused as of 20230318 bug is
 * kept for archival purposes.  portions of it will later 
 * be modified and used for a web handler for configuration
 * menus
 * **********************************************************
 * **********************************************************
 */

#include <Arduino.h>
#include "config.h"

void webap_init(void);
void webap_deinit(int reason);
void webap_process(void);
bool webap_getWebMode();
bool webap_getWebEndRequest();
void webap_adjustWebEndRequest(bool newValue);

String webap_start_page(void);
String webap_end_page(void);
String webap_start_local_js(void);
String webap_end_local_js(void);
String webap_commonJS(void);

String show_menu(void);
String show_mycss(void);

#endif  // WEBAP_CORE_H
