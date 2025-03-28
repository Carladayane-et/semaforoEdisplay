#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define BTN_C_PIN 7

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

void display_message(const char *lines[], int line_count) {
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    int y = 0;
    for (int i = 0; i < line_count; i++) {
        ssd1306_draw_string(ssd, 5, y, (char *) lines[i]); 
        y += 8;
    }

    render_on_display(ssd, &frame_area);
}

int A_state = 0;    //Botao A está pressionado?

void SinalVerde(){
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);   
}

void SinalAmarelo(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
}

void SinalVermelho(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
}

int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

int main(){
    stdio_init_all(); 

    i2c_init(i2c1, ssd1306_i2c_clock * 1000); 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
    // INICIANDO LED
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    // INICIANDO BOTÃO
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
    gpio_init(BTN_C_PIN);
    gpio_set_dir(BTN_C_PIN, GPIO_IN);
    gpio_pull_up(BTN_C_PIN);

    while(true){
        SinalVermelho();
        const char *msg1[] = {"\n ", "SINAL FECHADO", "\n ", "AGUARDE"};
        display_message(msg1, 4);
        A_state = WaitWithRead(8000);// espera a leitura do botão

        if(A_state){                  //ALGUEM APERTOU O BOTÃO - SAI DO SEMAFORO NORMAL
            // SINAL AMARELO PARA O PEDESTRE POR 2s
            SinalAmarelo();
            const char *msg2[] = {"\n", "SINAL DE","\n", "ATENCAO", "\n", "PREPARE-SE"};
            display_message(msg2, 6);
            sleep_ms(2000);
            // SINAL VERDE PARA O PEDESTRE POR 10s
            SinalVerde();
            const char *msg3[] = {"\n", "SINAL ABERTO", "\n", "ATRAVESSAR COM","\n", "CUIDADO"};
            display_message(msg3, 6);
            sleep_ms(10000);
        } else {                      //NINGUEM APERTOU O BOTÃO - SEMAFORO NORMAL
            
            SinalAmarelo();
            const char *msg2[] = {"\n", "SINAL DE","\n", "ATENCAO", "\n", "PREPARE-SE"};
            display_message(msg2, 6);
            sleep_ms(2000);
            // SINAL VERDE PARA O PEDESTRE POR 8s
            SinalVerde();
            const char *msg3[] = {" \n", "SINAL ABERTO", "\n ", "ATRAVESSAR COM","\n", "CUIDADO"};
            display_message(msg3, 6);
            sleep_ms(8000);
        }
    }

    return 0;
}