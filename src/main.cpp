// #include <Arduino.h>
#include <esp_sleep.h>
#include <LoRa.h>
#include <RTClib.h>
// #include "driver/rtc_io.h"

// Sleep Parameters
// #define SLEEP_ENABLE
#define SLEEP_TIME 3 * 1000000  // Sleep time in microseconds
#define RTC_ARRAY_SIZE 3

// Temperature Pins
#define TEMP_ENABLE
#define TMEMP_PIN_POS 32
#define TMEMP_PIN_NEG 33

// put function declarations here:
# ifdef SLEEP_ENABLE
RTC_DATA_ATTR byte bootCount = 0;
# endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello, World!");

# ifdef TEMP_ENABLE
  // Read temperature sensor values
  const int tempPos = analogRead(TMEMP_PIN_POS);
  const int tempNeg = analogRead(TMEMP_PIN_NEG);
  const int tempDiff = tempPos - tempNeg;

  Serial.printf("Temperature Sensor Readings: Positive: %d\n", tempPos);
  Serial.printf("Temperature Sensor Readings: Negative: %d\n", tempNeg);
  Serial.printf("Calculated Sensor Readings: %d\n", tempDiff);
# endif

# ifdef SLEEP_ENABLE
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
# ifdef TEMP_ENABLE
  // Read temperature sensor values
  const int tempPos = analogRead(TMEMP_PIN_POS);
  const int tempNeg = analogRead(TMEMP_PIN_NEG);
  const int tempDiff = tempPos - tempNeg;

  Serial.printf("Temperature Readings: Positive: %d, Negative: %d, Calculated: %d\n", tempPos, tempNeg, tempDiff);
# endif
}