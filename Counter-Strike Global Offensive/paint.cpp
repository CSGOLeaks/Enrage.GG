#include "hooked.hpp"
#include "menu.hpp"
#include "visuals.hpp"
#include "chams.hpp"
#include "menu/menu/menu_v2.h"
#include "sound_parser.hpp"
#include "grenades.hpp"

#include "hooked.hpp"
#include "displacement.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "prediction.hpp"
#include "movement.hpp"
#include "anti_aimbot.hpp"
#include <intrin.h>
#include "rendering.hpp"

#include "source.hpp"
#include <intrin.h>

using FnPT = void(__thiscall*)(void*, unsigned int, bool, bool);
int ScreenSize2W, ScreenSize2H;

int knife;// = ctx.m_settings.skinchanger_knife;
int skin;// = ctx.m_settings.skinchanger_knife_skin;
bool enabled;// = ctx.m_settings.skinchanger_enabled;
bool chams_weapons;// = ctx.m_settings.chams_misc[1];

namespace Hooked
{

	long __stdcall EndScene(IDirect3DDevice9* device)
	{
		using Fn = long(__stdcall*)(IDirect3DDevice9* device);
		auto original = vmt.m_device->VCall<Fn>(Index::IDirectX::EndScene);

		if (!original)
			return 0;

		static void* allowed_ret_addr = nullptr; if (!allowed_ret_addr) allowed_ret_addr = _ReturnAddress();
		if (allowed_ret_addr != _ReturnAddress())
			return original(device);

		IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
		device->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
		device->GetVertexDeclaration(&vertDec);
		device->GetVertexShader(&vertShader);

		renderer::run(device);

		pixel_state->Apply();
		pixel_state->Release();
		device->SetVertexDeclaration(vertDec);
		device->SetVertexShader(vertShader);

		return original(device);
	}

	long __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp)
	{
		using Fn = long(__stdcall*)(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp);
		auto original = vmt.m_device->VCall<Fn>(Index::IDirectX::Reset);

		if (!original)
			return 0;

		renderer::lost(device);
		auto result = original(device, pp);
		renderer::create(device);

		return result;
	}

	void __fastcall PaintTraverse(void* ecx, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
	{
		static unsigned int scope_panel;
		if (!scope_panel)
		{
			const char* panelname = csgo.m_panel()->GetName(vguiPanel);

			if (!strcmp(panelname, sxor("HudZoom")))
				scope_panel = vguiPanel;
		}

		//if ((std::string(csgo.m_panel()->GetName(vguiPanel)).find("HudZoom") == std::string::npos || !ctx.m_settings.visuals_no_scope) && vmt.m_panel)
		//	vmt.m_panel->VCall<FnPT>(Index::IPanel::PaintTraverse)(ecx, vguiPanel, forceRepaint, allowForce);

		if (vguiPanel == scope_panel && ctx.m_settings.visuals_no_scope)
			return;

		vmt.m_panel->VCall<FnPT>(Index::IPanel::PaintTraverse)(ecx, vguiPanel, forceRepaint, allowForce);

		static unsigned int drawPanel = 0;
		if (!drawPanel)
		{
			const char* panelname = csgo.m_panel()->GetName(vguiPanel);

			if (panelname[0] == 'M' && panelname[2] == 't')
				drawPanel = vguiPanel;
		}

		if (vguiPanel != drawPanel)
			return;

		if (csgo.m_engine()->IsInGame() && ctx.m_local() && !ctx.m_local()->IsDead() && ctx.hud_death_notice) {
			static auto had_notices = false;

			if (ctx.m_settings.misc_preserve_killfeed) {
				float* localDeathNotice = &ctx.hud_death_notice->localplayer_lifetime_mod;
				if (localDeathNotice && *localDeathNotice < 1000.f)* localDeathNotice = FLT_MAX;
				had_notices = true;
			}
			else if (had_notices)
			{
				const auto death_notices = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ctx.hud_death_notice) - 20);

				if (death_notices) {
					const auto ClearDeathNotices = reinterpret_cast<void(__thiscall*)(DWORD)>(Memory::Scan(
						sxor("client.dll"), sxor("55 8B EC 83 EC 0C 53 56 8B 71 58")));

					if (ClearDeathNotices)
						ClearDeathNotices(reinterpret_cast<DWORD>(ctx.hud_death_notice) - 20);

					had_notices = false;
				}
			}
		}

		static bool* s_bOverridePostProcessingDisable = *reinterpret_cast<bool**>(Memory::Scan(sxor("client.dll"), sxor("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 0x2);
		*s_bOverridePostProcessingDisable = (csgo.m_engine()->IsInGame() && ctx.m_local() && ctx.m_settings.visuals_no_postprocess);
	}

	void __stdcall EngineVGUI_Paint(int mode)
	{
		const auto translate_id = [](int wpn) -> int
		{
			for (auto i = 0; i < parser::weapons.list.size(); i++) {
				auto& skin = parser::weapons.list[i];

				if (skin.id == wpn) {
					return i;
				}
			}

			return 0;
		};

		//if (ctx.m_local())
		//{
		//	if (!ctx.m_local()->m_bIsScoped())
		//	{
		//		//if (byte_44719D74)
		//		{
		//			if (!ctx.m_local()->IsDead())
		//			{
		//				auto weapon_info = ctx.latest_weapon_data;
		//				if (weapon_info && *(int*)(uintptr_t(weapon_info) + 0xC8) == 5)
		//					*(int*)(uintptr_t(weapon_info) + 0xC8) = 0;
		//			}
		//		}
		//	}
		//}

		typedef void(__thiscall * Paint_t)(IEngineVGui*, int);
		if (vmt.m_engine_vgui)
			vmt.m_engine_vgui->VCall<Paint_t>(14)(csgo.m_engine_vgui(), mode);

		typedef void(__thiscall * start_drawing)(void*);
		typedef void(__thiscall * finish_drawing)(void*);

		static start_drawing start_draw = reinterpret_cast<start_drawing>(Memory::Scan(sxor("vguimatsurface.dll"), sxor("55 8B EC 83 E4 C0 83 EC 38")));
		static finish_drawing end_draw = reinterpret_cast<finish_drawing>(Memory::Scan(sxor("vguimatsurface.dll"), sxor("8B 0D ? ? ? ? 56 C6 05")));

		if (mode & 1)
		{
			start_draw(csgo.m_surface());
			g_menuinput.pre();

			static int chams_type_menu = -1;

			if (feature::menu->_menu_opened && ctx.m_settings.menu_chams_type != chams_type_menu)
			{
				ctx.m_settings.changing_chams[0] = false;
				ctx.m_settings.changing_chams[1] = false;
				ctx.m_settings.changing_chams[2] = false;
				ctx.m_settings.changing_chams[3] = false;
				ctx.m_settings.changing_chams[4] = false;
				ctx.m_settings.changing_chams[5] = false;
				ctx.m_settings.changing_chams[6] = false;

				switch (ctx.m_settings.menu_chams_type)
				{
				case 0:
					ctx.m_settings.changing_chams[0] = true;
					break;
				case 1:
					ctx.m_settings.changing_chams[1] = true;
					break;
				case 2:
					ctx.m_settings.changing_chams[2] = true;
					break;
				case 3:
					ctx.m_settings.changing_chams[3] = true;
					break;
				case 4:
					ctx.m_settings.changing_chams[4] = true;
					break;
				case 5:
					ctx.m_settings.changing_chams[5] = true;
					break;
				case 6:
					ctx.m_settings.changing_chams[6] = true;
					break;
				}

				chams_type_menu = ctx.m_settings.menu_chams_type;
			}

			static bool bResChange = false;
			static auto prev_time = csgo.m_globals()->framecount;

			if ((csgo.m_globals()->framecount - prev_time) > 5 || ctx.screen_size.Length() == 0.f) {
				csgo.m_engine()->GetScreenSize(ScreenSize2W, ScreenSize2H);
				prev_time = csgo.m_globals()->framecount;
			}

			if (!bResChange && (ScreenSize2W != ctx.screen_size.x
				|| ScreenSize2H != ctx.screen_size.y))
			{
				ctx.screen_size.x = ScreenSize2W;
				ctx.screen_size.y = ScreenSize2H;
				bResChange = true;
			}

			if (bResChange)
				Drawing::CreateFonts();

			//cheat::features::dormant.start();

			if (feature::menu->_menu_opened && csgo.m_globals()->framecount % 2 == 0) {
				knife = ctx.m_settings.skinchanger_knife;
				skin = ctx.m_settings.skinchanger_knife_skin;
				enabled = ctx.m_settings.skinchanger_enabled;
				chams_weapons = ctx.m_settings.chams_misc[1];
			}

			feature::menu->render();
			feature::sound_parser->start();
			feature::visuals->render(bResChange);
			feature::grenades->paint();
			//feature::sound_parser->finish();

			feature::visuals->damage_esp();

			//g_menu.draw();

			if (bResChange) // yes it was important to some of my visuals (manual anti-aim arrows for example) so i guess this code is fine. (NO)
				bResChange = false;

			//g_menuinput.think();

			if (feature::menu->_menu_opened && csgo.m_globals()->framecount % 2 == 1)
			{
				if (ctx.m_settings.chams_misc[1] != chams_weapons || ctx.m_settings.skinchanger_enabled != enabled)
					ctx.updated_skin = true;

				if (knife != ctx.m_settings.skinchanger_knife)
				{
					const auto cknife = translate_id(parser::knifes.list[ctx.m_settings.skinchanger_knife].id);
					ctx.skins.clear();
					for (auto i = 0; i < parser::weapons.list[cknife].skins.list.size(); i++)
					{
						auto skins = parser::weapons.list[cknife].skins.list.data()[i];
						std::string p = std::string(skins.translated_name.begin(), skins.translated_name.end());
						ctx.skins.push_back(p);
					}
					ctx.m_settings.skinchanger_knife_skin = Math::clamp(ctx.m_settings.skinchanger_knife_skin, 0, (int)ctx.skins.size() - 1);
				}

				//static auto tick = csgo.m_globals()->tickcount;

				//if (csgo.m_globals()->tickcount - tick > 20)
				//{
				//	static auto s = std::string(sxor("C:\\2k17_v2\\sounds\\"));
				//	static auto r = std::string(sxor("read tutorial on forums"));
				//	static auto wa = std::string(sxor("*.wav"));
				//	//#ifndef AUTH
				//	//				static auto n = std::string(sxor("no objects found"));
				//	//				static auto lu = std::string(sxor("*.lua"));
				//	//				static auto fold = std::string(sxor("C:\\2k17_v2\\"));
				//	//				ctx.lua_scripts_count = ctx.get_all_files_names_within_folder(fold.c_str(), lu.c_str());
				//	//
				//	//				if (ctx.lua_scripts_count.size() < 1)
				//	//					ctx.lua_scripts_count.push_back(n.c_str());
				//	//#endif // !AUTH
				//	ctx.music_found = ctx.get_all_files_names_within_folder(s, wa);
				//	//if (vec.size() > 0 && vec.size() < 1000)

				//	if (ctx.music_found.empty())
				//		ctx.music_found.emplace_back(r);

				//	tick = csgo.m_globals()->tickcount;
				//}

				if (knife != ctx.m_settings.skinchanger_knife || skin != ctx.m_settings.skinchanger_knife_skin)
					ctx.updated_skin = true;
			}

			end_draw(csgo.m_surface());
		}
	}

	void __fastcall SceneEnd(void* ecx, void* edx)
	{
		if (vmt.m_render_view)
			vmt.m_render_view->VCall<void(__thiscall*)(void*)>(9)(ecx);
		else
			return;

		if (ctx.m_settings.esp_glow && csgo.m_engine()->IsConnected() && csgo.m_engine()->IsInGame() && ctx.m_local() && csgo.m_glow_object() != nullptr && csgo.m_glow_object()->GetSize() > 0)
		{
			for (auto i = 0; i < csgo.m_glow_object()->GetSize(); i++)
			{
				auto& glowObject = csgo.m_glow_object()->m_GlowObjectDefinitions[i];
				C_BasePlayer* entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

				if (!entity || entity->IsDead() || entity->IsDormant())
					continue;

				if (glowObject.IsUnused())
					continue;

				if (!entity->GetClientClass() || entity->GetClientClass()->m_ClassID != class_ids::CCSPlayer)
					continue;

				const bool is_local_player = entity == ctx.m_local();
				//bool is_teammate = ctx.m_local()->m_iTeamNum() == entity->m_iTeamNum() && !is_local_player;
				const bool is_enemy = ctx.m_local()->m_iTeamNum() != entity->m_iTeamNum() || is_local_player;

				/*if (is_local_player){
					if (!ctx.m_settings.esp_glow[2]) 
						continue;
				}
				else {
					if (is_enemy && !ctx.m_settings.esp_glow[0])
						continue;
					else */if (!is_enemy)
						continue;
				//}

				Color color = ctx.flt2color(ctx.m_settings.colors_glow_enemy);

				if (ctx.m_settings.colors_glow_hp)
				{
					auto hp = Math::clamp(entity->m_iHealth(), 0, 100);

					color._r() = 255 - (hp * 2.55f);
					color._g() = hp * 2.55f;
				}

				if (is_local_player)
					color = ctx.m_settings.colors_glow_local;

				glowObject.m_nGlowStyle = 0;
				glowObject.m_bFullBloomRender = false;
				glowObject.m_flRed = color.r() / 255.0f;
				glowObject.m_flGreen = color.g() / 255.0f;
				glowObject.m_flBlue = color.b() / 255.0f;
				glowObject.m_flAlpha = color.a() / 255.0f;
				glowObject.m_bRenderWhenOccluded = true;
				glowObject.m_bRenderWhenUnoccluded = false;
			}
		}

		////--- Wireframe Smoke ---//
		//static std::vector<const char*> vistasmoke_wireframe =
		//{
		//	"particle/vistasmokev1/vistasmokev1_smokegrenade",
		//};

		//static std::vector<const char*> vistasmoke_nodraw =
		//{
		//	"particle/vistasmokev1/vistasmokev1_fire",
		//	"particle/vistasmokev1/vistasmokev1_emods",
		//	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		//};

		//static bool old_value = false;

		//if (ctx.m_settings.visuals_no_flashsmoke) {
		//	for (auto mat_s : vistasmoke_wireframe)
		//	{
		//		IMaterial* mat = csgo.m_material_system()->FindMaterial(mat_s, "Other Textures");
		//		mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true); //wireframe
		//	}

		//	for (auto mat_n : vistasmoke_nodraw)
		//	{
		//		IMaterial* mat = csgo.m_material_system()->FindMaterial(mat_n, "Other Textures");
		//		mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		//	}

		//	old_value = true;
		//}
		//else if (old_value)
		//{
		//	for (const auto* mat_s : vistasmoke_wireframe)
		//	{
		//		IMaterial* mat = csgo.m_material_system()->FindMaterial(mat_s, "Other Textures");
		//		mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false); //wireframe
		//	}

		//	for (const auto* mat_n : vistasmoke_nodraw)
		//	{
		//		IMaterial* mat = csgo.m_material_system()->FindMaterial(mat_n, "Other Textures");
		//		mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
		//	}
		//}

		/*static IMaterial* xblur_mat = csgo.m_material_system()->FindMaterial("dev/blurfilterx_nohdr", 0);
		static IMaterial* yblur_mat = csgo.m_material_system()->FindMaterial("dev/blurfiltery_nohdr", 0);
		static IMaterial* scope = csgo.m_material_system()->FindMaterial("dev/scope_bluroverlay", 0);

		if (xblur_mat && yblur_mat && scope && !scope->IsErrorMaterial() && !yblur_mat->IsErrorMaterial() && !xblur_mat->IsErrorMaterial()) {
			xblur_mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			yblur_mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			scope->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}*/
		//feature::chams->work();
	}

	int __fastcall ListLeavesInBox(void* bsp, void* edx, Vector& mins, Vector& maxs, unsigned short* pList, int listMax) {
		typedef int(__thiscall * ListLeavesInBox)(void*, const Vector&, const Vector&, unsigned short*, int);
		static auto ofunc = vmt.m_bsp_tree_query->VCall< ListLeavesInBox >(6);

		// occulusion getting updated on player movement/angle change,
		// in RecomputeRenderableLeaves ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L674 );
		// check for return in CClientLeafSystem::InsertIntoTree

		static auto list_leaves = (void*)(Memory::Scan("client.dll", "56 52 FF 50 18") + 5);
		if (!csgo.m_engine()->IsInGame() || _ReturnAddress() != list_leaves) // 89 44 24 14 ( 0x14244489 ) - new / 8B 7D 08 8B ( 0x8B087D8B ) - old
			return ofunc(bsp, mins, maxs, pList, listMax);

		// get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
		auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);
		if (!info || !info->m_pRenderable || !info->m_pRenderable->GetIClientUnknown())
			return ofunc(bsp, mins, maxs, pList, listMax);

		// check if disabling occulusion for players ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1491 )
		auto base_entity = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();
		if (!base_entity || !base_entity->IsPlayer())
			return ofunc(bsp, mins, maxs, pList, listMax);

		// fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
		// AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
		info->m_Flags &= ~0x100;
		info->m_Flags2 |= 0xC0;

		// extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
		static const Vector map_min = Vector(-16384.0f, -16384.0f, -16384.0f);
		static const Vector map_max = Vector(16384.0f, 16384.0f, 16384.0f);
		const auto count = ofunc(bsp, map_min, map_max, pList, listMax);
		return count;
	}

	bool is_sleeve(IClientEntity* ent, const char* name) {

		if (ent == nullptr)
			return false;

		auto client_class = ent->GetClientClass();

		if (client_class == nullptr || !client_class->m_ClassID)
			return false;

		const auto classid = client_class->m_ClassID;

		if (classid != class_ids::CPredictedViewModel)
			return false;

		return (strstr(name, ("sleeve")) != nullptr);
	}

	bool is_arms(IClientEntity* ent,const char* name) {

		if (ent == nullptr)
			return false;

		auto client_class = ent->GetClientClass();

		if (client_class == nullptr || !client_class->m_ClassID)
			return false;

		const auto classid = client_class->m_ClassID;

		if (classid != class_ids::CPredictedViewModel)
			return false;

		return (strstr(name, ("arms")) != nullptr || strstr(name, ("v_models")) != nullptr);
	}

	bool is_weapon(IClientEntity* ent, const char* name) {

		if (ent == nullptr)
			return false;

		auto client_class = ent->GetClientClass();

		if (client_class == nullptr || !client_class->m_ClassID)
			return false;

		const auto classid = client_class->m_ClassID;

		if (classid != class_ids::CBaseWeaponWorldModel)
			return false;

		const auto is_sleeve = strstr(name, ("sleeve")) != nullptr;
		auto is_arm = strstr(name, ("arms")) != nullptr;
		return (strstr(name, ("weapons/v_")) != nullptr || strstr(name, ("v_")) != nullptr || strstr(name, ("uid")) != nullptr || strstr(name, ("stattrak")) != nullptr) && (strstr(name, ("arms")) == nullptr) /*&& (strstr(name, "knife") == nullptr)*/ && !is_sleeve;
	}

	int __stdcall DrawModelExecute(void* context, void* state, ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld)
	{
		vmt.m_model_render->Restore();

		auto* const model_ent = csgo.m_entity_list()->GetClientEntity(info.entity_index);

		bool is_player = model_ent != nullptr && model_ent->entindex() < 64 && model_ent->IsPlayer();

		int bRet;

		if (!csgo.m_engine()->IsInGame() || info.pModel == nullptr || ctx.m_local() == nullptr || info.pRenderable == nullptr || *(void**)info.pRenderable == nullptr || csgo.m_model_render()->IsForcedMaterialOverride() && (is_player || !is_arms(model_ent, info.pModel->szName) && !is_weapon(model_ent, info.pModel->szName))/*&& strstr(info.pModel->szName, "arms") == nullptr && strstr(info.pModel->szName, ("sleeve")) == nullptr*//*&& strstr(info.pModel->szName, ("models/player")) != nullptr*/)
			bRet = csgo.m_model_render()->DrawModelExecute(context, state, info, pCustomBoneToWorld);
		else if (ctx.m_settings.visuals_no_teammates && is_player && model_ent != ctx.m_local() && model_ent->m_iTeamNum() == ctx.m_local()->m_iTeamNum() && strstr(info.pModel->szName, ("models/player")) != nullptr)
			bRet = 0; // skip teammates
		else if (ctx.m_settings.visuals_no_sleeves && is_sleeve(model_ent, info.pModel->szName))
			bRet = 0; // skip sleeves
		//else if (strstr(info.pModel->szName, ("shadow")) != nullptr && ctx.fps < (1.0f / csgo.m_globals()->interval_per_tick))// && cheat::Cvars.Visuals_rem_sleeves.GetValue())
		//	bRet = 0; // skip shadows
		else if (auto bred = feature::chams->work(context, state, info, pCustomBoneToWorld); bred != -1)
			bRet = bred;
		else
			bRet = csgo.m_model_render()->DrawModelExecute(context, state, info, pCustomBoneToWorld);

		vmt.m_model_render->Replace();
		return bRet;
	}

}