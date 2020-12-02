#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsServer.h>
#include <NewPing.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include "Templates.h"

// Web Config
String serverURL = "http://10.10.79.150/send_data";
String botName = "xbot";
String botKey = "395686";

// Websocket, WebServer and HTTP
WebSocketsServer wsServer = WebSocketsServer(81);
ESP8266WebServer webServer(80);
HTTPClient httpClient;
WiFiClient wifiClient;

// NTP Config
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 28800;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

// L298N Control Pins
const int motorLF = 14;
const int motorLB = 12;
const int motorRF = 13;
const int motorRB = 15;

// Ultrasonic Sensor and Pins
const int trigPin = 4;
const int echoPin = 5;
int obstacleDist;
NewPing ultraSonic = NewPing(trigPin, echoPin);

// Air Quality Sensor Pins
const int airQualityPin = A0;
int airQualityReading;

// Interval
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;

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
        obstacleDist = ultraSonic.ping_cm();
        if (String((char *)payload) == "forward")
        {
            if (obstacleDist > 0 && obstacleDist < 60)
            {
                wsServer.sendTXT(num, "frontObstacle");
                moveStop();
            }
            else
            {
                wsServer.sendTXT(num, "good");
                moveForward();
            }
        }
        else if (String((char *)payload) == "backward")
            moveBackward();
        else if (String((char *)payload) == "left")
            turnLeft();
        else if (String((char *)payload) == "right")
            turnRight();
        else if (String((char *)payload) == "readAirQuality")
        {
            airQualityReading = analogRead(airQualityPin);
            wsServer.sendTXT(num, (char *)airQualityReading);
        }
        else
            moveStop();
        break;
    }
}

void setup()
{
    Serial.begin(9600);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_AP_STA);
    WiFi.hostname("harriebot");
    Serial.println(WiFi.softAP("wifi-bot", "hackerm4n") ? "AP spawn" : "AP spawn failed");
    Serial.println(WiFi.begin("wifi22", "justdoit123") ? "STA spawn" : "STA spawn failed");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    Serial.print("STA IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("STA Gateway: ");
    Serial.println(WiFi.gatewayIP());

    timeClient.begin();
    timeClient.update();
    startMillis = millis();

    Serial.println(timeClient.getDay());

    wsServer.begin();
    wsServer.onEvent(webSocketEvent);

    pinMode(motorLF, OUTPUT);
    pinMode(motorLB, OUTPUT);
    pinMode(motorRF, OUTPUT);
    pinMode(motorRB, OUTPUT);

    webServer.on("/", handleRoot);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
}

void loop()
{
    wsServer.loop();
    webServer.handleClient();

    
    if(timeClient.getSeconds() % 20 == 0) {
        currentMillis = millis();
        if (currentMillis - startMillis >= period) {
            sendData();
            startMillis = currentMillis;
        }
            
    }
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

void sendData() {
    if(WiFi.status() == WL_CONNECTED) {
        String payload;
        int reading = analogRead(A0);
        payload = "name=" + botName + "&key=" + botKey + "&air_quality=" + reading;
        httpClient.begin(wifiClient, serverURL);
        httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
        Serial.println(timeClient.getFormattedTime());
        Serial.println(payload);
        int responseCode = httpClient.POST(payload);
        Serial.println(httpClient.getString());
        httpClient.end();
    }
}