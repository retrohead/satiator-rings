#include <jo/jo.h>
#include <stdint.h>
#include <string.h>

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
static  __jo_force_inline jo_extended_ram_cartridge_type      jo_get_extended_ram_cartridge_type(void)
{
    return *((char *)0x24FFFFFF) == 0x5a ? CART_8MBits : *((char *)0x24FFFFFF) == 0x5c ? CART_32MBits : CART_NONE;
}
char * getMemCardString()
{
    static char buf[30];
    strcpy(buf, "No Cartridge");
    // try mounting a save device
    if(!jo_backup_mount(JoCartridgeMemoryBackup))
        return (char *)buf;
    sprintf(buf, "Mem Card %d Blocks", jo_backup_get_total_block_count(JoCartridgeMemoryBackup));
    jo_backup_unmount(JoCartridgeMemoryBackup);
    return (char *)buf;
}
const char * getCartridgeString()
{
    jo_extended_ram_cartridge_type cart = jo_get_extended_ram_cartridge_type();
    switch(cart)
    {
        case CART_8MBits:
            return "1mb Extended RAM";
            break;
        case CART_32MBits:
            return "4mb Extended RAM";
            break;
        case CART_NONE:
        default:
            return getMemCardString();
            break;
    }
}