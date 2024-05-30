#pragma once

extern "C" {
#include <tonc_types.h>
}

namespace audio {

void initialise();
void play_song(u32);
void play_sfx(u32);
void play_jingle(u32);
void set_bgm_volume(u32);
void set_jingle_volume(u32);
void pause_bgm();
void resume_bgm();
void stop_jingle();

} // namespace audio
