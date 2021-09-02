#include <jo/jo.h>
#include "pad_controllers.h"

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

void updateController(int i)
{
    int newDirection = jo_get_input_direction_pressed(i);
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
    updateControllerButtonStatus(&pad_controllers[i].btn_start, jo_is_input_key_pressed(i, JO_KEY_START));
    updateControllerButtonStatus(&pad_controllers[i].btn_a, jo_is_input_key_pressed(i, JO_KEY_A));
    updateControllerButtonStatus(&pad_controllers[i].btn_b, jo_is_input_key_pressed(i, JO_KEY_B));
    updateControllerButtonStatus(&pad_controllers[i].btn_c, jo_is_input_key_pressed(i, JO_KEY_C));
    updateControllerButtonStatus(&pad_controllers[i].btn_x, jo_is_input_key_pressed(i, JO_KEY_X));
    updateControllerButtonStatus(&pad_controllers[i].btn_y, jo_is_input_key_pressed(i, JO_KEY_Y));
    updateControllerButtonStatus(&pad_controllers[i].btn_z, jo_is_input_key_pressed(i, JO_KEY_Z));
    updateControllerButtonStatus(&pad_controllers[i].btn_l, jo_is_input_key_pressed(i, JO_KEY_L));
    updateControllerButtonStatus(&pad_controllers[i].btn_r, jo_is_input_key_pressed(i, JO_KEY_R));

    if((i != controllerMonitor) && 
        (
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