#include <pgmspace.h>

const char index_html_default[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
   

    <title>ESP DEVICE DEFAULT PAGE</title>
    <style>
        html {
            font-family: Helvetica;
            display:inline - block;
            margin:0px auto;
            text-align: center;
        }

        body {
            margin-top: 50px;
        }

        h1 {
            color: #444444;
            margin: 50px auto 30px;
        }

        h3 {
            color: #444444;
            margin-bottom: 50px;
        }

        
        p {
            font-size: 14px;
            color: #888;
            margin-bottom: 10px;
        }
    </style>
</head>

<body>
        <h1>ESP DEVICE WEB-SERVER</h1>
        <br>Default page<br>
        __log__
        <br>
        <br><small><small>Compiled on __cdate__ at __ctime__ </small></small>
        <br>

</body>
</html>

)=====";