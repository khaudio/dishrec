#ifndef PCM422XPINS_H
#define PCM422XPINS_H

#include "driver/gpio.h"

/* Pin names, I/O types, and descriptions are from
PCM4220 datasheet.  Pin numbers reference MCU. */

/* PCM output enable (active high)
ADC I/O: Input */
#define PCM422X_PCMEN               (GPIO_NUM_22)

/* Right channel high-pass filter disable (active high)
ADC I/O: Input */
#define PCM422X_HPFDR               (GPIO_NUM_2)

/* Input Left channel high-pass filter disable (active high)
ADC I/O: Input */
#define PCM422X_HPFDL               (GPIO_NUM_15)

/* Sampling Mode:
FS0 = 0 and FS1 = 0: Normal mode
FS0 = 1 and FS1 = 0: Double Speed mode
FS0 = 0 and FS1 = 1: Quad Speed mode
FS0 = 1 and FS1 = 1: Reserved Sampling mode
ADC I/O: Input */
#define PCM422X_FS0                 (GPIO_NUM_9)
#define PCM422X_FS1                 (GPIO_NUM_10)

/* Digital decimation filter response
DF = 0: Classic filter response
DF = 1: Low Group Delay response
ADC I/O: Input */
#define PCM422X_DF                  (GPIO_NUM_27)

/* TDM active sub-frame:
SUB0 = 0 and SUB1 = 0: Sub-frame 0
SUB0 = 1 and SUB1 = 0: Sub-frame 1
SUB0 = 0 and SUB1 = 1: Sub-frame 2
SUB0 = 1 and SUB1 = 1: Sub-frame 3
ADC I/O: Input */
#define PCM422X_SUB0                (GPIO_NUM_17)
#define PCM422X_SUB1                (GPIO_NUM_1)

/* Audio serial port data
ADC I/O: Output */
#ifdef I2S_IN_DI
#define PCM422X_DATA                (I2S_IN_DI)
#else
#define PCM422X_DATA                (GPIO_NUM_39)
#endif

/* Audio serial port bit clock
ADC I/O: I/O */
#ifdef I2S_IN_BCK
#define PCM422X_BCK                 (I2S_IN_BCK)
#else
#define PCM422X_BCK                 (GPIO_NUM_33)
#endif

/* Audio serial port left/right word clock
ADC I/O: I/O */
#ifdef I2S_IN_WS
#define PCM422X_LRCK                (I2S_IN_WS)
#else
#define PCM422X_LRCK                (GPIO_NUM_32)
#endif

/* Master clock
ADC I/O: Input */
#ifdef I2S_MCK
#define PCM422X_MCKI                (I2S_MCK)
#else
#define PCM422X_MCKI                (GPIO_NUM_0)
#endif

/* Reset and power-down (active low)
ADC I/O: Input */
#define PCM422X_RST                 (GPIO_NUM_13)

/* Left channel overflow flag (active high)
ADC I/O: Output */
#define PCM422X_OVFL                (GPIO_NUM_34)

/* Right channel overflow flag (active high)
ADC I/O: Output */
#define PCM422X_OVFR                (GPIO_NUM_35)

/* Audio serial port Slave/Master mode:
S/M = 0: Master mode
S/M = 1: Slave mode
ADC I/O: Input */
#define PCM422X_SM                  (GPIO_NUM_26)

/* Output word length:
OWL0 = 0 and OWL1 = 0: 24-bits
OWL0 = 1 and OWL1 = 0: 18-bits
OWL0 = 0 and OWL1 = 1: 20-bits
OWL0 = 1 and OWL1 = 1: 16-bits
ADC I/O: Input */
#define PCM422X_OWL0                (GPIO_NUM_14)
#define PCM422X_OWL1                (GPIO_NUM_12)

/* Audio data format:
FMT0 = 0 and FMT1 = 0: Left-justified
FMT0 = 1 and FMT1 = 0: I2S
FMT0 = 0 and FMT1 = 1: TDM
FMT0 = 1 and FMT1 = 1: TDM with one BCK delay
ADC I/O: Input */
#define PCM422X_FMT0                (GPIO_NUM_8)
#define PCM422X_FMT1                (GPIO_NUM_7)


/* Additional MCU pin assignments specific to PCM4222 */

/* Right channel high-pass filter disable input (active high),
or modulator Data output 1 (LSB) when MODEN = high
ADC I/O: I/O */
#define PCM422X_MOD1                (PCM422X_HPFDR)

/* Left channel high-pass filter disable input (active high),
or modulator data output 2 when MODEN = high
ADC I/O: I/O */
#define PCM422X_MOD2                (PCM422X_HPFDL)

/* DSD output enable input (active high),
or modulator data output 6 (MSB) when MODEN = high
ADC I/O: I/O */
#define PCM422X_MOD3                (PCM422X_FS0)

/* PCM sampling mode selection input,
or modulator data output 4 when MODEN = high
ADC I/O: I/O */
#define PCM422X_MOD4                (PCM422X_FS1)

/* Digital decimation filter response selection Input,
or modulator data output 5 when MODEN = high
ADC I/O: I/O */
#define PCM422X_MOD5                (PCM422X_DF)

/* DSD output enable input (active high),
or modulator data output 6 (MSB) when MODEN = high
ADC I/O: I/O */
#define PCM422X_DSDEN               (GPIO_NUM_3)
#define PCM422X_MOD6                (PCM422X_DSDEN)

/* Multi-bit modulator output enable (Active High)
ADC I/O: Input */
#define PCM422X_MODEN               (GPIO_NUM_6)

/* DSD output mode/rate
ADC I/O: Input */
#define PCM422X_DSDMODE             (GPIO_NUM_11)

/* TDM active sub-frame selection input,
or master clock output when MODEN = high
ADC I/O: I/O */
#define PCM422X_MCKO                (PCM422X_SUB1)

/* TDM active sub-frame selection input,
or modulator left/right word clock output when MODEN = high
ADC I/O: I/O */
#define PCM422X_WCKO                (PCM422X_SUB0)


/* Pin selection macros for GPIO config */

/* Common pins */

#define PCM4220_GPIO_OUTPUT_PINS    ((1ULL << PCM422X_PCMEN) \
        | (1ULL << PCM422X_HPFDR)   \
        | (1ULL << PCM422X_HPFDL)   \
        | (1ULL << PCM422X_FS0)     \
        | (1ULL << PCM422X_FS1)     \
        | (1ULL << PCM422X_DF)      \
        | (1ULL << PCM422X_SUB1)    \
        | (1ULL << PCM422X_SUB0)    \
        | (1ULL << PCM422X_RST)     \
        | (1ULL << PCM422X_SM)      \
        | (1ULL << PCM422X_OWL1)    \
        | (1ULL << PCM422X_OWL0)    \
        | (1ULL << PCM422X_FMT1)    \
        | (1ULL << PCM422X_FMT0)    \
    )

/* PCM4222 only */
#define PCM4222_GPIO_OUTPUT_PINS    ((PCM4220_GPIO_OUTPUT_PINS) \
        | (1ULL << PCM422X_DSDEN)   \
        | (1ULL << PCM422X_MODEN)   \
        | (1ULL << PCM422X_DSDMODE) \
    )

#define PCM422X_GPIO_INPUT_PINS     ((1ULL << PCM422X_OVFL) | (1ULL << PCM422X_OVFR))

#endif