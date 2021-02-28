#include <jo/jo.h>
#include <stdint.h>

static struct region_s {
   uint8_t id;
   const char *key;
} regions[] = {
   {1,   "JAPAN"},
   {2,   "TAIWAN and PHILIPINES"},
   {4,   "USA and CANADA"},
   {5,   "BRAZIL"},
   {6,   "KOREA"},
   {10,  "ASIA PAL area"},
   {12,  "EUROPE"},
   {13,  "LATIN AMERICA"},
   {0,   NULL}
};

const char * getRegionString()
{
    jo_smpc_begin_command();
    jo_smpc_write_byte(InputRegister0, 0x01);
    jo_smpc_write_byte(InputRegister1, 0x00);
    jo_smpc_write_byte(InputRegister2, 0xF0);
    jo_smpc_end_command(0xA0);

    uint8_t region = jo_smpc_read_byte(OutputRegister9);
    struct region_s *r;
    for (r=regions; r->id; r++)
        if (r->id == region)
            return r->key;
    return NULL;
}