#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

int main(void)
{
    if (cyw43_arch_init() != 0) {
        return 1;
    }

    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        sleep_ms(500);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        sleep_ms(500);
    }
}
