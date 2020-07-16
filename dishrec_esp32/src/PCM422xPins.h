#ifndef PCM422XPINS_H
#define PCM422XPINS_H

#include "driver/gpio.h"

/* Pin names, I/O types, and descriptions from PCM4220 datasheet
Pin numbers are pins on MCU */

/* PCM output enable (active high)
ADC I/O: Input */
#define PCMEN

/* Right channel high-pass filter disable (active high)
ADC I/O: Input */
#define HPFDR

/* Input Left channel high-pass filter disable (active high)
ADC I/O: Input */
#define HPFDL

/* Sampling Mode:
FS0 = 0 and FS1 = 0: Normal mode
FS0 = 1 and FS1 = 0: Double Speed mode
FS0 = 0 and FS1 = 1: Quad Speed mode
FS0 = 1 and FS1 = 1: Reserved Sampling mode
ADC I/O: Input */
#define FS0
#define FS1

/* Digital decimation filter response
DF = 0: Classic filter response
DF = 1: Low Group Delay response
ADC I/O: Input */
#define DF

/* TDM active sub-frame:
SUB0 = 0 and SUB1 = 0: Sub-frame 0
SUB0 = 1 and SUB1 = 0: Sub-frame 1
SUB0 = 0 and SUB1 = 1: Sub-frame 2
SUB0 = 1 and SUB1 = 1: Sub-frame 3
ADC I/O: Input */
#define SUB1
#define SUB0

/* Audio serial port data
ADC I/O: Output */
#define DATA

/* Audio serial port bit clock
ADC I/O: I/O */
#define BCK

/* Audio serial port left/right word clock
ADC I/O: I/O */
#define LRCK

/* Master clock
ADC I/O: Input */
#define MCKI

/* Reset and power-down (active low)
ADC I/O: Input */
#define RST

/* Left channel overflow flag (active high)
ADC I/O: Output */
#define OVFL

/* Right channel overflow flag (active high)
ADC I/O: Output */
#define OVFR

/* Audio serial port Slave/Master mode:
S/M = 0: Master mode
S/M = 1: Slave mode
ADC I/O: Input */
#define SM

/* Output word length:
OWL0 = 0 and OWL1 = 0: 24-bits
OWL0 = 1 and OWL1 = 0: 18-bits
OWL0 = 0 and OWL1 = 1: 20-bits
OWL0 = 1 and OWL1 = 1: 16-bits
ADC I/O: Input */
#define OWL1
#define OWL0

/* Audio data format:
FMT0 = 0 and FMT1 = 0: Left-justified
FMT0 = 1 and FMT1 = 0: I2S
FMT0 = 0 and FMT1 = 1: TDM
FMT0 = 1 and FMT1 = 1: TDM with one BCK delay
ADC I/O: Input */
#define FMT1
#define FMT0


/* Additional MCU pin assignments specific to PCM4222 */

/* Right channel high-pass filter disable input (active high),
or modulator Data output 1 (LSB) when MODEN = high
ADC I/O: I/O */
#define MOD1                    (HPFDR)

/* Left channel high-pass filter disable input (active high),
or modulator data output 2 when MODEN = high
ADC I/O: I/O */
#define MOD2                    (HPFDL)

/* DSD output enable input (active high),
or modulator data output 6 (MSB) when MODEN = high
ADC I/O: I/O */
#define MOD3                    (FS0)

/* PCM sampling mode selection input,
or modulator data output 4 when MODEN = high
ADC I/O: I/O */
#define MOD4                    (FS1)

/* Digital decimation filter response selection Input,
or modulator data output 5 when MODEN = high
ADC I/O: I/O */
#define MOD5                    (DF)

/* DSD output enable input (active high),
or modulator data output 6 (MSB) when MODEN = high
ADC I/O: I/O */
#define DSDEN
#define MOD6                    (DSDEN)

/* Multi-bit modulator output enable (Active High)
ADC I/O: Input */
#define MODEN

/* DSD output mode/rate
ADC I/O: Input */
#define DSDMODE

/* TDM active sub-frame selection input,
or master clock output when MODEN = high
ADC I/O: I/O */
#define MCKO                    (SUB1)

/* TDM active sub-frame selection input,
or modulator left/right word clock output when MODEN = high
ADC I/O: I/O */
#define WCKO                    (SUB0)


/* Pin selection macros for GPIO config */

#define GPIO_OUTPUT_PINS        (
        /* Common pins */
        (1ULL << PCMEN)
        | (1ULL << HPFDR)
        | (1ULL << HPFDL)
        | (1ULL << FS0)
        | (1ULL << FS1)
        | (1ULL << DF)
        | (1ULL << SUB1)
        | (1ULL << SUB0)
        | (1ULL << RST)
        | (1ULL << SM)
        | (1ULL << OWL1)
        | (1ULL << OWL0)
        | (1ULL << FMT1)
        | (1ULL << FMT0)
        
        /* PCM4222 only */
        | (1ULL << DSDEN)
        | (1ULL << MODEN)
        | (1ULL << DSDMODE)
    )

#define GPIO_INPUT_PINS         ((1ULL << OVFL) | (1ULL << OVFR))

#endif