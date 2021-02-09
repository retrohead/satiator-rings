#include <jo/jo.h>
#include "../sprite_manager.h"
#include "../pad_controllers.h"
#include "routine_states.h"

enum routine_state_types splash_state = ROUTINE_STATE_INITIALIZE;

void logic_splash()
{
    static unsigned short texId = 0;
    static unsigned short texId2 = 0;
    static unsigned short spriteId = 0;
    
    jo_printf(0, 4, "splash_state=%d", splash_state);
    jo_printf(0, 5, "sprite_z=%d", sprites[spriteId].z);
    jo_printf(0, 6, "dir=%d", pad_controllers[0].direction_id);
    jo_printf(0, 7, "dir_status=%d", pad_controllers[0].direction_status);
    switch(splash_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            texId = load_sprite_texture("ISO/GH", "BOX.TGA");
            texId2 = load_sprite_texture("ISO/GH", "BOXSMALL.TGA");
            spriteId = create_sprite(texId, 0, 0, 100, 1.0, 1.0, 0);
            spriteId = create_sprite(texId2, 0, 0, 100, 1.0, 1.0, 0);
            splash_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            if((pad_controllers[0].direction_id == LEFT) && (pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS))
            {
                sprites[spriteId].z--;
            }
            if((pad_controllers[0].direction_id == RIGHT) && (pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS))
            {
                sprites[spriteId].z++;
            }
            break;
        case ROUTINE_STATE_END:
            break;
    }
}