#include "hooked.hpp"
#include "displacement.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "prediction.hpp"
#include "movement.hpp"
#include "rage_aimbot.hpp"
#include "anti_aimbot.hpp"
#include "lag_comp.hpp"
#include <intrin.h>
#include "menu.hpp"
#include "usercmd.hpp"
#include "visuals.hpp"
#include "prop_manager.hpp"
#include "sound_parser.hpp"

struct twoints
{
	int first; int second;
};

std::vector<twoints> g_states;

namespace Hooked
{
	//bool in_ping_spike{}, flipped_state{};
	//
	//float calculate_wanted_ping(INetChannel* channel)
	//{
	//	if (!csgo.m_engine()->GetNetChannelInfo())
	//		return 0.f;
	//
	//	auto wanted_ping = 0.f;
	//
	//	/*if (in_ping_spike)
	//		wanted_ping = (200.f / 1000.f);
	//	else */if (ctx.m_settings.misc_extend_backtrack)
	//		wanted_ping = (200.f / 1000.f) - ctx.lerp_time;
	//	else
	//		return 0.f;
	//
	//	return max(0.f, wanted_ping - csgo.m_engine()->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) * 2.f);
	//}
	
	//void set_suitable_in_sequence(INetChannel* channel)
	//{
	//	if (flipped_state || ctx.exploit_allowed && ctx.m_settings.aimbot_tickbase_exploit > 0)
	//	{
	//		flipped_state = false;
	//		return;
	//	}
	//
	//	const auto spike = TIME_TO_TICKS(calculate_wanted_ping(channel));
	//
	//	if (channel->in_sequence_nr > spike)
	//		channel->in_sequence_nr -= spike;
	//}

	//void flip_state(INetChannel* channel)
	//{
	//	static auto last_reliable_state = -1;
	//
	//	if (channel->in_reliable_state != last_reliable_state)
	//		flipped_state = true;
	//
	//	last_reliable_state = channel->in_reliable_state;
	//}

	//void __fastcall ProcessPacket(INetChannel* channel, uint32_t, void* packet, bool header)
	//{
	//	static auto ofunc = vmt.m_net_channel->VCall<int(__thiscall*)(void*, void*, bool)>(39);
	//	ofunc(channel, packet, header);
	//
	//	if (csgo.m_engine()->IsInGame() && ctx.m_local() && !ctx.m_local()->IsDead()) {
	//		flip_state(channel);
	//
	//		auto v5 = channel->in_sequence_nr;
	//		auto v6 = channel->in_reliable_state;
	//		auto v10 = channel->in_sequence_nr;
	//		auto v11 = v6;
	//	
	//		g_states.emplace_back(twoints{ v10, v6 });
	//	
	//		if (g_states.size() > 0 && (v10 - g_states.back().first) > 128)
	//			g_states.pop_back();
	//	}
	//}

	bool __fastcall ProcessTempEntities(pPastaState* cl_state, void* EDX, void* msg)
	{
		using Fn = bool(__thiscall*)(void*, void*);
		static auto ofunc = vmt.m_clientstate->VCall<Fn>(36);

		auto o_ents = csgo.m_client_state()->m_nMaxClients;
		csgo.m_client_state()->m_nMaxClients = 1;
		const auto ret = ofunc(cl_state, msg);
		csgo.m_client_state()->m_nMaxClients = o_ents;

		csgo.m_engine()->FireEvents();

		return ret;
	}

	void __fastcall PacketEnd(pPastaState* cl_state, void* EDX)
	{
		using Fn = void(__thiscall*)(void*);
		static auto ofunc = vmt.m_clientstate->VCall<Fn>(6);

		if (!csgo.m_engine()->IsInGame()) {
			ctx.m_corrections_data.clear();
			ctx.command_numbers.clear();
			return ofunc(cl_state);
		}

		if (!ctx.m_local() || ctx.m_local()->IsDead()) {
			ctx.m_corrections_data.clear();
			ctx.command_numbers.clear();
			return ofunc(cl_state);
		}

		if (*reinterpret_cast<int*>(uintptr_t(cl_state) + 0x164) == *reinterpret_cast<int*>(uintptr_t(cl_state) + 0x16C)) {
			//const auto command_ack = *reinterpret_cast<int*>(uintptr_t(cl_state) + 0x4D34);
			//constexpr auto m_p_offset = offsetof(CClientState, m_iCommandAck) - 0x4D2C;
			//const auto m_last_server_tick = *reinterpret_cast<int*>(uintptr_t(cl_state) + 0x4D2C);

			feature::sound_parser->get_active_sounds();

			//if (!ctx.m_corrections_data.empty()) {
			//	const auto correct = std::find_if(ctx.m_corrections_data.begin(), ctx.m_corrections_data.end(), [command_ack](const timing_data& a) {
				//	return a.cmd_num == command_ack;
				//	});

				//if (correct != ctx.m_corrections_data.begin() && correct != ctx.m_corrections_data.end()) {

				//	if (csgo.m_client_state()->m_iCommandAck != csgo.m_client_state()->m_iLastCommandAck) {

				//		auto ticks = csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - correct->tick_count;
				//	
				//		if (ticks < 0)
				//			ticks = 0;

				//		if (ticks > ctx.tickrate)
				//			ticks = ctx.tickrate;

				//		ctx.last_4_deltas.emplace_front(ticks);
				//	}
				//	/*else
				//		ctx.last_cmd_delta = 0;*/

				auto lastCommand = (*(int(__thiscall**)(void*))(*(uintptr_t*)csgo.m_engine() + 820))(csgo.m_engine());

				//auto m_latency_arr = &ctx.m_arr_latency[csgo.m_client_state()->m_iCommandAck % 150];

				if (csgo.m_client_state()->m_iCommandAck != lastCommand)
				{
					auto& v85 = Engine::Prediction::Instance()->m_tick_history[lastCommand % 150];

					if (v85.command_num == lastCommand
						&& abs(lastCommand - v85.command_num) <= 150) {
						auto ticks = Math::clamp(csgo.m_engine()->GetTick() - v85.m_tick, 0, 64);
						//ctx.last_cmd_delta = ticks;

						//ctx.m_last_servertick_delta.emplace_front(ticks);

						ctx.last_4_deltas.emplace_front(ticks);// Math::clamp(csgo.m_globals()->tickcount - csgo.m_client_state()->m_clockdrift_manager.m_nServerTick, 0, 64);
					}
				}

				while (ctx.last_4_deltas.size() >= 8)
					ctx.last_4_deltas.pop_back();

				if (ctx.allow_shooting > 0 && lastCommand >= ctx.allow_shooting)
					ctx.allow_shooting = 0;
				//}
			//}
		}

		ofunc(cl_state);

	}

	int __fastcall SendDatagram(void* netchan, void*, void* datagram)
	{
		const auto ofunc = vmt.m_net_channel->VCall<int(__thiscall*)(void*, void*)>(46);

		if (!csgo.m_engine()->IsInGame() || !ctx.m_local())
			return ofunc(netchan, datagram);

		//auto* const net_channel = static_cast<INetChannel*>(netchan);
		/*
		const auto backup_seqnr = net_channel->in_sequence_nr;
		const auto backup_relst = net_channel->in_reliable_state;

		ping_spike(net_channel);

		const auto ret = ofunc(net_channel, datagram);
		net_channel->in_sequence_nr = backup_seqnr;
		net_channel->in_reliable_state = backup_relst;

		return ret;*/

		/*const auto backup_in_seq = net_channel->in_sequence_nr;
		const auto backup_in_rel = net_channel->in_sequence_nr;

		auto net = csgo.m_engine()->GetNetChannelInfo();*/

		//if (!net || !ctx.m_settings.misc_extend_backtrack)
		ctx.in_send_datagram = true;
		//auto pflow = (netflow_t*)(uintptr_t(netchan) + 0x78B * (0) + 0x127);
		/*float old_flow, oldflow_avg;
		if (pflow) {
			old_flow = pflow->latency;
			oldflow_avg = pflow->avglatency;
		}*/
		auto org = ofunc(netchan, datagram);
		/*if (pflow) {
			pflow->latency = old_flow;
			pflow->avglatency = oldflow_avg;
		}*/
		//feature::lagcomp->update_network_info()

		ctx.in_send_datagram = false;
		
		//auto latency_out = net->GetLatency(FLOW_OUTGOING);
		//if (latency_out < 0.2f)
		//{
		//	auto v13 = net_channel->in_sequence_nr - (((0.2f - latency_out) / csgo.m_globals()->interval_per_tick) + 0.5f);
		//	net_channel->in_sequence_nr = v13;

		//	for (auto i = 0; i < g_states.size(); i++)
		//	{
		//		if (g_states[i].first != v13)
		//			continue;

		//		net_channel->in_reliable_state = g_states[i].second;
		//	}
		//}

		////set_suitable_in_sequence(net_channel);

		//const auto original = ofunc(netchan, datagram);
		//net_channel->in_sequence_nr = backup_in_seq;
		//net_channel->in_reliable_state = backup_in_rel;
		return org;
	}

	/*void WriteUsercmd(bf_write* buf, CUserCmd* incmd, CUserCmd* outcmd) {
		using WriteUsercmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);
		static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)Memory::Scan("client.dll", ("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));

		__asm
		{
			mov     ecx, buf
			mov     edx, incmd
			push    outcmd
			call    WriteUsercmdF
			add     esp, 4
		}
	}*/

	//void tickbase_manipulation(CCLCMsg_Move_t* CL_Move, INetChannel* pNetChan) {
	//	// not shifting or dont need do extra fakelag
	//	if (ctx.shift_amount == 0 && (CL_Move->m_nNewCommands != 15 || csgo.m_client_state()->m_iChockedCommands <= 14))
	//		return;
	//
	//	using assign_lol = std::string& (__thiscall*)(void*, uint8_t*, size_t);
	//	auto assign_std_autistic_string = (assign_lol)Memory::Scan("client.dll", "55 8B EC 53 8B 5D 08 56 8B F1 85 DB 74 57 8B 4E 14 83 F9 10 72 04 8B 06 EB 02"); // "55 8B EC 53 8B 5D 08 56 8B F1 85 DB 74 57 8B 4E 14 83 F9 10 72 04 8B 06 EB 02"
	//
	//	// rebuild CL_SendMove
	//	uint8_t data[4000];
	//	bf_write buf;
	//	buf.m_nDataBytes = 4000;
	//	buf.m_nDataBits = 32000;
	//	buf.m_pData = data;
	//	buf.m_iCurBit = false;
	//	buf.m_bOverflow = false;
	//	buf.m_bAssertOnOverflow = false;
	//	buf.m_pDebugName = false;
	//	int numCmd = csgo.m_client_state()->m_iChockedCommands + 1;
	//	int nextCmdNr = csgo.m_client_state()->m_iLastOutgoingCommand + numCmd;
	//	if (numCmd > 62)
	//		numCmd = 62;
	//
	//	bool bOk = true;
	//
	//	auto to = nextCmdNr - numCmd + 1;
	//	auto from = -1;
	//	if (to <= nextCmdNr) {
	//		int newcmdnr = to >= (nextCmdNr - numCmd + 1);
	//		do {
	//			bOk = bOk && csgo.m_client()->WriteUsercmdDeltaToBuffer(0, &buf, from, to, to >= newcmdnr);
	//			from = to++;
	//		} while (to <= nextCmdNr);
	//	}
	//
	//	if (bOk) {
	//		if (ctx.shift_amount > 0) {
	//			CUserCmd from_cmd, to_cmd;
	//			from_cmd = *csgo.m_input()->GetUserCmd(nextCmdNr);
	//			to_cmd = from_cmd;
	//			to_cmd.tick_count = INT_MAX;
	//
	//			do {
	//				if (numCmd >= 62) {
	//					ctx.shift_amount = 0;
	//					break;
	//				}
	//
	//				to_cmd.command_number++;
	//				WriteUsercmd(&buf, &to_cmd, &from_cmd);
	//
	//				ctx.shift_amount--;
	//				numCmd++;
	//			} while (ctx.shift_amount > 0);
	//		}
	//		else {
	//			ctx.shift_amount = 0;
	//		}
	//
	//		// bypass choke limit
	//		CL_Move->m_nNewCommands = numCmd;
	//		CL_Move->m_nBackupCommands = 0;
	//
	//		int curbit = (buf.m_iCurBit + 7) >> 3;
	//		assign_std_autistic_string(CL_Move->m_data, buf.m_pData, curbit);
	//	}
	//}

	void __fastcall PacketStart(void* ecx, void* edx, int incoming_sequence, int outgoing_acknowledged)
	{
		using Fn = void(__thiscall*)(void*, int, int);
		static auto ofunc = vmt.m_clientstate->VCall<Fn>(5);

		//if (ctx.fix_senddatagram)
		//	outgoing_acknowledged = ((pPastaState*)ecx)->m_iLastCommandAck;

		/*if (csgo.m_engine()->IsInGame() && *(int*)(uintptr_t(ecx) + 0x16C) >= 0) {
			for (auto i = 1; i < 64; i++)
			{
				auto ent = csgo.m_entity_list()->GetClientEntity(i);

				if (ent == nullptr ||
					!ent->IsPlayer() ||
					ent->IsDormant() ||
					ent == ctx.m_local())
					continue;

				auto log = &feature::lagcomp->records[ent->entindex() - 1];

				if (log->player != ent && log->player != nullptr)
					continue;

				log->uninterpolated_record.simulation_time = ent->m_flSimulationTime();
			}
		}*/

		/*if (!csgo.m_engine()->IsInGame() || ecx == nullptr) {
			ctx.m_corrections_data.clear();
			ctx.command_numbers.clear();
			return ofunc(ecx, incoming_sequence, outgoing_acknowledged);;
		}

		if (!ctx.m_local() || ctx.m_local()->IsDead()) {
			ctx.m_corrections_data.clear();
			ctx.command_numbers.clear();
			return ofunc(ecx, incoming_sequence, outgoing_acknowledged);
		}*/

		////auto retaddr = _ReturnAddress(); auto mod = GetModuleHandleA("engine.dll");

		//if (!csgo.m_engine()->IsInGame() || !csgo.m_game_rules() || csgo.m_game_rules()->IsValveDS() || !ctx.m_settings.aimbot_position_adjustment
		//	/*|| game_rules->is_freeze_period() || !c_events::is_active_round*/)
		//	return ofunc(ecx, incoming_sequence, outgoing_acknowledged);

		//static int outgoing = outgoing_acknowledged;

		//if (ctx.command_numbers.begin() != ctx.command_numbers.end()) {
		//	for (auto it = ctx.command_numbers.begin(); it != ctx.command_numbers.end();)
		//	{
		//		if (it == ctx.command_numbers.end())
		//			break;

		//		if (*it == outgoing_acknowledged)
		//		{
		//			it = ctx.command_numbers.erase(it);
		//			outgoing = *it;
		//			//ofunc(ecx, incoming_sequence, outgoing_acknowledged);
		//			//return ofunc(ecx, incoming_sequence, outgoing_acknowledged);
		//		}

		//		++it;
		//	}
		//}

		/*if (ctx.fix_senddatagram[outgoing_acknowledged % 150] <= 0) {
			ofunc(ecx,
				incoming_sequence, outgoing_acknowledged);
			return;
		}*/

		if (!ctx.command_numbers.empty() && /*(!csgo.m_game_rules() || !csgo.m_game_rules()->IsValveDS()) &&*/ ctx.m_settings.aimbot_enabled) {
			for (auto it = ctx.command_numbers.rbegin(); it != ctx.command_numbers.rend(); ++it) {
				if (!it->is_outgoing) {
					continue;
				}

				if (it->command_nr == outgoing_acknowledged
					|| outgoing_acknowledged > it->command_nr && (!it->is_used || it->prev_command_nr == outgoing_acknowledged)) {

					it->prev_command_nr = outgoing_acknowledged;
					it->is_used = true;
					ofunc(ecx,
						incoming_sequence, it->command_nr);

					break;
				}
			}

			auto result = false;

			for (auto it = ctx.command_numbers.begin(); it != ctx.command_numbers.end(); ) {
				if (outgoing_acknowledged == it->command_nr || outgoing_acknowledged == it->prev_command_nr)
					result = true;

				if (outgoing_acknowledged > it->command_nr && outgoing_acknowledged > it->prev_command_nr) {
					it = ctx.command_numbers.erase(it);
				}
				else {
					it++;
				}
			}

			if (!result)
				ofunc(ecx,incoming_sequence, outgoing_acknowledged);
		}
		else {
			//if (ctx.command_numbers.size() > 3) {
			//	if (ctx.command_numbers.begin() != ctx.command_numbers.end()) {
			//		for (auto it = ctx.command_numbers.begin(); it != ctx.command_numbers.end();)
			//		{
			//			if (it == ctx.command_numbers.end())
			//				break;

			//			if ((*it).command_nr < outgoing_acknowledged)
			//			{
			//				it = ctx.command_numbers.erase(it);
			//				//outgoing = (*it).command_nr;
			//				//ofunc(ecx, incoming_sequence, outgoing_acknowledged);
			//				//return ofunc(ecx, incoming_sequence, outgoing_acknowledged);
			//			}
			//			else
			//			{
			//				ofunc(ecx,
			//					incoming_sequence, outgoing_acknowledged);
			//			}

			//			++it;
			//		}
			//	}
			//}
			//else
				ofunc(ecx,
					incoming_sequence, outgoing_acknowledged);
		}

		/*static auto outgoing_command = 0;
		if (csgo.m_engine()->IsInGame() && ctx.m_settings.aimbot_position_adjustment && csgo.m_game_rules() && !csgo.m_game_rules()->IsValveDS())
		{
			outgoing_command = outgoing_acknowledged;
			auto v6 = ctx.fix_senddatagram[outgoing_acknowledged % 150];
			auto v7 = 0;
			if (v6 == outgoing_acknowledged)
				v7 = v6 + 1;
			if (v7)
				outgoing_command = v7;
		}
		else
		{
			outgoing_command = outgoing_acknowledged;
		}

		ofunc(ecx, incoming_sequence, outgoing_command);
*/

		//return;
	}

	bool __fastcall SendNetMsg(INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice)
	{
		using Fn = bool(__thiscall*)(INetChannel * pNetChan, INetMessage & msg, bool bForceReliable, bool bVoice);
		const auto ofc = vmt.m_net_channel->VCall<Fn>(40);

		if (ctx.m_local() && ctx.m_local() != nullptr && csgo.m_engine()->IsInGame())
		{
			/*if (msg.GetGroup() == 11 && ctx.m_settings.aimbot_tickbase_exploit && ctx.speed_hack <= 0) {
				uintptr_t uiMsg = (uintptr_t)(&msg);

				tickbase_manipulation((CCLCMsg_Move_t*)&msg, pNetChan);

				return ofc(pNetChan, msg, bForceReliable, bVoice);
			}*/

			if (msg.GetType() == 14) // Return and don't send messsage if its FileCRCCheck
				return true;

			if (msg.GetGroup() == 9) // Fix lag when transmitting voice and fakelagging
				bVoice = true;

			return ofc(pNetChan, msg, bForceReliable, bVoice);
		}

		return ofc(pNetChan, msg, bForceReliable, bVoice);
	}

	void __fastcall Shutdown(INetChannel* pNetChan, void* EDX, const char* reason) {
		using Fn = void(__thiscall*)(INetChannel*, const char*);
		const auto ofc = vmt.m_net_channel->VCall<Fn>(27);

		vmt.m_net_channel.reset();
		vmt.m_net_channel = nullptr;

		return ofc(pNetChan, reason);
	}
}