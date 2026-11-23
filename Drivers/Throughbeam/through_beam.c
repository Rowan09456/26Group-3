#include "through_beam.h"
#include "hardware/gpio.h"

static uint beam_pin;
static volatile bool is_blocked = false;

static void beam_gpio_isr(uint gpio, uint32_t events) {
    if (gpio == beam_pin) {
        // High = clear beam path; Low = obstacle blocking beam
        is_blocked = !gpio_get(beam_pin);
    }
}

void through_beam_init(uint gpio_pin) {
    beam_pin = gpio_pin;
    gpio_init(beam_pin);
    gpio_set_dir(beam_pin, GPIO_IN);
    gpio_pull_up(beam_pin);

    // Initial sensor state
    is_blocked = !gpio_get(beam_pin);

    // Register hardware interrupt for immediate collision response
    gpio_set_irq_enabled_with_callback(
        beam_pin,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &beam_gpio_isr
    );
}

bool through_beam_is_blocked(void) {
    return is_blocked;
}