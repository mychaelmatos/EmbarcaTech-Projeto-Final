#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/pwm.h"

// Definições para o I2C e Display OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Definições para WS2812 (Matriz 5x5)
#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

// Definições do sistema
#define BUZZER_PIN 10

// Definição dos botões
const uint button_A = 5;  // Simula sensor de nível alto do poço
const uint button_B = 6;  // Simula sensor de nível baixo do tanque

// Adicionar definição para PWM do buzzer
#define BUZZER_VOLUME 65535  // Volume máximo (16 bits)

// Variáveis globais
static ssd1306_t ssd;
static bool poco_tem_agua = true;      // Começa com nível alto (> 10000L)
static bool tanque_nivel_baixo = false; // Começa com nível alto (> 2000L)
static bool bomba_ligada = false;

// Adicionar variável global para controle do buzzer
static uint32_t buzzer_timer = 0;

// Função para controlar os LEDs da matriz
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void atualizar_matriz_leds() {
    // Reduzindo a intensidade dos LEDs para 20% do máximo (51 em vez de 255)
    uint32_t vermelho = urgb_u32(1, 0, 0);
    uint32_t verde = urgb_u32(0, 1, 0);
    uint32_t azul = urgb_u32(0, 0, 1);
    uint32_t desligado = urgb_u32(0, 0, 0);

    for (int i = 0; i < NUM_PIXELS; i++) {
        if (i == 2) { // Terceiro LED da primeira linha - Estado do poço
            // LED verde se poço tiver água suficiente, vermelho se estiver baixo
            put_pixel(poco_tem_agua ? verde : vermelho);
        }
        else if (i == 3) { // Quarto LED da primeira linha - Estado do tanque
            // LED verde se tanque estiver OK, vermelho se estiver baixo
            put_pixel(tanque_nivel_baixo ? vermelho : verde);
        }
        else if (i == 4) { // Quinto LED da primeira linha - Estado da bomba
            // LED azul se bomba ligada, desligado se bomba desligada
            put_pixel(bomba_ligada ? azul : desligado);
        }
        else {
            put_pixel(desligado);
        }
    }
}

void atualizar_display() {
    ssd1306_fill(&ssd, false);
    char buffer[32];
    
    snprintf(buffer, sizeof(buffer), "POCO N: %s", 
             poco_tem_agua ? "ALTO" : "BAIXO");
    ssd1306_draw_string(&ssd, buffer, 2, 0);
    
    snprintf(buffer, sizeof(buffer), "TANQUE N: %s", 
             tanque_nivel_baixo ? "BAIXO" : "ALTO");
    ssd1306_draw_string(&ssd, buffer, 2, 20);
    
    snprintf(buffer, sizeof(buffer), "BOMBA AGUA: %s", 
             bomba_ligada ? "ON" : "OFF");
    ssd1306_draw_string(&ssd, buffer, 2, 40);
    
    ssd1306_send_data(&ssd);
}

void controlar_bomba() {
    // A bomba liga se:
    // - O tanque está com nível baixo
    // - O poço tem água suficiente
    if (tanque_nivel_baixo && poco_tem_agua) {
        bomba_ligada = true;
        
        // Alterna o estado do buzzer a cada 100ms
        if (time_us_32() - buzzer_timer > 100000) {
            buzzer_timer = time_us_32();
            gpio_put(BUZZER_PIN, !gpio_get(BUZZER_PIN)); // Inverte o estado do buzzer
        }
    } else {
        bomba_ligada = false;
        gpio_put(BUZZER_PIN, 0); // Garante que o buzzer está desligado
    }
}

int main() {
    stdio_init_all();

    // Inicialização do I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização do display
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);

    // Inicialização do WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Inicialização dos botões
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);

    // Inicialização do buzzer como GPIO normal (não PWM)
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    while (true) {
        // Ler estado dos "sensores" (botões)
        if (!gpio_get(button_A)) {      // Botão A pressionado
            poco_tem_agua = !poco_tem_agua;  // Inverte o estado do poço
            sleep_ms(200);  // Debounce simples
        }
        
        if (!gpio_get(button_B)) {      // Botão B pressionado
            tanque_nivel_baixo = !tanque_nivel_baixo;  // Inverte o estado do tanque
            sleep_ms(200);  // Debounce simples
        }

        controlar_bomba();
        atualizar_display();
        atualizar_matriz_leds();
        
        sleep_ms(100);
    }

    return 0;
} 
