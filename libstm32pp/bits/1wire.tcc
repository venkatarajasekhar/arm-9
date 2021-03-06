
namespace onewire
{
  template<gpio::Address DATA_PORT, u8 DATA_PIN, tim::Address DELAY_TIMER_ADDRESS>
  void Functions<DATA_PORT, DATA_PIN, DELAY_TIMER_ADDRESS>::initializeTimer()
  {
    TIMER::enableClock();
    TIMER::configureBasicCounter(
      tim::cr1::cen::COUNTER_DISABLED,
      tim::cr1::udis::UPDATE_EVENT_ENABLED,
      tim::cr1::urs::UPDATE_REQUEST_SOURCE_OVERFLOW_UNDERFLOW,
      tim::cr1::opm::DONT_STOP_COUNTER_AT_NEXT_UPDATE_EVENT,
      tim::cr1::arpe::AUTO_RELOAD_UNBUFFERED);
    TIMER::setMicroSecondResolution();
  }

  template<gpio::Address DATA_PORT, u8 DATA_PIN, tim::Address DELAY_TIMER_ADDRESS>
  void Functions<DATA_PORT, DATA_PIN, DELAY_TIMER_ADDRESS>::initialize()
  {
    DATA::enableClock();
    DATA::setHigh();

#ifdef STM32F1XX
    DATA::setMode(gpio::cr::GP_OPEN_DRAIN_2MHZ);
#else
    DATA::setPullMode(gpio::pupdr::PULL_UP);
    DATA::setOutputMode(gpio::otyper::OPEN_DRAIN);
    DATA::setMode(gpio::moder::OUTPUT);
#endif
    DATA::setHigh();
  }
  
  template<gpio::Address DATA_PORT, u8 DATA_PIN, tim::Address DELAY_TIMER_ADDRESS>
  bool Functions<DATA_PORT, DATA_PIN, DELAY_TIMER_ADDRESS>::reset()
  {
    DATA::setLow(); TIMER::delay(D_RESET);
    DATA::setHigh(); TIMER::delay(D_RS_WAIT);
    bool i = !DATA::getInput();
    TIMER::delay(D_RS_EPI);
    return i;
  }

  template<gpio::Address DATA_PORT, u8 DATA_PIN, tim::Address DELAY_TIMER_ADDRESS>
  u8 Functions<DATA_PORT, DATA_PIN, DELAY_TIMER_ADDRESS>::readByte()
  {
    u8 byte = 0;

    for (u8 i = 0; i < 8; i++)
    {
      DATA::setLow(); TIMER::delay(1);
      DATA::setHigh(); TIMER::delay(1);
      byte >>= 1;
      byte |= DATA::getInput() ? 0x80 : 0;
      TIMER::delay(D_RW_EPI);
    }

    return byte;
  }

  template<gpio::Address DATA_PORT, u8 DATA_PIN, tim::Address DELAY_TIMER_ADDRESS>
  void Functions<DATA_PORT, DATA_PIN, DELAY_TIMER_ADDRESS>::writeByte(u8 byte)
  {
    for (u8 i = 0; i < 8; i++)
    {
      DATA::setLow(); TIMER::delay(1);
      if (byte & 1) DATA::setHigh();
      TIMER::delay(D_RW_EPI);
      DATA::setHigh(); TIMER::delay(2);
      byte >>= 1;
    }
  }
}
