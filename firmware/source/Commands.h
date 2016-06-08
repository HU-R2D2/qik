//Motor control via Qik motor driver
static const unsigned char command_sync       = 0xAA;   //Synchronisation
static const unsigned char command_version    = 0x81;   //Firmware Version
static const unsigned char command_error      = 0x82;   //Error
static const unsigned char command_forward_M0 = 0x88;   //Motor M0 Forward 
static const unsigned char command_reverse_M0 = 0x8A;   //Motor M0 reverse 
static const unsigned char command_brake_M0   = 0x86;   //Motor M0 Brake
static const unsigned char command_forward_M1 = 0x8C;   //Motor M1 Forward 
static const unsigned char command_reverse_M1 = 0x8E;   //Motor M1 reverse
static const unsigned char command_brake_M1   = 0x87;   //Motor M1 Brake

//Rosbee control using Uart
static const unsigned char uart_forward         = 0x30;   //Rosbee Forward 
static const unsigned char uart_backward        = 0x31;   //Rosbee Backward
static const unsigned char uart_rotate_left     = 0x32;   //Rosbee rotate left
static const unsigned char uart_rotate_right    = 0x33;   //Rosbee rotate right
static const unsigned char uart_stop            = 0x34;   //Rosbee stop
static const unsigned char uart_set_speed       = 0x35;   //Rosbee Set Speed
static const unsigned char uart_get_speed       = 0x36;   //Rosbee Get Speed
static const unsigned char uart_version         = 0x37;   //Firmware Version

static const unsigned char uart_ok              = 0x38;   //reply ok
