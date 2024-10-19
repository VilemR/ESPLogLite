#include <pgmspace.h>

const char control_html[] PROGMEM = R"=====(

<table>
 <tr>
 <td> 
 <h2>Command Form</h2>
    <form action="/submit" method="post">
        <label for="commandInput">Enter Command:</label>
        <input type="text" id="commandInput" name="commandInput" required>
        <button type="submit">Submit</button>
   <!-- <button type="button" onclick="showConfirmation()">START LOGGING</button>  -->
    </form>

    <script>
        function showConfirmation() {
        var confirmation = confirm("Are you sure you want to start the data logging? 
                                    Ensure BATTERY is charged, SWITCH POSITION is in DS, 
                                    SENSING mode is ON and LOGGING PERIOD (__logper__) and 
                                    other CONFIG values are set accordingly.");
        if (confirmation) {
            fetch('/deepsleep', {
                    method: 'GET' // You might need to adjust the method depending on your server configuration
                })
                .then(response => {
                    console.log('Request sent');
                })
                .catch(error => {
                    console.error('Error sending reboot request:', error);
                });
        } else {
            alert("Submission canceled!");
        }
        }
    </script>
 
 </td>
 </tr>
 <tr>
 <td> 
 <small><small>
 
 __log__
 
 </small></small>
 </td>
 </tr>
</table>

)=====";