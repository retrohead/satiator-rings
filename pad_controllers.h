enum controllerButtonStateType
{
    BUTTON_STATE_IDLE,
    BUTTON_STATE_NEWPRESS,
    BUTTON_STATE_HELD
};

typedef struct 
{
    int direction_id;
    enum controllerButtonStateType direction_status;
    enum controllerButtonStateType btn_a;
    enum controllerButtonStateType btn_b;
    enum controllerButtonStateType btn_c;
    enum controllerButtonStateType btn_x;
    enum controllerButtonStateType btn_y;
    enum controllerButtonStateType btn_z;
    enum controllerButtonStateType btn_l;
    enum controllerButtonStateType btn_r;
    enum controllerButtonStateType btn_start;
} controllerStateType;

extern controllerStateType pad_controllers[JO_INPUT_MAX_DEVICE];

extern void initControllers();
extern void updateControllers();