#include "esp-idf-dht11.h"
#include "esp_log.h"

#define DHT11_TAG "DHT11: "

int await_state(idf_dht11 dht11, int state, int timeout_us) {
    int timer = 0;
    gpio_set_direction(dht11.data_pin, GPIO_MODE_INPUT);

    while (gpio_get_level(dht11.data_pin) != state) {
        if (timer >= timeout_us) { return -1; }
        timer += 2;
        ets_delay_us(2);
    }
    return timer;
}

void hold_low(idf_dht11 dht11, int us_to_hold) {
    gpio_set_direction(dht11.data_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(dht11.data_pin, 0);
    ets_delay_us(us_to_hold);
    gpio_set_level(dht11.data_pin, 1);
}

int dht11_read(idf_dht11* dht11, int tries) {

    uint8_t data[5] = {0};
    int one_timer;
    int zero_timer;

    int attempt_counter = 0;

    do {
        attempt_counter++;
        // Hold low for 18ms
        hold_low(*dht11, 18000);

        // Wait ~45us for response part 1 [0]
        if (await_state(*dht11, 0, 45) == -1) {
            ESP_LOGE(DHT11_TAG, "Phase 1 failed!");
            ets_delay_us(20000);
            continue;
        }

        // Wait ~90us for response part 2 [1]
        if (await_state(*dht11, 1, 90) == -1) {
            ESP_LOGE(DHT11_TAG, "Phase 2 failed!");
            ets_delay_us(20000);
            continue;
        }

        // Wait ~90us for response part 3 [10]
        if (await_state(*dht11, 0, 90) == -1) {
            ESP_LOGE(DHT11_TAG, "Phase 3 failed!");
            ets_delay_us(20000);
            continue;
        }

        break;

    } while (attempt_counter < tries);


    // 5 bytes - 40 bits
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0;j < 8;j++)
        {
            // Check how long '1' was held, insert bit based on that
            zero_timer = await_state(*dht11, 1, 55);
            one_timer = await_state(*dht11, 0, 78);

            // Insert value
            data[i] = data[i] << 1;
            data[i] |= (one_timer > zero_timer);

        }
    }
    uint8_t crc = data[0] + data[1] + data[2] + data[3];

    if (crc != data[4]) {
        ESP_LOGE(DHT11_TAG, "Invalid Checksum!");
        return -1;
    }

    // Assign values on success
    dht11->humidity = data[0] + data[1]/10.0;
    dht11->temperature = data[2] + data[3]/10.0;

    return 0;

}
