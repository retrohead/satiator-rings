#include <jo/jo.h>
#include "pad_controllers.h"
#include "debug.h"
#include "states/routine_states.h"

#define PORT2 (9)

controllerStateType pad_controllers[JO_INPUT_MAX_DEVICE];
int controllerMonitor = 0;

void initControllers()
{
    for(int i=0;i<JO_INPUT_MAX_DEVICE;i++)
    {
        pad_controllers[i].direction_id = NONE;
        pad_controllers[i].direction_status = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_a = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_b = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_c = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_x = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_y = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_z = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_l = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_r = BUTTON_STATE_IDLE;
        pad_controllers[i].btn_start = BUTTON_STATE_IDLE;
    }
}

void updateControllerButtonStatus(enum controllerButtonStateType *btn, bool is_pressed)
{
    if (is_pressed)
    {
        if((*btn == BUTTON_STATE_NEWPRESS) || (*btn == BUTTON_STATE_HELD))
        {
            *btn = BUTTON_STATE_HELD;
        } else if(*btn == BUTTON_STATE_IDLE)
        {
            *btn = BUTTON_STATE_NEWPRESS;
        }
    } else
    {
        *btn = BUTTON_STATE_IDLE;
    }
}

int get_input_direction_pressed(int port)
{
    Uint16 data = 0;
    if(port >= 6)
        port += PORT2; // port 2 data is offset
    data = Smpc_Peripheral[port].data;
    if((data & PER_DGT_KD)== 0)
    {
        return DOWN;
    }
    else if((data & PER_DGT_KU)== 0)
    {
        return UP;
    }
    // Check horizontal movement
    else if((data & PER_DGT_KR)== 0)
    {
        return RIGHT;
    }
    else if((data & PER_DGT_KL)== 0)
    {
        return LEFT;
    }
    return NONE;
}

void updateController(int i)
{
    int newDirection = get_input_direction_pressed(i);
    if((pad_controllers[i].direction_id == newDirection) && ((pad_controllers[i].direction_status == BUTTON_STATE_NEWPRESS) || (pad_controllers[i].direction_status == BUTTON_STATE_HELD)))
    {
        // same direction pressed, change to held status
        pad_controllers[i].direction_status = BUTTON_STATE_HELD;
    } else if(newDirection == NONE)
    {
        // no direction held
        pad_controllers[i].direction_status = BUTTON_STATE_IDLE;
    } else
    {
        pad_controllers[i].direction_status = BUTTON_STATE_NEWPRESS;
    }
    pad_controllers[i].direction_id = newDirection;

    Uint16 data = 0;
    if(i < 6)
        data = Smpc_Peripheral[i].data;
    else
        data = Smpc_Peripheral[i+PORT2].data; // port 2 data is offset
    updateControllerButtonStatus(&pad_controllers[i].btn_start, ((data & PER_DGT_ST)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_a, ((data & PER_DGT_TA)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_b, ((data & PER_DGT_TB)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_c, ((data & PER_DGT_TC)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_x, ((data & PER_DGT_TX)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_y, ((data & PER_DGT_TY)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_z, ((data & PER_DGT_TZ)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_l, ((data & PER_DGT_TL)== 0));
    updateControllerButtonStatus(&pad_controllers[i].btn_r, ((data & PER_DGT_TR)== 0));

    if((i != controllerMonitor) && 
        (
            (newDirection != NONE) ||
            (pad_controllers[i].btn_start != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_a != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_b != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_c != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_x != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_y != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_z != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_l != BUTTON_STATE_IDLE) ||
            (pad_controllers[i].btn_r != BUTTON_STATE_IDLE)
        )
    )
    {
        controllerMonitor = i;
    }
}

void updateControllers()
{
    for(int i=0;i<JO_INPUT_MAX_DEVICE;i++)
    {
        updateController(i);
    }
}