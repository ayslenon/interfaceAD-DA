# Interface AD DA to use with uC

## Chacteristics 
- Developed for atmega328p
- Written according to the datasheets of components
- 2 examples of uses of DAC0832
- Multiplexed input, using less pins
- Made in Arduino IDE

## Overview
This project was made for a class, the focus is to understand how the ADC and DAC conversion works.
Here I used the ADC0804 and DAC0832, and made a interface for each one to use with the Atmega328p
The ADC0804 has 8bit parallel output, so I multiplexed its outputs to have one bit serial, and 
made the conversion with a 4051 IC multiplexer and a 40193 IC counter.

The counter receives pulses in its clock pin, incoming from the uC, and increment the 4051 selector pins
so the 4051 will convert the 8 bit to 1 bit-serial.
The uC will read each serial bit before a new increment on 40193.

Afterwards, the uC will have the AD conversion in its memory, and we can use this however we want, in my case
I decided to make a complement of 1, and send a 8bit value to the DAC.

For the DA conversion I decided to use a 74595 shift register to store 8bit incoming from uC, and then send these 8bit to 
the DAC0832, using it in 2 different schematics to compare its outputs

The store is made by a clock incrementing the shift register, after 8 shifts the output is set to send data to the DAC

I recommend you to see the datasheets, from both IC I used:
- [ADC0804](https://www.ti.com/lit/ds/symlink/adc0804-n.pdf) 
- [40193 Counter](https://www.ti.com/lit/ds/symlink/cd40193b.pdf?ts=1606420709126&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FCD40193B)
- [4051 Multiplexer](https://www.ti.com/lit/ds/symlink/cd4051b.pdf?ts=1606420744325&ref_url=https%253A%252F%252Fwww.google.com%252F)
- [DAC0832](https://datasheet.octopart.com/DAC0832LCN-National-Semiconductor-datasheet-14101993.pdf)
- [74595 Shift register](https://www.ti.com/lit/ds/scls041i/scls041i.pdf?ts=1606413365686&ref_url=https%253A%252F%252Fwww.google.com%252F)
- [Atmega328p](https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf)

## Circuitry

The following 3 circuits presents the schematic connections made in this project.

### ADC Circuit
![ADC circuit](/img/ADC.png)

### ATmega Circuit
![ATmega circuit](/img/uC.png)

### DAC circuit
![DAC circuit - 74595](/img/DAC1.png)
![DAC circuit - DAC1](/img/DAC2.png)
![DAC circuit - DAC2](/img/DAC3.png)

### Circuit Overview
![Overview](/img/over.png)
