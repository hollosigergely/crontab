#include "crontab.h"

CronTab cron;


void callback() {
    Serial.println("Callback called");
}

void setup(){
    Serial.begin(115200);
    std::variant<CronID,CronError> ret = cron.add("0 10 * * * *", callback);  // 10th minute of every hours (e.g. 2:10,2:10,...)
    if(std::holds_alternative<CronError>(ret))
    {
        Serial.printf("Error code: %d\n",std::get<CronError>(ret));
        //...
    }
}

void loop() {
    cron.serve();
    delay(500);
}