#include <pgmspace.h>

const char config_html[] PROGMEM = R"=====(

<table>
   <tr>
      <td>
         <h2>DEVICE CONFIG FILE</h2>
         <form id="configForm">
            <label for="configText">CONFIG FILE Content:</label><br>
            <textarea id="configText" name="configText" rows="15" cols="55">__config__</textarea><br>
            <input type="submit" value="SAVE">
            <button type="button" onclick="rebootDevice()">REBOOT</button>
      <!--  <button type="button" onclick="DeepSleep()">DeepSleep()</button>  -->
         </form>
         <script>
            function rebootDevice() {
                fetch('/reboot', {
                    method: 'GET' // You might need to adjust the method depending on your server configuration
                })
                .then(response => {
                    console.log('Reboot request sent');
                    // Optionally, you can handle the response here
                })
                .catch(error => {
                    console.error('Error sending reboot request:', error);
                });
            }
         </script>
      </td>
   </tr>
   <tr>
      <td>
         <small>
            <b>CONFIG TEMPLATE</b><br>
               HOSTNAME=ML-Z004<br>
               MDNS_ENABLED=TRUE<br> 
               SERIAL_PRINT=TRUE<br>
               -SERIAL_PRINT=FALSE<br>
               WIFIMODE=MIXED<br>
               -WIFIMODE=AP<br>
               SHOWFILE_INSTEAD_OF_DOWNLOAD=TRUE<br>
               -SHOWFILE_INSTEAD_OF_DOWNLOAD=FALSE<br>
               WIFI_SSID_STA=home<br>
               WIFI_PASS_STA_ENCODED=fG5pbWRhfA==<br>
               SAVE_INDEX=2<br>
         </small>
      </td>
   </tr>
</table>
<script>
   document.getElementById('configText').value = '__config__';
</script>

)=====";