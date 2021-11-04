#pragma once

#include "source.hpp"

class c_dormant_esp
{
public:
	// Call before and after ESP.
	virtual void start();
	virtual void get_active_sounds();

	virtual void draw_sounds();
	//void adjustplayer_finish();
	virtual void setup_adjust(C_BasePlayer* player, SndInfo_t& sound);

	virtual bool valid_sound(SndInfo_t& sound);

	struct SoundPlayer
	{
		SoundPlayer(SndInfo_t& sound)
		{
			Override(sound);
		}

		virtual void reset()
		{
			m_iIndex = 0;
			m_iReceiveTime = 0;
			m_anim_time = 0;
			m_vecOrigin = Vector(0, 0, 0);
			/* Restore data */
			m_nFlags = 0;
			player = nullptr;
			m_vecAbsOrigin = Vector(0, 0, 0);
			m_bDormant = false;
		}

		virtual void Override(SndInfo_t& sound) {
			m_iIndex = sound.m_nSoundSource;
			m_vecOrigin = *sound.m_pOrigin;
			m_iReceiveTime = csgo.m_globals()->realtime;
			m_anim_time = feature::lagcomp->get_interpolated_time();
		}

		int m_iIndex = 0;
		float m_iReceiveTime = 0;
		float m_anim_time = 0;
		Vector m_vecOrigin = Vector(0, 0, 0);
		/* Restore data */
		int m_nFlags = 0;
		C_BasePlayer* player = nullptr;
		Vector m_vecAbsOrigin = Vector(0, 0, 0);
		bool m_bDormant = false;
	};

	std::vector<SoundPlayer> m_cSoundPlayers;
	CUtlVector<SndInfo_t> m_utlvecSoundBuffer;
	CUtlVector<SndInfo_t> m_utlCurSoundList;
	std::vector<SoundPlayer> m_arRestorePlayers;
};