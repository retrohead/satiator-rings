#include <stdint.h>

#define SCI_REG_SMR     *((volatile uint8_t *)0xFFFFFE00)
#define SCI_REG_BRR     *((volatile uint8_t *)0xFFFFFE01)
#define SCI_REG_SCR     *((volatile uint8_t *)0xFFFFFE02)
#define SCI_REG_TDR     *((volatile uint8_t *)0xFFFFFE03)
#define SCI_REG_SSR     *((volatile uint8_t *)0xFFFFFE04)
#define SCI_REG_RDR     *((volatile uint8_t *)0xFFFFFE05)

#define SCR_TIE         0x80
#define SCR_RIE         0x40
#define SCR_TE          0x20
#define SCR_RE          0x10
#define SCR_MPIE        0x08
#define SCR_TEIE        0x04

#define SSR_TDRE        0x80
#define SSR_RDRF        0x40
#define SSR_ORER        0x20
#define SSR_FER         0x10
#define SSR_PER         0x08
#define SSR_TEND        0x04
#define SSR_MPB         0x02
#define SSR_MPBT        0x01

void sci_putchar(uint8_t ch) {
    while (!(SCI_REG_SSR & SSR_TDRE));
    SCI_REG_TDR = ch;
    SCI_REG_SSR &= ~SSR_TDRE;
}

void sci_init(void) {
    SCI_REG_BRR = 21;
    SCI_REG_SMR = 0;
    SCI_REG_SCR = SCR_TE;
}

static void sci_put4(uint8_t nyb) {
    sci_putchar("0123456789abcdef"[nyb & 0xf]);
}

void sci_put16(uint16_t val) {
    sci_put4(val >> 12);
    val <<= 4;
    sci_put4(val >> 12);
    val <<= 4;
    sci_put4(val >> 12);
    val <<= 4;
    sci_put4(val >> 12);
}

void sci_put32(uint32_t val) {
    sci_put16(val >> 16);
    sci_put16(val);
}

void sci_putstr(const char *str) {
    char ch;
    while ((ch = *str++))
        sci_putchar(ch);
}
