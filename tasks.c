#include "FreeRTOS.h"
#include "task.h"

// Definições de tempos (em milissegundos) e constantes
#define HEARTBEAT_PERIOD_MS 200
#define HEARTBEAT_TIMEOUT_MS 500
#define SYNC_MAGIC_BYTE 0x5A

// Variáveis Globais de Estado
OBC_State_t current_state = OBC_STATE_SLAVE; // O sistema arranca como Slave por defeito
uint32_t last_heartbeat_time = 0;
uint16_t tx_counter = 0;
uint16_t last_rx_counter = 0;

// Declaração de funções HAL (Hardware Abstraction Layer)
extern uint32_t get_system_time_ms(void);
extern void transmit_data(uint8_t* data, uint8_t size);
extern bool receive_data(uint8_t* buffer, uint8_t size, uint32_t timeout);


// --- TAREFA 1: TRANSMISSÃO DO MASTER ---
void Task_Heartbeat_TX(void *pvParameters) {
    HeartbeatPacket_t tx_packet;
    tx_packet.sync_byte = SYNC_MAGIC_BYTE;

    for (;;) {
        // Só transmite se tiver a autoridade de Master
        if (current_state == OBC_STATE_MASTER) {
            tx_counter++;
            tx_packet.counter = tx_counter;
            tx_packet.checksum = calcular_checksum(&tx_packet);

            transmit_data((uint8_t*)&tx_packet, sizeof(tx_packet));
        }
        
        // Cede o processador durante 200ms
        vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS));
    }
}


// --- TAREFA 2: RECEÇÃO DO SLAVE E ARBITRAGEM DE FALHAS ---
void Task_Heartbeat_RX_Monitor(void *pvParameters) {
    HeartbeatPacket_t rx_packet;

    // Assumimos que o satélite acabou de ligar, regista o tempo inicial
    last_heartbeat_time = get_system_time_ms(); 

    for (;;) {
        if (current_state == OBC_STATE_SLAVE) {
            
            // Tenta ler o barramento (função de leitura não-bloqueante ou com timeout curto)
            bool received = receive_data((uint8_t*)&rx_packet, sizeof(rx_packet), 10);

            // 1. Fase de Validação
            if (received) {
                if (rx_packet.sync_byte == SYNC_MAGIC_BYTE &&
                    calcular_checksum(&rx_packet) == rx_packet.checksum &&
                    rx_packet.counter > last_rx_counter) {

                    // Pacote íntegro e atualizado! Regista a hora exata da receção.
                    last_heartbeat_time = get_system_time_ms();
                    last_rx_counter = rx_packet.counter;
                }
            }

            // 2. Fase de Arbitragem (O Mecanismo de Sobrevivência)
            if ((get_system_time_ms() - last_heartbeat_time) > HEARTBEAT_TIMEOUT_MS) {
                
                // TIMEOUT CRÍTICO ATINGIDO: O Master original falhou.
                // Executa a transição de estado imediata.
                current_state = OBC_STATE_MASTER;
                
                // reset_hardware_peripherals(); // Reconfigura os pinos de controlo
            }
        }
        
        // Pausa muito curta para permitir a execução de outras tarefas do sistema
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}