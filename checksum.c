#include <stdint.h>
#include <stdbool.h>

// 1. Definição da Máquina de Estados do OBC
typedef enum {
    OBC_STATE_SLAVE = 0,
    OBC_STATE_MASTER = 1
} OBC_State_t;

// 2. Estrutura do pacote de Heartbeat 
typedef struct __attribute__((packed)) {
    uint8_t sync_byte;      // Byte de sincronização (ex: 0x5A) para início de trama
    uint16_t counter;       // Contador incremental (prova de processamento lógico da ALU)
    uint8_t checksum;       // Verificação de integridade
} HeartbeatPacket_t;

// 3. Função para calcular o Checksum (Operação XOR)
// Retorna o valor XOR linear de todos os bytes de dados antes do próprio checksum
uint8_t calcular_checksum(HeartbeatPacket_t *packet) {
    uint8_t *byte_array = (uint8_t *)packet;
    uint8_t xor_result = 0;
    
    // O tamanho do pacote é 4 bytes. Os dados ocupam os primeiros 3 bytes.
    // byte_array[0] = sync_byte
    // byte_array[1] = byte inferior do counter
    // byte_array[2] = byte superior do counter
    for (int i = 0; i < 3; i++) {
        xor_result ^= byte_array[i];
    }
    
    return xor_result;
}