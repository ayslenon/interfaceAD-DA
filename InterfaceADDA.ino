// Author Ayslenon Câmara - 2019 
// Little project made for eletrocnic instrumentation classes


// ================================================================================================================
// include libs for project
// ================================================================================================================
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



// ================================================================================================================
// hardware mapping 
// ================================================================================================================

//PORT B
#define ADC_IN_READ   0x01  
#define ADC_OUT_READ  0x02 // actives in 0
#define ADC_OUT_WRITE 0x04 // actives in 0
#define ADC_OUT_CLOCK 0x08 // actives in 1
#define ADC_OUT_CLK_C 0x10 // actives in 1

//PORT D
#define DAC_REG_CLK1  0x04 // actives in 1
#define DAC_REG_CLK2  0x08 // actives in 1 
#define DAC_WR1       0x10 // actives in 0 
#define DAC_WR2       0x20 // actives in 0
#define DAC_REG_SER   0x40 // actives in 1
#define DAC_REG_OEN   0x80 // actives in 0

// note: where we see "pulse" for any bit, it means change its logic value to opposite, and back again to "normal value"
// because some bits are active in 0, as we can see above

// ================================================================================================================
// variables declaration 
// ================================================================================================================

uint8_t read_value = 0, final_value = 0;


// ================================================================================================================
// funcions declaration
// ================================================================================================================

// function to set pwm on arduino pin 11 with duty 50% 
// duty calculed by OCR2A/255 * 100%
void PWMpin11(void) {
  OCR2A = 128;                                            // Set pwm with compare mode
  TCCR2A |= (1 << COM2A1);                                // Set compare to clear
  TCCR2A |= (1 << WGM21) | (1 << WGM20);                  // Set Waveform Genertion to Fast PWM
  TCCR2B |= (1 << CS20)  | (1 << CS22);                   // Set Prescale to divide the clock by 1024
}


// ================================================================================================================
// main function 
// ================================================================================================================

int main (void) {
  // Set the inputs and outputs according to the hardware
  DDRB  = 0x1E;
  DDRD  = 0xFC;
  // Start ADC and DAC bits on its normal operation
  PORTB = ADC_OUT_READ | ADC_OUT_WRITE;
  PORTD = DAC_WR1 | DAC_WR2;
  // Starts the clk reference to ADC
  PWMpin11();
  
  // Starting the first read, because the first is always slowly compared to next ones
  // We make a new read by pulsing the ADC_OUT_WRITE and then ADC_OUT_READ 
  PORTB ^= ADC_OUT_WRITE;
  _delay_ms(1);
  PORTB ^= ADC_OUT_WRITE | ADC_OUT_READ;
  _delay_ms(1);
  PORTB ^= ADC_OUT_READ;

  // Here we have a new reading stored on ADC 8bit Register

  
// ================================================================================================================
// here starts the infiniy loop
// ================================================================================================================
  while (1) {
    // We are only read the value on ADC, make a complement of 1 and send to DAC
    // We start pulsing ADC_OUT_WRITE to start a conversion
    PORTB ^= ADC_OUT_WRITE;
    _delay_ms(1);
    // then set ADC_OUT_READ 
    PORTB ^= ADC_OUT_WRITE | ADC_OUT_READ;
    _delay_ms(1);

    // Check if the logic level in the serial input is high or low for each bit in ADC register
    read_value = 0;
    for (int i = 0; i < 8; i++) {
      if (PINB & ADC_IN_READ) read_value += (1<<i);
      // Pulse ADC_OUT_CLK_C to increment one bit for the parallel to serial conversion via the 4051 IC
      PORTB ^= ADC_OUT_CLK_C;
      _delay_ms(1);
      PORTB ^= ADC_OUT_CLK_C;
      _delay_ms(1);
    }
    // Set tri-state on ADC reseting ADC_OUT_READ
    PORTB ^= ADC_OUT_READ;
    
    // make the complement of 1 from the read_value
    final_value = 255 - read_value;

    for (int i = 0; i < 8; i++) {
      // According to the read_value, we set and reset the DAC_REG_SER

      if ((read_value & (1 << i)) == (1 << i)) PORTD |= DAC_REG_SER;
      else PORTD &= (~DAC_REG_SER);
      
      // then we pulse the DAC_REG_CLK1 to shift the register 74595 IC

      PORTD ^= DAC_REG_CLK1;
      _delay_ms(1);
      PORTD ^= DAC_REG_CLK1;
      _delay_ms(1);
    }
    // after all 8 bits shifted, we allow the output to send its values to the DAC
    // by pulsing DAC_REG_CLK2
    PORTD ^= DAC_REG_CLK2;
    _delay_ms(1);
    PORTD ^= DAC_REG_CLK2;
    _delay_ms(1);
    // And finally pulsing DAC_WR1 and then DAC_WR2
    // To receive the 8bit incoming from register and then convert the data respectively
    PORTD ^= DAC_WR1;
    _delay_ms(1);
    PORTD ^= (DAC_WR1 | DAC_WR2);
    _delay_ms(1);
    //PORTD ^= DAC_WR2;
    //_delay_ms(1);
    PORTD ^= DAC_WR2;
  }
}
