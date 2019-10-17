#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include "Templates.h"

// Websocket and WebServer
WebSocketsServer wsServer = WebSocketsServer(81);
ESP8266WebServer webServer(80);

// L298N Control Pins
const int motorLF = 14;
const int motorLB = 12;
const int motorRF = 13;
const int motorRB = 15;

// IR Sensor Pins
const int irFront = 5;
const int irRear = 4;
int frontObstacle;
int rearObstacle;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = wsServer.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        wsServer.sendTXT(num, "Connected");
    }
    break;
    case WStype_TEXT:
        Serial.printf("[%u] get Text: %s\n", num, payload);
        Serial.println((char *)payload);
        if (String((char *)payload) == "forward")
            if (frontObstacle)
            {
                wsServer.sendTXT(num, "good");
                moveForward();
            }
            else
            {
                wsServer.sendTXT(num, "frontObstacle");
                moveStop();
            }
        else if (String((char *)payload) == "backward")
            moveBackward();
        else if (String((char *)payload) == "left")
            turnLeft();
        else if (String((char *)payload) == "right")
            turnRight();
        else
            moveStop();
        break;
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println(WiFi.softAP("wifi-bot", "hackerm4n") ? "AP spawn" : "AP spawn failed");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    wsServer.begin();
    wsServer.onEvent(webSocketEvent);

    pinMode(motorLF, OUTPUT);
    pinMode(motorLB, OUTPUT);
    pinMode(motorRF, OUTPUT);
    pinMode(motorRB, OUTPUT);

    pinMode(irFront, INPUT);

    webServer.on("/", handleRoot);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
}

void loop()
{
    frontObstacle = digitalRead(irFront);
    wsServer.loop();
    webServer.handleClient();
    // moveForward();
    // delay(2000);
    // moveBackward();
    // delay(2000);
    // turnLeft();
    // delay(2000);
    // turnRight();
    // delay(2000);
    // moveStop();
    // delay(2000);
}

void moveStop()
{
    digitalWrite(motorRF, LOW);
    digitalWrite(motorRB, LOW);
    digitalWrite(motorLF, LOW);
    digitalWrite(motorLB, LOW);
}

void moveForward()
{
    digitalWrite(motorRF, HIGH);
    digitalWrite(motorRB, LOW);
    digitalWrite(motorLF, HIGH);
    digitalWrite(motorLB, LOW);
}

void moveBackward()
{
    digitalWrite(motorRF, LOW);
    digitalWrite(motorRB, HIGH);
    digitalWrite(motorLF, LOW);
    digitalWrite(motorLB, HIGH);
}

void turnRight()
{
    digitalWrite(motorRF, LOW);
    digitalWrite(motorRB, HIGH);
    digitalWrite(motorLF, HIGH);
    digitalWrite(motorLB, LOW);
}

void turnLeft()
{
    digitalWrite(motorRF, HIGH);
    digitalWrite(motorRB, LOW);
    digitalWrite(motorLF, LOW);
    digitalWrite(motorLB, HIGH);
}

// Web Server
void handleRoot()
{
    webServer.send(200, "text/html", mainPage);
}

void handleNotFound()
{
    webServer.send(404, "text/html", "<h1>Fuck Off!</h1>");
}