// #include <Arduino.h>
#include <esp_sleep.h>
#include <LoRa.h>
#include <RTClib.h>
// #include "driver/rtc_io.h"

#define VREF 3.3
#define ADC_RESOLUTION 4095

#define LOOP_DELAY 1000 // Delay between each loop iteration in milliseconds when sleep is not enabled

// Sleep Parameters
#define SLEEP_ENABLE
#ifdef SLEEP_ENABLE
#define SLEEP_TIME 1 * 1000000 // Sleep time in microseconds
#define RTC_ARRAY_SIZE 3
RTC_DATA_ATTR byte bootCount = 0;
#endif

// Temperature Parameters
#define TEMP_ENABLE
#ifdef TEMP_ENABLE
#define TMEMP_PIN_POS 32
#define TMEMP_PIN_NEG 33
#define TEMP_SAMPLES 0xff
#endif

using namespace std;

// put function declarations here:
int mean(const int *arr, const int size);
int standard_deviation(const int *arr, const int size);
int variance(const int *arr, const int size);
int filter(int *arr, const int size, function<bool(int)> func);

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello, World!");

#ifndef SLEEP_ENABLE
  // Repeat the loop indefinitely if sleep is not enabled
  while (true)
  {
#endif

#ifdef TEMP_ENABLE
    // Read temperature sensor values
    int size = TEMP_SAMPLES;
    int tempReadingsPos[TEMP_SAMPLES], tempReadingsNeg[TEMP_SAMPLES];
    for (int i = 0; i < TEMP_SAMPLES; i++)
    {
      tempReadingsPos[i] = analogRead(TMEMP_PIN_POS);
      tempReadingsNeg[i] = analogRead(TMEMP_PIN_NEG);
    }

    // Filter out readings that are more than 1 standard deviations away from the mean
    // d < sqrt(v)
    // d^2 < v
    const int vPos = variance(tempReadingsPos, TEMP_SAMPLES);
    const int mPos = mean(tempReadingsPos, TEMP_SAMPLES);
    const int sizePos = filter(tempReadingsPos, size, [vPos, mPos](int x)
                               { x -= mPos; return x * x < vPos; });

    const int vNeg = variance(tempReadingsNeg, TEMP_SAMPLES);
    const int mNeg = mean(tempReadingsNeg, TEMP_SAMPLES);
    const int sizeNeg = filter(tempReadingsNeg, size, [vNeg, mNeg](int x)
                               { x -= mNeg; return x * x < vNeg; });

    const int tempDiff = mean(tempReadingsPos, sizePos) - mean(tempReadingsNeg, sizeNeg);

    Serial.printf("Calculated Sensor Readings: %.2f\n", (float)tempDiff * VREF / ADC_RESOLUTION);
    Serial.printf("size: %d, sizeNeg: %d, sizePos: %d\n", size, sizeNeg, sizePos);
#endif

#ifdef SLEEP_ENABLE
    // Test RTC memory
    Serial.printf("Boot count: %d\n", bootCount);

    // Got to sleep
    Serial.printf("Entering deep sleep for %d seconds...\n\n", (int)(SLEEP_TIME / 1e6));
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    Serial.flush();
    bootCount++;
    esp_deep_sleep_start();
#else
    delay(LOOP_DELAY);
}
#endif
  }

  void loop() {return;}  // Not needed but Arduino requires definition of loop() when using Arduino.h

  int mean(const int *arr, const int size)
  {
    int sum = 0;
    for (int i = 0; i < size; i++)
      sum += arr[i];
    return sum / size;
  }

  int variance(const int *arr, const int size)
  {
    // Mean of the squares minus the square of the mean
    int arr_sqr[size];
    memcpy(arr_sqr, arr, size * sizeof(int));
    for (int i = 0; i < size; i++)
      arr_sqr[i] *= arr_sqr[i];

    const int m_sqr = mean(arr_sqr, size);

    const int m = mean(arr, size);
    return m_sqr - m * m;
  }

  // Standard deviation is the square root of the variance
  // Use Variance instead of Standard Deviation for filtering to avoid the cost of computing the square root
  int standard_deviation(const int *arr, const int size) { return sqrt(variance(arr, size)); }

  int filter(int *arr, const int size, function<bool(int)> func)
  {
    int filtered_size = size;
    for (int i = 0; i < filtered_size;)
    {
      if (func(arr[i]))
        i++; // Element is valid, move to the next one
      else
      {
        // Move this element to the end of the array and decrease the size of the filtered array
        int temp = arr[i];
        arr[i] = arr[filtered_size - 1];
        arr[filtered_size - 1] = temp;
        filtered_size--;
      }
    }
    return filtered_size;
  }