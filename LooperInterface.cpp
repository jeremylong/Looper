#include "LooperInterface.h"
#include "Util.h"

constexpr const int GAIN_POT            = 0;
constexpr const int SATURATION_POT      = 1;
constexpr const int DELAY_MIX_POT       = 2;
constexpr const int DELAY_TIME_POT      = 3;
constexpr const int DELAY_FEEDBACK_POT  = 4;
constexpr const int MIX_POT             = 5;

LOOPER_INTERFACE::LOOPER_INTERFACE() :
  m_dials( { DIAL( A20 ), DIAL( A19 ), DIAL( A18 ), DIAL( A17 ), DIAL( A16 ), DIAL( A13 ) } ),
  m_mode_button( MODE_BUTTON_PIN, false ),
  m_record_button( RECORD_BUTTON_PIN, false ),
  m_leds( { LED( LED_1_PIN, false ), LED( LED_2_PIN, false ), LED( LED_3_PIN, false ) } ),
  m_current_play_back_sample(-1),
  m_num_samples( 0 ),
  m_mode( MODE::LOOPER )
{

}

void LOOPER_INTERFACE::setup( int num_samples )
{
  m_num_samples = num_samples;
  
  m_mode_button.setup();
  m_record_button.setup();

  for( int l = 0; l < NUM_LEDS; ++l )
  {
    m_leds[l].setup();
    m_leds[l].set_brightness( 0.25f );
  }
}

bool LOOPER_INTERFACE::update( ADC& adc, uint32_t time_in_ms )
{
  // read each pot
  for( int d = 0; d < NUM_DIALS; ++d )
  {
    const bool filter = d == DELAY_TIME_POT;
    m_dials[d].update( adc, filter );
  }
  
  m_mode_button.update( time_in_ms );
  m_record_button.update( time_in_ms );

  bool mode_changed = false;
  if( m_mode_button.single_click() )
  {
    m_mode = MODE( ( static_cast<int>(m_mode) + 1 ) % static_cast<int>(MODE::NUM_MODES) );
    mode_changed = true;
    Serial.print("Click mode:");
    Serial.println(static_cast<int>(m_mode));
  }

  for( int l = 0; l < NUM_LEDS; ++l )
  {
    LED& led = m_leds[l];
    if( l < static_cast<int>(MODE::NUM_MODES) )
    {
      led.set_active( l == static_cast<int>(m_mode) );
    }
    
    led.update( time_in_ms );
  }

  return mode_changed;
}

void LOOPER_INTERFACE::set_recording( bool recording, uint32_t time_in_ms )
{
  LED& recording_led = m_leds[ NUM_LEDS - 1 ];
  if( recording )
  {
    constexpr const int RECORD_FLASH_TIME(500);
    recording_led.flash_on( time_in_ms, RECORD_FLASH_TIME );
  }
  else
  {
    recording_led.flash_off();
  }
}

LOOPER_INTERFACE::MODE LOOPER_INTERFACE::mode() const
{
  return m_mode;
}

const BUTTON& LOOPER_INTERFACE::record_button() const
{
  return m_record_button;
}

bool LOOPER_INTERFACE::sample_to_play( int& sample_index )
{
  const int next_sample = round_to_int( m_dials[0].value() * (m_num_samples-1) );

  if( next_sample != m_current_play_back_sample )
  {
    sample_index = next_sample;
    m_current_play_back_sample = next_sample;

    return true;
  }
  
  return false;
}

float LOOPER_INTERFACE::gain() const
{
  // top dial control digital gain reduction
  return m_dials[GAIN_POT].value();
}

float LOOPER_INTERFACE::saturation() const
{
  // saturation value for soft clipping
  return m_dials[SATURATION_POT].value();
}

float LOOPER_INTERFACE::delay_mix() const
{
  return m_dials[DELAY_MIX_POT].value();  
}

float LOOPER_INTERFACE::delay_time() const
{
  return m_dials[DELAY_TIME_POT].value();  
}

float LOOPER_INTERFACE::delay_feedback() const
{
  return m_dials[DELAY_FEEDBACK_POT].value();  
}

float LOOPER_INTERFACE::mix() const
{
  // bottom dial is mix, for consistency with my other modules
  return m_dials[MIX_POT].value();
}


