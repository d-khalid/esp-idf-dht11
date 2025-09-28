//
// Created by alkalinelemon on 9/28/25.
//

#ifndef IDF_DHT11_H
#define IDF_DHT11_H
#include "../../../esp-idf/components/esp_driver_gpio/include/driver/gpio.h"
#include "rom/ets_sys.h"

typedef struct {
    int data_pin;
    float temperature;
    float humidity;
} idf_dht11;

// Listens for a particular state for specific, returns -1 on timeout
int await_state(idf_dht11 dht11, int state, int timeout_us);

// Keep the data pin of the dht11 low for the given time
void hold_low(idf_dht11, int us_to_hold);

// Takes one reading from the sensor, returns -1 if failed
int dht11_read(idf_dht11* dht11, int tries);

#endif //IDF_DHT11_H
