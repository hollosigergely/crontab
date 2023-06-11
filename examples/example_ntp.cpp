#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "crontab.h"

const char *ssid     = "<SSID>";
const char *password = "<PASSWORD>";

WiFiUDP     ntpUDP;
NTPClient   timeClient(ntpUDP);
CronTab     cron;

void setup(){
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }

    timeClient.begin();

    Serial.println("Waiting for NTP");
	while(!timeClient.isTimeSet())
	{
		Serial.print(".");
		timeClient.update();
		delay(1000);
	}

    cron.setTimeFunction([&](time_t*) { return timeClient.getEpochTime(); });

    std::variant<CronID,CronError> ret = cron.add("0 10 * * * *", callback);  // 10th minute of every hours (e.g. 2:10,2:10,...)
    if(std::holds_alternative<CronError>(ret))
    {
        Serial.printf("Error code: %d\n",std::get<CronError>(ret));
        //...
    }
}

void loop() {
    timeClient.update();
    cron.serve();
    
    Serial.println(timeClient.getFormattedTime());

    delay(1000);
}