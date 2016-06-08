#ifndef _ENCODER_H
#define _ENCODER_H

/// @brief Simple interface for the Quadrature encoder.
///
/// The Encoder class provides a simple interface for interfacing between
/// the Parallax Propeller and the quadrature encoder(s).
/// This class continuous polls the light encoder and counts high to low phase transition.
/// To do this uninterrupted this class makes use of on of the Propeller cogs allocated with 250 bytes of memory.
class Encoder{
private:
  int pin1, pin2;
  unsigned int pulseCount;
  int speed;
  int direction;
  // This is the minimum stack size required to for the cog (thread) to run.
  // Don't set it below 250.
  char stack[250];  
  int cogId;
  int side;
  static void run(void* obj);
  
public:
  
  /// @brief Create a new Encoder object.
  ///
  /// The contractor will start the pulse counting on a new cog. Make sure one or more cogs are available.
  /// @param newPin The pin on which the encoder is connected.
  /// @param side The side of the motor. 0 for left, 1 for right.
  explicit Encoder(int newPin, int side);
  
  /// @brief Destroys this object freeing the cog.
  ~Encoder();
  
  /// @brief Get pulse count.
  ///
  /// Get the amount of pulses counted since the creation of this encoder is turning the same way.
  /// It resets when it turn the other way around. 
  /// It also resets when the encoder Speed is Zero for 250 milliseconds. 
  /// @return The pulse count.
  unsigned int getEncoderCount();
  
  /// @brief Get the direction 
  ///
  /// Get the direction which the encoder it turning.
  /// 0 is backwards, 1 is forwards. 
  /// @return the direction.
  int getDirection();
  
  
  /// @brief Get pulse speed.
  ///
  /// Get the amount of pulses counted each second.
  /// Since we use a rotary encoder disk this effectively translates in the rotation speed.
  /// The speed is calculated from a second ago and may lag behind the reality a little.
  /// @return The pulse count last second.
  int getSpeed();
};
  
#endif // _ENCODER_H