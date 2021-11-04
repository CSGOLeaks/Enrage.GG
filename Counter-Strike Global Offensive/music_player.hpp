#pragma once
#include "sdk.hpp"
#include <deque>
#include "source.hpp"

class c_music_player {
	//using sound_files_t = std::vector< std::string >;
public:
	virtual void init(void);

	virtual void play(std::size_t file_idx);
	virtual void play(std::string file, float delay_time = 0.6f);

	virtual void stop(void);

	virtual void run(void);

	virtual void load_sound_files(void);

	//sound_files_t	m_sound_files;
	bool			m_playing = false;
private:
	//std::string		m_sound_files_path;
	ConVar* m_voice_loopback = nullptr;
};
