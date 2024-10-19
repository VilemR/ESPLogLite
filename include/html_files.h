#include <pgmspace.h>

const char index_files[] PROGMEM = R"=====(

<table>
  <tr>
   <td> 
    <h2>SPIFFS FILES</h2>
   </td>
  </tr>

  <tr>
   <td> 
        <a href="/config" target="_blank">CONFIG</a>
        &nbsp;&nbsp;
        <a href="/control" target="_blank">CONTROL</a>
   </td>
  </tr>

 <tr>
  <td> 
    __files__
  </td>
 </tr>

<tr>
 <td> 
   <small><small><b>Current log file : </b> __logfile__ </small></small><br>
   <small><small><b>Storage capacity : </b> __footer__ <br> __count__ files</small></small>
   <small><small> / runtime : __runtime__ sec(s).</small></small><br>
  </td>
 </tr>


</table>


)=====";