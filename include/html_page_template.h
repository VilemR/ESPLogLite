#include <pgmspace.h>

const char page_template_html[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<head>
     <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">

    <title>__PAGE_TITLE__</title>
    <style>
        html {
            -- font-family: Helvetica;
            display:inline - block;
            margin:0px auto;
            text-align: center;
        }

        body {
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            padding: 0;
            background-color: #f4f4f4;
        }

        h1 {
            color: #444444;
            margin: 50px auto 30px;
        }

        table {
            width:100%;
            border-collapse: collapse;
            border: 1px solid #dddddd;
            margin-top: 20px;
        }

        th, td {
            border: 1px solid #dddddd;
            text-align: left;
            padding: 8px;
        }

        h3 {
            color: #444444;
            margin-bottom: 50px;
        }
        .container {
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
        }
    </style>
</head>

<body>

<div class="container">
 
   __body__

</div>
     
</body>
</html>



)=====";