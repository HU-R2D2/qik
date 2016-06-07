#ifndef QIK
#define QIK

class Qik{
public:
  Qik(int transmit_pin, int receive_pin, int baud_rate);

  void execute_command(unsigned char byte);

  unsigned char command_response();
  
  void set_motor_speed(int motor, int speed);
  void set_brake_power(int motor, unsigned char strength);
  
  const static int M0=0;
  const static int M1=1;
private:
  static const int US = 1000 * 1000;

  int transmit_pin;
  int receive_pin;
  int bit_width;
  
  static const int INPUT = 0;
  static const int OUTPUT = 1;
};

#endif