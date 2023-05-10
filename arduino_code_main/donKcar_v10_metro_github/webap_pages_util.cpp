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
#include "webap_pages_util.h"
#include "webap_core.h"

extern String pageBuf;

/*
 * this checks for pages that perform basic utility functions
 * 
 * IMPORTANT this function must always be LAST in the sequence
 * of build_XXX pages because it ALWAYS processes a page --
 * if it doesn't match a specific page from its list then it 
 * displays the default "index.html" page
 * 
 * if an appropriate page request is present in (header) then
 * it builds the page in pageBuf and returns true
 * otherwise it returns false and does not modify pageBuf
 */
 
void webap_build_util(String header) {
  char   tmpBuf[36];  // for simple numeric conversions

  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page getcfg.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /getcfg.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";  
      
    pageBuf = pageBuf + "<body>\n<h1>Configuration File</h1>\n";  
      pageBuf = pageBuf + "<div style='white-space: pre; text-align: left;'>\n";
        pageBuf = pageBuf + cfg_showfile();
      pageBuf = pageBuf + "</div>";
      
    pageBuf = pageBuf + show_menu();    
    pageBuf = pageBuf + webap_start_local_js();
      // note need the start and end local even if no local js needed (the start has constants needed for common)
      // and must have the following function, which may be expanded if page generates parameters
      pageBuf = pageBuf + "function local_api_process(paramid, myvalue) { alert('local process:' + paramid + '$' + myvalue); }\n";
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();
  
   
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page bye.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  } else if (header.indexOf("GET /bye.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Configurator Terminated</h1>\n";
    pageBuf = pageBuf + "<p>The web configuration process has ended.  Control by Nunchuk has been restored</p>\n";
    pageBuf = pageBuf + webap_end_page(); 
    webap_adjustWebEndRequest(true);  

    
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page index.html  (or actually any unknown page or "no page"
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  } else {
    // if no page name (ie index.html) then it displays this page by default
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";  
      pageBuf = pageBuf + "<body>\n<h1>Racer Configuration Menu</h1>\n";
    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();
      // note need the start and end local even if no local js needed (the start has constants needed for common)
      // and must have the following function, which may be expanded if page generates parameters
      pageBuf = pageBuf + "function local_api_process(paramid, myvalue) { alert('local process:' + paramid + '$' + myvalue); }\n";
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();
  }
}
  



    
/*
 * process commands that have been sent by a web button 
 * note this should not display anything to website
 * it returns a status code to caller though, which MIGHT display an alert or might use a returned value
 * 
 * returns  "OK":  if success; note calling page just continues with no acknowledgement
 *          "SUCCESS message":   calling page does alert showing this string
 *          "ERROR message":     calling page does alert showing this string
 *          "VALUE paramid value" this returns a value that calling page will process (no alert)
 *                                (note the value can be a string but if so no spaces allowed)
 *                                
 *          "NOMATCH"            didn't find any matching URLs                      
 */
String webap_process_API_util(String header) {    
  return "NOMATCH";
}


    
