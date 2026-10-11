#include <stdint.h>
#include <stdbool.h>

#define LED_A_PIN 1
#define LED_B_PIN 2

// The volatile qualifier is mandatory 
volatile bool state_A = false;
volatile bool state_B = false;

extern void write_pin(uint8_t pin, bool state);
extern void configure_hardware_timer(uint8_t timer_id, uint32_t milliseconds);

void ISR_Timer1_Handler(void) {
    state_A = !state_A;
    write_pin(LED_A_PIN, state_A);
}

void ISR_Timer2_Handler(void) {
    state_B = !state_B;
    write_pin(LED_B_PIN, state_B);
}

void init(void) {
    // Configures the underlying hardware to generate interrupts
    configure_hardware_timer(1, 250); // Timer 1 for LED A (2 Hz)
    configure_hardware_timer(2, 100); // Timer 2 for LED B (5 Hz)
}

void main_loop(void) {
    while (1) {
        // The processor is free.
    }
}