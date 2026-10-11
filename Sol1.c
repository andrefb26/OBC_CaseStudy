#include <stdint.h>
#include <stdbool.h>

#define LED_A_PIN 1
#define LED_B_PIN 2

// Half-periods in milliseconds (2 Hz = 250ms, 5 Hz = 100ms)
#define HALF_PERIOD_A 250 
#define HALF_PERIOD_B 100 

uint32_t last_time_A = 0;
uint32_t last_time_B = 0;

bool state_A = false;
bool state_B = false;

extern uint32_t get_time_ms(void);
extern void write_pin(uint8_t pin, bool state);

void init(void) {
    last_time_A = get_time_ms();
    last_time_B = get_time_ms();
}

void main_loop(void) {
    while (1) {
        uint32_t current_time = get_time_ms();

        // Non-blocking execution for LED A
        if (current_time - last_time_A >= HALF_PERIOD_A) {
            state_A = !state_A;                  
            write_pin(LED_A_PIN, state_A);    
            last_time_A = current_time;          
        }

        // Non-blocking execution for LED B
        if (current_time - last_time_B >= HALF_PERIOD_B) {
            state_B = !state_B;
            write_pin(LED_B_PIN, state_B);
            last_time_B = current_time;
        }
    }
}