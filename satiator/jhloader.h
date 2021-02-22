int boot_disc(void);

// return codes for boot_disc
#define BOOT_BAD_HEADER         -1
#define BOOT_BAD_SECURITY_CODE  -4
#define BOOT_BAD_REGION         -8

#define BOOT_UNRECOGNISED_BIOS  -1024
#define BIOS_BOOT 1
