enum sfxType
{
    SFX_MOVE,
    SFX_SELECT,
    SFX_INTRO,
    SFX_THUD,
    SFX_SANSHIRO,
    SFX_SLIDE,
    SFX_CHANGE,
    SFX_BACK,
    SFX_OPTION,
    /*end*/
    MAX_SFX
};

extern void playSfx(enum sfxType type, bool activateDelay);
extern void initSfx();
extern void processSfxDelays();
extern void freeSfx();
extern void loadSfx(enum sfxType type);
