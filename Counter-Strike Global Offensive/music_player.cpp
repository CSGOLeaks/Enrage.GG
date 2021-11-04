#include "sdk.hpp"
#include "source.hpp"
#include "music_player.hpp"

void c_music_player::init(void) {
	/// folder the wav files have to be in
	//m_sound_files_path = "D:\\_Users\\Ô\\Desktop\\ExWare\\csgo\\";

	/// create it if it's not there
	//CreateDirectoryA(m_sound_files_path.c_str(), 0);

	/// setting this to 1 let's us hear the music
	m_voice_loopback = csgo.m_engine_cvars()->FindVar("voice_loopback");

	m_playing = false;
	/// find all the sound files in the folder
	//load_sound_files();
}

void voice_record_start(const char* uncompressed_file, const char* decompressed_file, const char* mic_input_file) {
	using voice_record_start_t = void(__fastcall*)(const char*, const char*, const char*);
	//offsets->voice_record_start = util::find_pattern( "engine.dll", "55 8B EC 83 EC 0C 83 3D ? ? ? ? ? 56 57" );
	static auto voice_record_start_fn = reinterpret_cast<voice_record_start_t>(Memory::Scan("engine.dll", "55 8B EC 83 EC 0C 83 3D ? ? ? ? ? 56 57"));

	/*
	char __usercall voice_record_start@<al>(char *a1@<edx>, char *a2@<ecx>, char *a3)
	{
	  char *v3; // esi
	  char *v4; // edi
	  char result; // al
	  int v6; // [esp+8h] [ebp-Ch]
	  int v7; // [esp+Ch] [ebp-8h]
	  int v8; // [esp+10h] [ebp-4h]

	  v3 = a1;
	  v4 = a2;
	  if ( !dword_10887774 )
		return 0;
	  sub_1007F9D0();
	  sub_10080A80();
	  if ( a3 )
	  {
		sub_10084300(a3, (int)a3, (int)&v6, (int)&v7, (int)&v8);
		dword_1088774C = 0;
		dbl_10605CF8 = Plat_FloatTime();
	  }
	  if ( v4 )
	  {
		dword_1088776C = (*(int (__stdcall **)(signed int))(*g_pMemAlloc + 4))(0x100000);
		dword_10887768 = 0;
		dword_10887764 = v4;
	  }
	  if ( v3 )
	  {
		dword_10887760 = (*(int (__stdcall **)(signed int))(*g_pMemAlloc + 4))(0x100000);
		dword_1088775C = 0;
		dword_10887758 = v3;
	  }
	  result = 0;
	  byte_10887770 = 0;
	  if ( dword_10887778 )
	  {
		result = sub_1007F390();
		byte_10887770 = result;
		if ( result )
		{
		  ((void (__thiscall *)(int (__thiscall ***)(int, size_t, int), signed int, _DWORD, signed int))off_10518470[11])(
			&off_10518470,
			-1,
			0,
			1);
		  result = byte_10887770;
		}
	  }
	  return result;
	}

	// being called 3 times.

	char sub_100B0430()
	{
	  char result; // al

	  result = dword_1058BCFC[0];
	  if ( *(_DWORD *)(dword_1058BCFC[0] + 264) == 6 && !byte_10887770 )
		result = voice_record_start(0, 0, 0);
	  return result;
	}
	*/

	__asm {
		push mic_input_file
		mov edx, decompressed_file
		mov ecx, uncompressed_file
		call voice_record_start_fn
		add esp, 0x4
	}
}

void c_music_player::play(std::size_t file_idx) {
	/// is the index valid?
	/*if (file_idx >= m_sound_files.size())
		return;

	/// is the client ingame?
	if (!csgo.m_engine()->IsInGame())
		return;

	/// this makes us hear our own music
	m_voice_loopback->SetValue(1);

	/// use voice chat
	csgo.m_engine()->ExecuteClientCmd("+voicerecord");

	/// the path of the wav file
	auto file = m_sound_files_path + m_sound_files.at(file_idx);

	/// play the wav
	voice_record_start(nullptr, nullptr, file.c_str());*/
}

void c_music_player::play(std::string file,float delay_time) {
	// is the index valid?
	if (file.length() <= 2 || m_playing)
		return;

	// is the client ingame?
	if (!csgo.m_engine()->IsInGame())
		return;

	ctx.time_to_reset_sound = csgo.m_globals()->realtime + delay_time;

	// this makes us hear our own music
	m_voice_loopback->SetValue(1);

	// use voice chat
	csgo.m_engine()->ExecuteClientCmd("+voicerecord");

	// play the wav
	voice_record_start(nullptr, nullptr, file.c_str());

	m_playing = true;
}

void c_music_player::stop(void) {
	// so we don't hear ourself when we use voice chat normally

	if (m_voice_loopback->GetInt() == 0)
		return; // idk

	m_voice_loopback->SetValue(0);

	/// stop using voice chat
	csgo.m_engine()->ExecuteClientCmd("-voicerecord");
	ctx.time_to_reset_sound = 0.f;
	m_playing = false;
}

void c_music_player::run(void) 
{
	/*if (ctx.get_key_press('N')) {
		if (!m_playing)
			play(ctx.m_settings.music_curtrack);
		else
			stop();

		m_playing = !m_playing;
	}*/
}

void c_music_player::load_sound_files(void) {
	/*m_sound_files.clear();

	WIN32_FIND_DATAA data;
	auto file_handle = FindFirstFileA((m_sound_files_path + "*.wav").c_str(), &data);
	if (file_handle == INVALID_HANDLE_VALUE)
		return;

	do {
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		m_sound_files.push_back(data.cFileName);
	} while (FindNextFileA(file_handle, &data));*/
}