enum sfxType
{
    SFX_MOVE,
    SFX_SELECT,
    SFX_INTRO,
    SFX_THUD,
    SFX_SANSHIRO
};

extern void playSfx(enum sfxType type, bool activateDelay);
extern void initSfx();
extern void processSfxDelays();
extern void freeSfx();
extern void loadSfx(enum sfxType type);
