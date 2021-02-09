#include <jo/jo.h>
#include "../sprite_manager.h"
#include "../pad_controllers.h"
#include "routine_states.h"

enum routine_state_types splash_state = ROUTINE_STATE_INITIALIZE;

void logic_splash()
{
    static unsigned short texId[4];
    static unsigned short spriteId[4];
    
    jo_printf(0, 4, "splash_state=%d", splash_state);

    switch(splash_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            texId[0] = load_sprite_texture("ISO/GH", "BOX.TGA", 120, 200);
            texId[1] = load_sprite_texture("ISO/GH", "BOXSMALL.TGA", 64, 100);
            texId[2] = load_sprite_texture("ISO/RS", "BOX.TGA", 160, 160);
            texId[3] = load_sprite_texture("ISO/RS", "BOXSMALL.TGA", 80, 80);
            spriteId[0] = create_sprite(texId[0], 60, 100, 100, 1.0, 1.0, 0);
            spriteId[1] = create_sprite(texId[1], 32 + 130, 50 + 100, 100, 1.0, 1.0, 0);
            spriteId[2] = create_sprite(texId[2], 80, 80 + 100, 100 , 1.0, 1.0, 0);
            spriteId[3] = create_sprite(texId[3], 40 + 90, 40 + 90 + 100, 100, 1.0, 1.0, 0);
            splash_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            if((pad_controllers[0].direction_id == LEFT) && (pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS))
            {

            }
            if((pad_controllers[0].direction_id == RIGHT) && (pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS))
            {

            }
            break;
        case ROUTINE_STATE_END:
            break;
    }
}