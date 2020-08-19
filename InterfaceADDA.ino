#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//PORT B
#define ADC_IN_READ   0x01
#define ADC_OUT_READ  0x02 // ativa em 0
#define ADC_OUT_WRITE 0x04 // ativa em 0
#define ADC_OUT_CLOCK 0x08 // ativa em 1
#define ADC_OUT_CLK_C 0x10 // ativa em 1

//PORT D
#define DAC_REG_CLK1  0x04 // ativa em 1 2
#define DAC_REG_CLK2  0x08 // ativa em 1 3 
#define DAC_WR1       0x10 // ativa em 0 4 
#define DAC_WR2       0x20 // ativa em 0 5
#define DAC_REG_SER   0x40 // ativa em 1 6
#define DAC_REG_OEN   0x80 // ativa em 0 7

uint8_t valor_lido[8] = {0, 0, 0, 0, 0, 0, 0, 0}, aux = 0, valor_final = 0;

void PWMpin11(void) {
  OCR2A = 128;                                            // Configurar o modo de comparacao (ativar o PWM)
  TCCR2A |= (1 << COM2A1);                                // Set modo clear
  TCCR2A |= (1 << WGM21) | (1 << WGM20);                  // Configura a forma de onda para o modo Fast PWM
  TCCR2B |= (1 << CS20)  | (1 << CS22);                   // Configura Prescale (divide o clock por 1024)
}

int main (void) {
  DDRB  = 0x1E;
  PORTB = ADC_OUT_READ | ADC_OUT_WRITE;
  DDRD  = 0xFC;
  PORTD = DAC_WR1 | DAC_WR2;
  PWMpin11();
  // gasta a primeira leitura

  PORTB ^= ADC_OUT_WRITE;
  _delay_ms(1);
  PORTB ^= ADC_OUT_WRITE | ADC_OUT_READ;
  _delay_ms(1);
  PORTB ^= ADC_OUT_READ;
  while (1) {
    // leitura periodica (habilita o adc)
    // write
    PORTB ^= ADC_OUT_WRITE;
    _delay_ms(1);
    // read
    PORTB ^= ADC_OUT_WRITE | ADC_OUT_READ;
    _delay_ms(1);

    // verificação no port se o nivel logico da entrada é alto ou baixo
    for (int i = 0; i < 8; i++) {
      if (PINB & ADC_IN_READ) valor_lido[i] = 1;
      else valor_lido[i] = 0;
      // conta +1 no contador
      PORTB ^= ADC_OUT_CLK_C;
      _delay_ms(1);
      PORTB ^= ADC_OUT_CLK_C;
      _delay_ms(1);
    }
    // tri-state no adc
    PORTB ^= ADC_OUT_READ;

    valor_final = 0;

    for (int i = 0; i < 8; i++) {
      // calcula a potencia de 2 para a posição i
      aux = 1;
      for (int j = 0; j < i; j++) {
        aux *= 2;
      }
      // acumula todos os valores pra gerar um valor de 8 bits
      valor_final += (valor_lido[i] * aux);
    }

    valor_final = 255 - valor_final;

    // fazer processo inverso, descobrir quem é o vetor de bits para o valor_final novo
    // depois enviar para o DAC
    //valor_final = 255 - valor_final;
    for (int i = 0; i < 8; i++) {
      valor_lido[i] = valor_final % 2; // resto da divisão gera 0 ou 1
      valor_final /= 2;                // a divisão reduz 1 bit, deslocando para direita
    }

    for (int i = 0; i < 8; i++) {
      // configurar o registrador, depois o DAC
      if (valor_lido[i] == 1) PORTD |= DAC_REG_SER; // caso o bit seja 1, atualiza o PORTD para ter bit PD6 = 1
      else PORTD &= (~DAC_REG_SER); // caso contrário, bit PD6 = 0
      // atualiza o shift register
      PORTD ^= DAC_REG_CLK1;
      _delay_ms(1);
      PORTD ^= DAC_REG_CLK1;
      _delay_ms(1);
    }
    // manda os dados para a saida
    PORTD ^= DAC_REG_CLK2;
    _delay_ms(1);
    PORTD ^= DAC_REG_CLK2;
    _delay_ms(1);
    // habilita saida do shift register e entrada do dac
    PORTD ^= DAC_WR1;
    _delay_ms(1);
    PORTD ^= (DAC_WR1 | DAC_WR2);
    _delay_ms(1);
    //PORTD ^= DAC_WR2;
    //_delay_ms(1);
    PORTD ^= DAC_WR2;
  }
}
