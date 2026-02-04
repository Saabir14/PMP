// #include <Arduino.h>
#include <esp_sleep.h>
#include <LoRa.h>
#include <RTClib.h>
// #include "driver/rtc_io.h"

// Sleep Parameters
#define SLEEP_TIME 0 * 1000000  // Sleep time in microseconds
#define RTC_ARRAY_SIZE 3

// put function declarations here:
# if SLEEP_TIME > 0
RTC_DATA_ATTR byte bootCount = 0;
# endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello, World!");

# if SLEEP_TIME > 0
  // Test RTC memory
  Serial.printf("Boot count: %d\n", bootCount);
  
  // Got to sleep
  Serial.printf("Entering deep sleep for %d seconds...\n\n", (int)(SLEEP_TIME / 1e6));
  esp_sleep_enable_timer_wakeup(SLEEP_TIME);
  Serial.flush();
  bootCount++;
  esp_deep_sleep_start();
# endif
}

void loop() {
  // put your main code here, to run repeatedly:
}