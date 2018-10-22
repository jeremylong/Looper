#pragma once

#include <Audio.h>

class SD_AUDIO_RECORDER : public AudioStream
{
  
public:

  enum class MODE
  {
    PLAY,
    STOP,
    RECORD,
    OVERDUB,
  };

  SD_AUDIO_RECORDER();

  virtual void      update() override;

  MODE              mode() const;
  
  void              play();
  void              play_file( const char* filename, bool loop );
  void              stop();
  void              record();

  void              set_read_position( float t );

private:

  audio_block_t*    m_input_queue_array[1];

  MODE              m_mode;
  const char*       m_play_back_file;

  File              m_recorded_audio_file;
  File              m_play_back_audio_file;
  uint32_t          m_play_back_file_size;
  uint32_t          m_play_back_file_offset;

  uint32_t          m_jump_position;
  bool              m_jump_pending;

  bool              m_looping;
  

  AudioRecordQueue  m_sd_record_queue;      // need to expose these for the AudioConnection

  void              start_recording();
  void              update_recording();
  void              stop_recording();

  bool              start_playing();
  void              update_playing();
  void              stop_playing();
};

