#pragma once

#include <limits>

#include <Arduino.h>
#include "CompileSwitches.h"

#ifdef DEBUG_OUTPUT

extern bool serial_port_initialised;

inline bool _assert_fail( const char* assert, const char* msg )
{
  if( serial_port_initialised )
  {
    Serial.print(assert);
    Serial.print(" ");
    Serial.println(msg);
  }
  
  return true;
}

#define ASSERT_MSG(x, msg) ((void)((x) || (_assert_fail(#x,msg))))
#define DEBUG_TEXT(x) if(serial_port_initialised) Serial.print(x);
#define DEBUG_TEXT_LINE(x) if(serial_port_initialised) Serial.println(x);
#define DEBUG_TEXT_LINE_MODE(x, y) if(serial_port_initialised) Serial.println(x, y);
#else
#define ASSERT_MSG(x, msg)
#define DEBUG_TEXT(x)
#define DEBUG_TEXT_LINE(x)
#define DEBUG_TEXT_LINE_MODE(x, y)
#endif

#ifdef SHOW_TIMED_SECTIONS
#define ADD_TIMED_SECTION(x, y) TIMED_SECTION __FILE__##__LINE__(x, y)
#else
#define ADD_TIMED_SECTION(x, y)
#endif

struct TIMED_SECTION
{
  const char* m_section_name;
  uint64_t    m_start_time;
  uint64_t    m_threshold_us;

  TIMED_SECTION( const char* section_name, uint64_t threshold_us ) :
    m_section_name( section_name ),
    m_start_time( micros() ),
    m_threshold_us( threshold_us )
  {      
  }

  ~TIMED_SECTION()
  {
    const uint64_t duration = micros() - m_start_time;
    if( duration > m_threshold_us )
    {
      Serial.print( m_section_name );
      Serial.print(" ");
      Serial.print( static_cast<int>(duration) );
      Serial.println( "us" );
    }
  }
};

/////////////////////////////////////////////////////

template <typename T>
T clamp( const T& value, const T& min, const T& max )
{
  if( value < min )
  {
    return min;
  }
  if( value > max )
  {
    return max;
  }
  return value;
}

template <typename T>
T max_val( const T& v1, const T& v2 )
{
  if( v1 > v2 )
  {
    return v1;
  }
  else
  {
    return v2;
  }
}

template <typename T>
T min_val( const T& v1, const T& v2 )
{
  if( v1 < v2 )
  {
    return v1;
  }
  else
  {
    return v2;
  }
}

template <typename T>
void swap( T& v1, T& v2 )
{
  T temp = v1;
  v1 = v2;
  v2 = temp;
}

/////////////////////////////////////////////////////

template <typename T>
T lerp( const T& v1, const T& v2, float t )
{
  return v1 + ( (v2 - v1) * t );
}

/////////////////////////////////////////////////////

inline constexpr int trunc_to_int( float v )
{
  return static_cast<int>( v );
}

inline constexpr int round_to_int( float v )
{
  return trunc_to_int( v + 0.5f );
}

/////////////////////////////////////////////////////

template < typename TYPE, int CAPACITY >
class RUNNING_AVERAGE
{
  TYPE                    m_values[ CAPACITY ];
  int                     m_current;
  int                     m_size;

public:

  RUNNING_AVERAGE() :
    m_values(),
    m_current(0),
    m_size(0)
  {
  }

  void add( TYPE value )
  {
    m_values[ m_current ] = value;
    m_current             = ( m_current + 1 ) % CAPACITY;
    ++m_size;
    if( m_size > CAPACITY )
    {
      m_size              = CAPACITY;
    }
  }

  void reset()
  {
    m_size                = 0;
    m_current             = 0;
  }
  
  TYPE average() const
  {
    if( m_size == 0 )
    {
      return 0;  
    }
    
    TYPE avg = 0;
    for( int x = 0; x < m_size; ++x )
    {
      avg += m_values[ x ];
    }

    return avg / m_size;
  }

  int size() const
  {
    return m_size;
  }
};

//// AUDIO ////

namespace DSP_UTILS
{

  inline int16_t soft_clip_sample( int16_t sample, float clip_coefficient, bool debug = false )
  {
    // scale input sample to the range [-1,1]
    const float sample_f = static_cast<float>(sample) / static_cast<float>(std::numeric_limits<int16_t>::max());
    ASSERT_MSG( sample_f >= -1.01f && sample_f <= 1.01f, "Soft clip - sample scale error" );
    const float clipped_sample = sample_f - ( clip_coefficient * ( sample_f * sample_f * sample_f ) );

    /*
    if( debug || sample_f < -1.01f || sample_f > 1.01f )
    {
      Serial.print("soft_clip ");
      if( !debug )
      {
        Serial.print( "clip error ");
      }
      Serial.print(sample_f);
      Serial.print(" ");
      Serial.print(clipped_sample);
      Serial.print(" ");
      Serial.println(clip_coefficient);
    }
    */
  
    // scale back to [int16 min, int16 max]
    const int16_t output_sample = round_to_int( clipped_sample * std::numeric_limits<int16_t>::max() );
  
    return output_sample;
  }

}
// DSP_UTILS
