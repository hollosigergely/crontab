Flexible and simple CRONTAB implementation
==========================================

The software is mainly for using in embedded systems, like ESP8266 and ESP32. The implementation is flexible is a way, that different time sources can be used. The software uses the [ccroneexpr](https://github.com/staticlibs/ccronexpr) cron expression parser. Implementation uses C++17 features.

Examples
--------

Basically, you just instantiate the `CronTab` class, and add callbacks. Do not forget to priodically call `serve()` to check the callbacks.
```c++
#include "crontab.h"

CronTab cron;

void callback() {
  Serial.println("Callback called");
}

void setup(){
    //...
    std::variant<CronID,CronError> ret = cron.add("0 10 * * * *", callback);  // 10th minute of every hours (e.g. 2:10,2:10,...)
    if(std::holds_alternative<CronError>(ret))
    {
        cout << "Error code: " << std::get<CronError>(ret) << endl;
        //...
    }
    //...
}

void loop() {
  cron.serve();
  delay(500);
}

```

Callbacks
---------

Since the interface is based on `std::function`, every kind of function can be used as callbacks:
```c++
class MyClass {
public:
  void mycallback(int count) {}
};

MyClass c;

//...
cron.add("0 10 * * * *", std::bind(&MyClass::mycallback,c,100)); // count = 100
//...
```

Time interface
--------------

By default, the library uses the `time_t time(time_t*)` system call. However, this can be override by calling the `CronTab::setTimeFunction(std::function<time_t(time_t*)> fn)` method. This can be used e.g. for using an external NTP client:

```c++
//...
NTPClient timeClient(ntpUDP);
CronTab cron;

void setup() {
  cron.setTimeFunction([&](time_t*) { return ntpClient.getEpochTime(); });
  //...
}
```

NTP
---

You can find an example using the (Arduino NTPClient)[https://github.com/arduino-libraries/NTPClient]. Please note, that the library method `serve()` only checks, that the timepoint for the callback has elapsed or not. The clock typically starts from 0 (in case of embedded systems), and when the clock jump to the actual time (e.g. NTP sets new time), the callbacks might fire spuriously. To work properly, you should ensure that NTP has set the clock already, before using crontab (see example).
