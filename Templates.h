const char mainPage[] = "<!DOCTYPE html>\
    <html>\
    <head>\
        <meta charset=\"utf-8\">\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">\
        <title>Web Control</title>\
    </head>\
    <body onkeypress=\"\">\
        <h1>Web Control</h1>\
        <h2>Movement: <span id=\"movement\">Move the Bot</span></h2>\
        <h2>Status: <span id=\"status\"></span></h2>\
        <div>\
            <button onpointerdown=\"moveForward();\"  onpointerup=\"stopMove();\">Forward</button>\
            <button onpointerdown=\"moveBackward();\" onpointerup=\"stopMove();\">Backward</button>\
            <button onpointerdown=\"turnLeft();\" onpointerup=\"stopMove();\">Left</button>\
            <button onpointerdown=\"turnRight();\" onpointerup=\"stopMove();\">Right</button>\
            <button onpointerdown=\"stopMove();\">Stop</button>\
        </div>\
        <script>\
            ws = new WebSocket('ws://' + location.hostname + ':81');\
            botMovement = document.getElementById('movement');\
            botStatus = document.getElementById('status');\
            botStatus.style = 'color: lime';\
            botStatus.textContent = 'No Obstacles';\
            function moveForward() {\
                ws.send('forward');\
                botMovement.style = 'color: black';\
                botMovement.textContent = 'Forward';\
            }\
            function moveBackward() {\
                ws.send('backward');\
                botMovement.style = 'color: black';\
                botMovement.textContent = 'Backward';\
            }\
            function turnLeft() {\
                ws.send('left');\
                botMovement.style = 'color: black';\
                botMovement.textContent = 'Left Turn';\
            }\
            function turnRight() {\
                ws.send('right');\
                botMovement.style = 'color: black';\
                botMovement.textContent = 'Right Turn';\
            }\
            function stopMove() {\
                ws.send('stop');\
                botMovement.style = 'color: red';\
                botMovement.textContent = 'Stop';\
            }\
            document.onkeyup = stopMove;\
            document.onkeydown = navBot;\
            function navBot(e) {\
                e = e || window.event;\
                if (e.keyCode == '38')\
                    moveForward();\
                else if (e.keyCode == '40')\
                    moveBackward();\
                else if (e.keyCode == '37')\
                    turnLeft();\
                else if (e.keyCode == '39')\
                    turnRight();\
                else if (e.keyCode == '32')\
                    stopMove();\
            }\
            ws.onmessage = function (e) {\
                switch(e.data) {\
                    case 'frontObstacle':\
                        botStatus.style = 'color: red';\
                        botStatus.textContent = 'Obstacle on the Front Detected';\
                        break;\
                    default:\
                        botStatus.style = 'color: lime';\
                        botStatus.textContent = 'No Obstacles';\
                }\
            };\
        </script>\
    </body>\
</html>";
