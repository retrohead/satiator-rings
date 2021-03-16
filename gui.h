#include "theme.h"

extern int selectionSprite;
extern int boxTexture;
extern int boxCornerTexture;

extern int load_gui_texture(enum themePalEntryTypes palId);
extern void updateSelectionSprite(int row, bool shortStyle);
extern void createGuiBoxes(char * headerTexture, bool withSelectionSprite);
