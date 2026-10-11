#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

#define LED_A_PIN 1
#define LED_B_PIN 2

extern void write_pin(uint8_t pin, bool state);

// Task dedicated to LED A (2 Hz -> 250ms half-period)
void Task_LED_A(void *pvParameters) {
    bool state_A = false;
    
    for (;;) {
        state_A = !state_A;
        write_pin(LED_A_PIN, state_A);
        
        vTaskDelay(pdMS_TO_TICKS(250)); 
    }
}

// Task dedicated to LED B (5 Hz -> 100ms half-period)
void Task_LED_B(void *pvParameters) {
    bool state_B = false;
    
    for (;;) {
        state_B = !state_B;
        write_pin(LED_B_PIN, state_B);
        
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

int main(void) {

    // Create RTOS tasks 
    // Parameters: Function, Name, Stack Size, Task Input, Priority, Task Handle
    xTaskCreate(Task_LED_A, "LED_A_Task", 128, NULL, 1, NULL);
    xTaskCreate(Task_LED_B, "LED_B_Task", 128, NULL, 1, NULL);

    // Hand over control to the RTOS scheduler
    vTaskStartScheduler();

    // The code should never reach this point unless the RTOS crashes
    while (1) {}
    return 0;
}   