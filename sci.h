#include <stdint.h>

void sci_putchar(uint8_t ch);
void sci_init(void);

void sci_put16(uint16_t val);
void sci_put32(uint32_t val);
void sci_putstr(const char *str);
#define sciLog(...)		do {sprintf(debug_buf, __VA_ARGS__); sci_putstr(debug_buf); } while(0)
