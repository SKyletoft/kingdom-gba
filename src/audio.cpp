#include "audio.h"

#include "soundbank.h"
#include "soundbank_bin.h"
#include <maxmod.h>

extern "C" {
#include <tonc_irq.h>
}

namespace audio {

void initialise() {
	mmInitDefault((mm_addr)soundbank_bin, 8);
	mmSetVBlankHandler((void *)mmFrame);
}

void play_song(u32 song) { mmStart(song, MM_PLAY_LOOP); }

void play_sfx(u32 sfx) { mmEffect(sfx); }

void play_jingle(u32 jingle) { mmJingle(jingle); }

void set_bgm_volume(u32 volume) { mmSetModuleVolume(volume); }

void set_jingle_volume(u32 volume) { mmSetJingleVolume(volume); }

void pause_bgm() { mmPause(); }

void resume_bgm() { mmResume(); }

void stop_jingle() { play_jingle(0); }
} // namespace audio
