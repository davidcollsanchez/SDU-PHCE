#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

constexpr uint LED_PIN = 7;
constexpr uint BUTTON_PIN = 10;
constexpr uint32_t DEBOUNCE_MS = 20;

int main()
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    bool stable_state = false;
    bool last_reading = false;
    bool last_raw = false;
    uint32_t last_change_ms = to_ms_since_boot(get_absolute_time());

    while (true)
    {
        bool reading = gpio_get(BUTTON_PIN);
        uint32_t now_ms = to_ms_since_boot(get_absolute_time());

        // Given: raw, un-debounced edge on the pin, printed as it happens.
        if (reading != last_raw)
        {
            printf("raw   %s at %u ms\n", reading ? "rising (0->1)" : "falling (1->0)", now_ms);
            last_raw = reading;
        }

        // TODO 1: if `reading` differs from `last_reading`, update `last_reading`
        //         and reset `last_change_ms` to `now_ms`.
        if (reading != last_reading)
        {
            last_reading = reading;
            last_change_ms = now_ms;   // restart the "has it been stable?" timer
        }

        // TODO 2: if the reading has been stable for at least DEBOUNCE_MS, and it
        //         differs from `stable_state`, update `stable_state`.

        // TODO 3: whenever `stable_state` just changed, print "pressed" or
        //         "released" to the Serial Monitor.

        if ((now_ms - last_change_ms) >= DEBOUNCE_MS && reading != stable_state)
        {
            stable_state = reading;
            printf("%s at %u ms\n", stable_state ? "pressed" : "released", now_ms);
        }

        // TODO 4: drive the LED to match `stable_state`.
        gpio_put(LED_PIN, stable_state);

        sleep_ms(1);
    }
}