#include "chams.hpp"
#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "math.hpp"
#include "displacement.hpp"
#include "lag_comp.hpp"
#include "resolver.hpp"
#include "usercmd.hpp"
#include "anti_aimbot.hpp"
#include <unordered_map>
#include <algorithm>
#include "menu.hpp"
#include "visuals.hpp"

#include <thread>
#include <cctype>
#include <fstream>

std::vector<c_hit_chams> hit_chams;

void c_chams::set_ignorez(const bool enabled, IMaterial* mat)
{
	if (!csgo.m_engine()->IsInGame() || ctx.m_local() == nullptr || mat == nullptr)
		return;

	if (mat && !mat->IsErrorMaterial()) {
		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, enabled);
		mat->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, enabled);
		mat->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, enabled);
	}
}

void c_chams::set_wireframe(const bool enabled, IMaterial* mat)
{
	if (!csgo.m_engine()->IsInGame() || ctx.m_local() == nullptr || mat == nullptr)
		return;

	if (mat && !mat->IsErrorMaterial())
		mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, enabled);
}

IMaterial* c_chams::get_material(int material) {

	if (!csgo.m_engine()->IsInGame() || ctx.m_local() == nullptr)
		return 0;

	static bool init_materials = false;

	if (!init_materials) {
		std::ofstream(sxor("csgo\\materials\\dsv_textured.vmt")) << R"("VertexLitGeneric"
		{
			"$basetexture"	"vgui/white_additive"
			"$model"		"1"
			"$flat"			"0"
			"$nocull"		"1"
			"$halflambert"	"1"
			"$nofog"		"1"
			"$ignorez"		"0"
			"$znearer"		"0"
			"$wireframe"	"0"
		})";
		std::ofstream(sxor("csgo\\materials\\dsv_flat.vmt")) << R"("UnlitGeneric"
		{
			"$basetexture"	"vgui/white"
			"$model"		"1"
			"$flat"			"1"
			"$nocull"		"1"
			"$selfillum"	"1"
			"$halflambert"	"1"
			"$nofog"		"1"
			"$ignorez"		"0"
			"$znearer"		"0"
			"$wireframe"	"0"
		})";
		std::ofstream(sxor("csgo\\materials\\skeet_glow.vmt")) << R"("VertexLitGeneric" {
		"$basetexture" "models/effects/cube_white"
		"$additive"                    "1"
		"$envmap"                    "models/effects/cube_white"
		"$envmaptint"                "[1.0 1.0. 1.0]"
		"$envmapfresnel"            "1.0"
		"$envmapfresnelminmaxexp"    "[0.0 1.0 2.0]"
		"$alpha"                    "0.99"
		})";

		init_materials = true;
	}

	switch (material) 
	{
		default:
		case 0: /* textured */
			return csgo.m_material_system()->FindMaterial(sxor("dsv_flat"));
		case 1: /* flat */
			return csgo.m_material_system()->FindMaterial(sxor("dsv_textured"));
		case 2: /* glowing */
			return csgo.m_material_system()->FindMaterial(sxor("dev/glow_armsrace"));
	}
}

void InitKeyValues(KeyValues* keyValues, const char* name)
{
	if (!Engine::Displacement::Signatures[c_signatures::INIT_KEY_VALUES]) {
		return;
	}

	auto pfunc = Engine::Displacement::Signatures[c_signatures::INIT_KEY_VALUES];

	__asm
	{
		push name
		mov ecx, keyValues
		call pfunc
	}
}

static Color old_clr[4];
static float old_reflectivity[4];
static float old_pearlescent[4];
static float old_shine[4];
static float old_rim[4];

void c_chams::modifications(IMaterial* mat, c_chams_settings settings)
{
	static KeyValues* kv = nullptr;
	if (!kv) {
		kv = static_cast<KeyValues*>(malloc(36));
		InitKeyValues(kv, sxor("VertexLitGeneric"));
	}
	else {
		static bool something_changed = true;

		Color clr = settings.reflectivity_clr;//ctx.m_settings.chams_reflectivity_c[0];
		float reflectivity = settings.reflectivity;//ctx.m_settings.chams_reflectivity[0];
		float pearlescent = settings.pearlescent;//ctx.m_settings.chams_pearlescent[0] * 0.5f;
		float shine = settings.shine;//ctx.m_settings.chams_shine[0];
		float rim = settings.rim;//ctx.m_settings.chams_rim[0];

		if (old_clr[settings.setting_type] != clr ||
			old_reflectivity[settings.setting_type] != reflectivity ||
			old_pearlescent[settings.setting_type] != pearlescent ||
			old_shine[settings.setting_type] != shine ||
			old_rim[settings.setting_type] != rim
			) {
			something_changed = true;
		}

		if (something_changed) {

			kv->SetString(sxor("$basetexture"), sxor("vgui/white_additive"));

			// reflectivity
			{
				char _envmaptint[64];
				sprintf(_envmaptint, sxor("[%f %f %f]"), (clr.r() / 255) * (reflectivity / 100), (clr.g() / 255) * (reflectivity / 100), (clr.b() / 255) * (reflectivity / 100));

				kv->SetString(sxor("$envmaptint"), _envmaptint);
				kv->SetString(sxor("$envmap"), sxor("env_cubemap"));
			}

			// pearlescence
			{
				char _pearlescent[64];
				sprintf(_pearlescent, sxor("%f"), pearlescent);

				kv->SetString(sxor("$pearlescent"), _pearlescent);
			}

			// shine
			{
				kv->SetInt(sxor("$phong"), 1);
				kv->SetInt(sxor("$phongexponent"), 15);
				kv->SetInt(sxor("$normalmapalphaenvmask"), 1);

				char _phongboost[64];
				sprintf(_phongboost, sxor("[%f %f %f]"), shine / 100, shine / 100, shine / 100);

				kv->SetString(sxor("$phongboost"), _phongboost);
				kv->SetString(sxor("$phongfresnelranges"), sxor("[.5 .5 1]"));
				kv->SetInt(sxor("$BasemapAlphaPhongMask"), 1);
			}

			// rim
			{
				kv->SetInt(sxor("$rimlight"), 1);
				kv->SetInt(sxor("$rimlightexponent"), 2);
				char _rimlightboost[64];
				sprintf(_rimlightboost, sxor("%f"), rim / 100);
				kv->SetString(sxor("$rimlightboost"), _rimlightboost);
			}

			/*{
				if (settings.discoball)
					kv->SetString(sxor("$bumpmap"), sxor("models/weapons/customization/materials/origamil_camo"));
			}*/

			old_clr[settings.setting_type] = clr;
			old_reflectivity[settings.setting_type] = reflectivity;
			old_pearlescent[settings.setting_type] = pearlescent;
			old_shine[settings.setting_type] = shine;
			old_rim[settings.setting_type] = rim;

			//kv->LoadFromBuffer("csgo\\materials\\dsv_textured.vmt", tmp);
			mat->SetShaderAndParams(kv);

			something_changed = false;
		}
	}
}

void c_chams::set_modulation(c_chams_settings settings, IMaterial* material)
{
	if (!csgo.m_engine()->IsInGame() || ctx.m_local() == nullptr || material == nullptr)
		return;

	if (material && !material->IsErrorMaterial())
	{
		material->ColorModulate(settings.color[0]/255.f, settings.color[1]/255.f, settings.color[2]/255.f);

		if (settings.type >= 0) {
			if (settings.type == 0) {
				modifications(material, settings);
			}
			else
			{
				auto envmaptint = material->FindVar(sxor("$envmaptint"), nullptr);

				if (envmaptint)
					envmaptint->SetVecValue(Vector((settings.reflectivity_clr.r() / 255.f), (settings.reflectivity_clr.g() / 255.f), (settings.reflectivity_clr.b() / 255.f)));
			}
		}
		/*auto pearlescent = material->FindVar("$pearlescent", nullptr);

		if (pearlescent) {
			char $pearlescent[64];
			sprintf($pearlescent, "%f", float(ctx.m_settings.chams_pearlescent[3]));

			pearlescent->SetStringValue($pearlescent);
		}*/
	}

	float c[] = { settings.color[0] / 255.f, settings.color[1] / 255.f, settings.color[2] / 255.f };

	csgo.m_render_view()->SetColorModulation(c);
}

void c_chams::modulate_exp(IMaterial* material, const float alpha, const float width)
{
	if (!csgo.m_engine()->IsInGame() || ctx.m_local() == nullptr || material == nullptr)
		return;

	if (!material || material->IsErrorMaterial())
		return;



	const auto transform = material->FindVar(sxor("$envmapfresnelminmaxexp"), 0);

	if (transform)
		transform->SetVectorComponent(width * alpha, 1);
}

void c_chams::player_chams(const std::function<void()> original, c_chams_settings settings, bool scope_blend, bool* vis_type)
{
	bool preserve = ctx.m_settings.menu_chams_type == 3;// conf.preserve_model;
	//bool wireframe = settings.wireframe;// conf.wireframe;

	IMaterial* mat = /*settings.type == 0 ? nullptr : */get_material(/*settings.type*/ctx.m_settings.menu_chams_type == 3 ? 2 : ctx.m_settings.menu_chams_type);

	if (!mat || mat->IsErrorMaterial() || mat == nullptr)
		return;

	mat->IncrementReferenceCount();

	const auto needs_shit = ctx.m_settings.menu_chams_type >= 2;

	//mat->Refresh();

	const float alpha = settings.color.a();

	float old_blend = csgo.m_render_view()->GetBlend();

	const auto first_color = settings.color;
	const auto hidden_color = settings.hidden_color;
	const auto lol = settings.white_color;
	const auto reflect_color = settings.reflectivity_clr;
	const auto type = settings.type;

	auto nigger = settings.type < 2;
	//auto org = mat->GetShaderParams();

	float nigger_alpha = settings.hidden_color.a();

	if (preserve)
	{
		settings.color = (settings.redraw ? first_color : lol);
		settings.reflectivity_clr = lol;
		settings.type = -1;
		set_modulation(settings, mat);
		mat->AlphaModulate(settings.redraw ? (scope_blend ? Math::clamp(ctx.m_settings.local_chams_scope_trans / 100.f, 0, 1) : (alpha / 255.f)) : 0);
		csgo.m_render_view()->SetBlend(settings.redraw ? (scope_blend ? Math::clamp(ctx.m_settings.local_chams_scope_trans / 100.f, 0, 1) : (alpha / 255.f)) : 0);
		original();
		settings.type = type;
		settings.color = first_color;
		settings.reflectivity_clr = reflect_color;
		csgo.m_model_render()->ForcedMaterialOverride(nullptr);
	}

	const float a = (scope_blend ? Math::clamp(2.55f * settings.scope_blend, 0, 255) : alpha);

	if (needs_shit)
		modulate_exp(mat, 1.f);

	mat->SetMaterialVarFlag(MATERIAL_VAR_UNUSED, true);

	if (vis_type != nullptr && !needs_shit)
	{
		if (*vis_type) {
			csgo.m_render_view()->SetBlend((nigger ? nigger_alpha : a) / 255.f);
			set_ignorez(true, mat);
			settings.color = hidden_color;
			set_modulation(settings, mat);
			mat->AlphaModulate((nigger ? nigger_alpha : a) / 255.f);

			settings.color = first_color;
			csgo.m_model_render()->ForcedMaterialOverride(mat);
			original();
		}

		if (*vis_type) {
			csgo.m_render_view()->SetBlend(a / 255.f);
			set_ignorez(false, mat);
			set_modulation(settings, mat);
			mat->AlphaModulate(a / 255.f);

			csgo.m_model_render()->ForcedMaterialOverride(mat);
			original();
		}
	}
	else
	{
		csgo.m_render_view()->SetBlend(a / 255.f);
		set_ignorez(false, mat);
		set_modulation(settings, mat);
		mat->AlphaModulate(a / 255.f);

		csgo.m_model_render()->ForcedMaterialOverride(mat);
		original();
	}

	set_wireframe(false, mat);
	csgo.m_render_view()->SetBlend(old_blend);
}

//void c_chams::player_chams(Menu22::visaul::chams_& conf, c_cs_player* entity, bool draw_fake, bool scope_blend)
//{
//	if (!entity)
//		return;
//
//	if (conf.material_type == 0)
//		return;
//
//	c_color c = c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255);
//	bool preserve = conf.preserve_model;
//	bool pulse = conf.pulse;
//	bool wireframe = conf.wireframe;
//
//	static c_material* pMatGloss = material_system()->find_material("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER); //you can create your own material to really make it shiny, but i like this subtle one
//
//	auto mat = GetMat(conf.material_type - 1);
//
//	if (!mat)
//		return;
//
//	if (!entity->is_alive())
//		return;
//
//	if (!mat->is_error_material())
//	{
//		mat->incrementreferencecount();
//
//		const auto alpha = pulse ? instance()->alpha : c.alpha;
//		if (preserve)
//		{
//
//			modulate(c_color(255, 255, 255, 255), conf, mat);
//			render_view()->set_blend((preserve ? 255 : alpha) / 255.f);
//			entity->draw_model(0x1, 255);
//		}
//
//		modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
//		do_wireframe(mat, wireframe);
//
//		if (conf.xqz)
//		{
//			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
//			set_ignorez(true, mat);
//			render_view()->set_blend(scope_blend ? std::clamp(alpha / 2, 0, 255) : alpha / 255.f);
//			model_render()->forced_material_override(mat);
//			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
//			entity->draw_model(0x1, 255);
//
//			render_view()->set_blend(scope_blend ? std::clamp(alpha / 2, 0, 255) : alpha / 255.f);
//			model_render()->forced_material_override(mat);
//			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
//			entity->draw_model(0x1, 255);
//		}
//		else
//		{
//			set_ignorez(false, mat);
//			modulate(c_color(conf.color[0] * 255, conf.color[1] * 255, conf.color[2] * 255, conf.color[3] * 255), conf, mat);
//			render_view()->set_blend(scope_blend ? std::clamp(alpha / 2, 0, 255) : alpha / 255.f);
//			model_render()->forced_material_override(mat);
//			entity->draw_model(0x1, 255);
//		}
//	}
//}

inline bool is_arms(const char* name) {
	return (strstr(name, ("arms")) != nullptr || strstr(name, ("v_models")) != nullptr);
}

inline bool is_weapon(const char* name) {
	auto is_sleeve = strstr(name, ("sleeve")) != nullptr;
	auto is_arm = strstr(name, ("arms")) != nullptr;
	return (strstr(name, ("weapons/v_")) != nullptr || strstr(name, ("weapons/w_knife")) != nullptr || strstr(name, ("v_")) != nullptr || strstr(name, ("uid")) != nullptr || strstr(name, ("stattrak")) != nullptr) && (strstr(name, ("arms")) == nullptr) /*&& (strstr(name, "knife") == nullptr)*/ && !is_sleeve;
}

int c_chams::work(void* context, void* state, ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld)
{
	auto renderable_entity = (C_BasePlayer*)(((IClientRenderable*)info.pRenderable)->GetIClientUnknown()->GetBaseEntity());

	if (!ctx.m_local() || !csgo.m_engine()->IsInGame() || renderable_entity == nullptr)
		return -1;

	auto client_class = renderable_entity->GetClientClass();

	if (client_class == nullptr || !client_class->m_ClassID)
		return -1;

	const auto classid = client_class->m_ClassID;

	if (/*classid != class_ids::CPredictedViewModel && classid != class_ids::CBaseAnimating &&
		classid != class_ids::CBaseWeaponWorldModel && classid != class_ids::CBreakableProp &&*/
		classid != class_ids::CCSPlayer)
		return -1;

	matrix3x4_t render_matrix[128];

	const auto original = [&]() -> void
	{
		csgo.m_model_render()->DrawModelExecute(context, state, info, pCustomBoneToWorld);
	};
	const auto extra_chams = [&]() -> void
	{
		csgo.m_model_render()->DrawModelExecute(context, state, info, render_matrix);
	};

	Color hecolor = ctx.flt2color(ctx.m_settings.colors_chams_enemy_hidden);
	Color vecolor = ctx.flt2color(ctx.m_settings.colors_chams_enemy_viable);

	//Color htcolor = ctx.m_settings.colors_chams_teammate_hidden;
	//Color vtcolor = ctx.m_settings.colors_chams_teammate_viable;

	Color acolor = ctx.flt2color(ctx.m_settings.colors_chams_local_desync);
	Color lcolor = ctx.flt2color(ctx.m_settings.colors_chams_local);
	 
	Color hwcolor = ctx.m_settings.colors_chams_weapon_hidden;
	Color vwcolor = ctx.m_settings.colors_chams_weapon_viable;

	Color hhcolor = ctx.m_settings.colors_chams_hands_hidden;
	Color vhcolor = ctx.m_settings.colors_chams_hands_viable;

	const auto entity = csgo.m_entity_list()->GetClientEntity(info.entity_index);

	if (classid == class_ids::CCSPlayer && strstr(info.pModel->szName, ("models/player")) != nullptr)
	{
		if (info.entity_index == csgo.m_engine()->GetLocalPlayer() && ctx.m_settings.scope_transparency < 100)
			csgo.m_render_view()->SetBlend(ctx.m_settings.scope_transparency / 100.f);

		if (entity != nullptr && entity->GetClientClass() != nullptr && !entity->m_bIsRagdoll() && !entity->IsDead() && entity->m_iHealth() > 0)
		{
			//if (!c_chams::instance()->second_pass && c_chams::instance()->current_player == entity && !entity->is_local_player())
			//	return;

			float c[] = { 0.f,0.f,0.f };

			csgo.m_render_view()->GetColorModulation(c);

			if (entity != ctx.m_local())
			{
				/*if (c_chams::instance()->current_matrix)
				{
					c_chams::player_chams(original, _config.Visuals.backtrack, false);
				}
				else */if (!entity->IsDormant() && ctx.m_settings.chams_enemy && entity->m_iTeamNum() != ctx.m_local()->m_iTeamNum())
				{
					auto log = &feature::lagcomp->records[entity->entindex() - 1];
					
					//static int o = csgo.m_globals()->tickcount;

					//if (o != csgo.m_globals()->tickcount)
					//{
					//	if (entity->IsDormant() || log->records_count < 2) {
					//		log->oldest_valid_record = nullptr;
					//		log->pre_old_valid_record = nullptr;
					//	}
					//	else {
					//		for (auto i = min(63, int(log->records_count - 1)); i >= 0; i--)
					//		{
					//			if (log->records_count <= 1)
					//				break;

					//			auto r = &log->tick_records[(log->records_count - i)&63];

					//			if (r == nullptr || !r->valid || r->dormant || !r->animated || feature::lagcomp->is_time_delta_too_large(r, true))
					//				continue;

					//			if (i < 1 && !log->oldest_valid_record)
					//				break;

					//			auto next = &log->tick_records[(log->records_count - (i - 1)) & 63];

					//			//if (!first)
					//			//{
					//				log->oldest_valid_record = r;
					//			//	first = true;
					//			//}
					//			//else {
					//				log->pre_old_valid_record = next;
					//				break;
					//			//}
					//		}
					//	}

					//	o = csgo.m_globals()->tickcount;
					//}

					if (ctx.m_settings.chams_backtrack && !ctx.m_local()->IsDead() && log->player == entity && log->oldest_valid_record && log->pre_old_valid_record && (entity->get_abs_origin() - log->oldest_valid_record->origin).LengthSquared() > 10.f)
					{
						static c_chams_settings b_settings;

						b_settings.color = ctx.m_settings.colors_chams_backtrack;
						b_settings.hidden_color = ctx.m_settings.colors_chams_backtrack;
						b_settings.white_color = Color::White();
						b_settings.reflectivity_clr = ctx.m_settings.chams_reflectivity_c_backtrack;
						b_settings.reflectivity = ctx.m_settings.chams_reflectivity_backtrack;
						b_settings.shine = ctx.m_settings.chams_shine_backtrack;
						b_settings.pearlescent = ctx.m_settings.chams_pearlescent_backtrack;
						b_settings.rim = ctx.m_settings.chams_rim_backtrack;
						b_settings.type = ctx.m_settings.chams_material_backtrack;
						b_settings.wireframe = ctx.m_settings.chams_wireframe_backtrack;
						b_settings.redraw = ctx.m_settings.chams_redraw_backtrack;
						b_settings.setting_type = 3;

						//const auto backup_lol = entity->GetBoneCount();
						memcpy(render_matrix, log->oldest_valid_record->matrixes, sizeof(matrix3x4_t) * 128);

						static bool lol[] = { true,true };

						//float time = csgo.m_globals()->curtime;
						//float interval = csgo.m_globals()->interval_per_tick;

						float time = feature::lagcomp->get_interpolated_time();
						
						/*
						float LagCompensation::GetInterpolatedTime(int tickbase_shift)
						{
							int tick_count = g_GlobalVars->tickcount;
							if (g_LocalPlayer)
								tick_count = g_LocalPlayer->m_nFinalPredictedTick();// NETVAR_EX(int, m_nFinalPredictedTick, "DT_BasePlayer", "m_nTickBase", 0x4);

							auto curtime = ((TICKS_TO_TIME(tick_count - tickbase_shift)
								- g_GlobalVars->interval_per_tick) + (g_GlobalVars->interpolation_amount * g_GlobalVars->interval_per_tick))
								- (LagCompensation::_lerp_time + LagCompensation::_latency);

							return curtime;
						}
						*/

						/*
						if (record->render_time >= lerped_render_time && lerped_render_time >= previous_record->render_time)
						{
						  auto v1 = record->render_time - lerped_render_time;
						  auto v2 = record->render_time - previous_record->render_time;
						  auto lerp_time = 1.0f - std::fminf(v1 / v2, 1.0f);
						  auto pos = previous_record->origin + (record->origin - previous_record->origin) * lerp_time;
						}
						*/

						auto v24 = log->oldest_valid_record->animation_time;
						auto v25 = Math::clamp(v24, time - 0.2f, time + 0.2f);
						float interpolation = (time - v24) / (log->pre_old_valid_record->animation_time - v24);
						interpolation = Math::clamp(interpolation, 0, 1);
						Vector lerp = Math::Lerp(log->oldest_valid_record->origin, log->pre_old_valid_record->origin, interpolation);

						const auto distance = (entity->get_abs_origin() - lerp).LengthSquared();

						if (entity->m_bone_count() > 0 && distance > 10)
						{
							const auto alpha = Math::clamp(distance * 0.002f, 0, 1);
							b_settings.color._a() = b_settings.color.malpha(alpha);

							for (size_t i{ }; i < 128; ++i)
							{
								render_matrix[i][0][3] -= log->oldest_valid_record->origin.x;
								render_matrix[i][1][3] -= log->oldest_valid_record->origin.y;
								render_matrix[i][2][3] -= log->oldest_valid_record->origin.z;

								render_matrix[i][0][3] += lerp.x;
								render_matrix[i][1][3] += lerp.y;
								render_matrix[i][2][3] += lerp.z;
							}

							c_chams::player_chams(extra_chams, b_settings, false, lol);
						}

						//entity->GetBoneCount() = backup_lol;

						//return odme;
					}

					static c_chams_settings settings;

					settings.color = vecolor;
					settings.hidden_color = hecolor;
					settings.white_color = Color::White();
					settings.reflectivity_clr = ctx.m_settings.chams_reflectivity_c[0];
					settings.reflectivity = ctx.m_settings.chams_reflectivity[0];
					settings.shine = ctx.m_settings.chams_shine[0];
					settings.pearlescent = ctx.m_settings.chams_pearlescent[0];
					settings.rim = ctx.m_settings.chams_rim[0];
					settings.type = ctx.m_settings.chams_material_type[0];
					settings.wireframe = ctx.m_settings.chams_wireframe[0];
					settings.redraw = ctx.m_settings.chams_redraw[0];
					settings.setting_type = 0;


					c_chams::player_chams(original, settings, false, &ctx.m_settings.chams_walls);
				}
			}
			else
			{
				//static c_chams_settings d_settings;

				//d_settings.color = acolor;
				//d_settings.hidden_color = acolor;
				//d_settings.white_color = Color::White();
				//d_settings.reflectivity_clr = ctx.m_settings.chams_reflectivity_c[3];
				//d_settings.reflectivity = ctx.m_settings.chams_reflectivity[3];
				//d_settings.shine = ctx.m_settings.chams_shine[3];
				//d_settings.pearlescent = ctx.m_settings.chams_pearlescent[3];
				//d_settings.rim = ctx.m_settings.chams_rim[3];
				//d_settings.type = ctx.m_settings.chams_material_type[3];
				//d_settings.wireframe = ctx.m_settings.chams_wireframe[3];
				//d_settings.redraw = ctx.m_settings.chams_redraw[3];
				//d_settings.scope_blend = ctx.m_settings.local_chams_fake_scope_trans;
				//d_settings.setting_type = 3;

				//memcpy(&render_matrix[0], &ctx.fake_matrix[0], sizeof(matrix3x4_t) * 128);

				//auto fake_pos = Vector(ctx.fake_matrix[8][0][3], ctx.fake_matrix[8][1][3], ctx.fake_matrix[8][2][3]);

				//if (ctx.m_settings.chams_local_player && !fake_pos.IsZero() && ctx.m_local()->get_bone_pos(8).DistanceSquared(fake_pos) > 1/*abs(Math::AngleDiff(ctx.angles[ANGLE_REAL], ctx.angles[ANGLE_FAKE])) > 1.f*/ && feature::anti_aim->enable_delay <= csgo.m_globals()->realtime && entity->m_bone_count() <= 128 && entity->m_bone_count() > 0)
				//	c_chams::player_chams(extra_chams, d_settings, (entity->m_bIsScoped() && csgo.m_input()->is_in_tp()));
				
				/*if (_config.Ragebot.enable && _config.Ragebot._antiaim && (_config.Ragebot.fakelag_enable || _config.Ragebot._antiaim) && antiaim->last_fakelag_matrix && local->get_velocity().length2d() > 5 && _config.Visuals.fakelag.material_type != 0)
				{
					const auto weapon = reinterpret_cast<c_base_combat_weapon*>(
						client_entity_list()->get_client_entity_from_handle(c_cs_player::get_local_player()->get_current_weapon_handle()));

					if (!weapon)
						return;

					c_chams::instance()->current_matrix = antiaim->last_fakelag_matrix;

					if (c_chams::instance()->current_matrix && !(_config.Ragebot.disableaaonknife && (weapon_system->get_weapon_data(weapon->get_item_definition())->WeaponType == weapontype_knife))) {
						c_chams::player_chams(original, _config.Visuals.fakelag, true, (entity->is_local_player() && entity->is_scoped() && true));
						original();
					}
					csgo.m_render_view()->set_color_modulation(c_color(255, 255, 255));
					csgo.m_model_render()->forced_material_override(nullptr);
				}*/

				float p[] = { 1.f,1.f,1.f };

				csgo.m_render_view()->SetColorModulation(p);
				csgo.m_model_render()->ForcedMaterialOverride(nullptr);

				static c_chams_settings settings;

				settings.color = lcolor;
				settings.hidden_color = lcolor;
				settings.white_color = Color::White();
				settings.reflectivity_clr = ctx.m_settings.chams_reflectivity_c[2];
				settings.reflectivity = ctx.m_settings.chams_reflectivity[2];
				settings.shine = ctx.m_settings.chams_shine[2];
				settings.pearlescent = ctx.m_settings.chams_pearlescent[2];
				settings.rim = ctx.m_settings.chams_rim[2];
				settings.type = ctx.m_settings.chams_material_type[2];
				settings.wireframe = ctx.m_settings.chams_wireframe[2];
				settings.redraw = ctx.m_settings.chams_redraw[2];
				settings.scope_blend = ctx.m_settings.local_chams_scope_trans;
				settings.setting_type = 2;

				//c_chams::player_chams(_config.Visuals.local, entity, false, (entity->is_local_player() && entity->is_scoped() && _config.Visuals.local.blend && input->camera_in_third_person));
				if (ctx.m_settings.chams_local_player)
					c_chams::player_chams(original, settings, (entity->m_bIsScoped() && csgo.m_input()->is_in_tp()));
			}

			csgo.m_render_view()->SetColorModulation(c);
		}
	//else if (!ctx.m_local()->IsDead() && !csgo.m_input()->is_in_tp()) {

		//bool piska[] = { true,true };

		/*if (is_weapon(info.pModel->szName) && entity && ctx.m_settings.chams_misc[1])
		{
			const auto owner = csgo.m_entity_list()->GetClientEntityFromHandle((CBaseHandle)entity->m_hOwnerEntity());

			static c_chams_settings settings;

			settings.color = vwcolor;
			settings.hidden_color = vwcolor;
			settings.white_color = Color::White();
			settings.reflectivity_clr = ctx.m_settings.chams_reflectivity_c[5];
			settings.reflectivity = ctx.m_settings.chams_reflectivity[5];
			settings.shine = ctx.m_settings.chams_shine[5];
			settings.pearlescent = ctx.m_settings.chams_pearlescent[5];
			settings.rim = ctx.m_settings.chams_rim[5];
			settings.type = ctx.m_settings.chams_material_type[5];
			settings.wireframe = ctx.m_settings.chams_wireframe[5];
			settings.redraw = ctx.m_settings.chams_redraw[5];
			settings.setting_type = 5;

			if (ctx.m_settings.chams_weapon_apply_only_on[0] && strstr(info.pModel->szName, ("weapons/v_knife")) != nullptr
				|| !ctx.m_settings.chams_weapon_apply_only_on[0])
				c_chams::player_chams(original, settings, false);
		}*/
		/*else if (is_arms(info.pModel->szName) && ctx.m_settings.chams_misc[0] && csgo.m_material_system()->FindMaterial(info.pModel->szName, ("Model textures")))
		{
			static c_chams_settings settings;

			settings.color = vhcolor;
			settings.hidden_color = vhcolor;
			settings.white_color = Color::White();
			settings.reflectivity_clr = ctx.m_settings.chams_reflectivity_c[4];
			settings.reflectivity = ctx.m_settings.chams_reflectivity[4];
			settings.shine = ctx.m_settings.chams_shine[4];
			settings.pearlescent = ctx.m_settings.chams_pearlescent[4];
			settings.rim = ctx.m_settings.chams_rim[4];
			settings.type = ctx.m_settings.chams_material_type[4];
			settings.wireframe = ctx.m_settings.chams_wireframe[4];
			settings.redraw = ctx.m_settings.chams_redraw[4];
			settings.setting_type = 4;

			c_chams::player_chams(original, settings, false);
		}*/

	}

	original();
	auto lol = csgo.m_model_render()->DrawModelExecute(context, state, info, pCustomBoneToWorld);
	csgo.m_model_render()->ForcedMaterialOverride(nullptr);

	return lol;
}

bool strstric(const std::string& strHaystack, const std::string& strNeedle)
{
	auto it = std::search(
		strHaystack.begin(), strHaystack.end(),
		strNeedle.begin(), strNeedle.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != strHaystack.end());
}

void c_chams::night_mode()
{
	static auto sv_skyname = csgo.m_engine_cvars()->FindVar(sxor("sv_skyname"));
	static auto original = sv_skyname->GetString();


	const auto reset = [&]()
	{
		csgo.m_engine_cvars()->FindVar(sxor("r_DrawSpecificStaticProp"))->SetValue(1);
		sv_skyname->SetValue(original);

		for (auto i = csgo.m_material_system()->FirstMaterial(); i != csgo.m_material_system()->InvalidMaterial(); i = csgo.m_material_system()->NextMaterial(i))
		{
			auto mat = csgo.m_material_system()->GetMaterial(i);
			if (!mat)
				continue;

			if (mat->IsErrorMaterial())
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();


			if (strstr(tex_name, sxor("World")) || strstr(tex_name, sxor("StaticProp")) || strstr(tex_name, sxor("SkyBox")))
			{
				mat->ColorModulate(1, 1, 1);
				mat->AlphaModulate(1);
			}
		}
	};

	//_(sky_csgo_night02, "sky_csgo_night02");
	const auto set = [&]()
	{
		//static auto load_named_sky = reinterpret_cast<void(__fastcall*)(const char*)>(sig("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
		//load_named_sky(sky_csgo_night02);

		sv_skyname->SetValue(sxor("sky_csgo_night02"));

		csgo.m_engine_cvars()->FindVar(sxor("r_DrawSpecificStaticProp"))->SetValue(0);

		for (auto i = csgo.m_material_system()->FirstMaterial(); i != csgo.m_material_system()->InvalidMaterial(); i = csgo.m_material_system()->NextMaterial(i))
		{
			auto mat = csgo.m_material_system()->GetMaterial(i);
			if (!mat)
				continue;

			if (mat->IsErrorMaterial())
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();

			if (ctx.m_settings.misc_visuals_world_modulation[1] && strstr(tex_name, sxor("World")))
			{
				mat->ColorModulate(0.13f, 0.109f, 0.14f);
			}
			if (strstr(tex_name, sxor("StaticProp")))
			{
				/*if ( !strstric( name, "box" ) && !strstric( name, "crate" ) && !strstric( name, "door" ) && !
					strstric( name, "stoneblock" ) && !strstric( name, "tree" ) && !strstric( name, "flower" ) && !
					strstric( name, "light" ) && !strstric( name, "lamp" ) && !strstric( name, "props_junk" ) && !
					strstric( name, "props_pipe" ) && !strstric( name, "chair" ) && !strstric( name, "furniture" ) && !
					strstric( name, "debris" ) && !strstric( name, "tire" ) && !strstric( name, "refrigerator" ) && !
					strstric( name, "fence" ) && !strstric( name, "pallet" ) && !strstric( name, "barrel" ) && !strstric(
						 name, "wagon" ) && !strstric( name, "wood" ) && !strstric( name, "wall" ) && !strstric( name, "pole" ) && !strstric( name, "props_urban" ) && !strstric( name, "bench" ) && !strstric( name, "trashcan" ) && !strstric( name, "infwll" ) && !strstric( name, "cash_register" ) && !strstric( name, "prop_vehicles" ) && !strstric( name, "rocks" ) && !strstric( name, "artillery" ) && !strstric( name, "plaster_brick" ) && !strstric( name, "props_interiors" ) && !strstric( name, "props_farm" ) && !strstric( name, "props_highway" ) && !strstric( name, "orange" ) && !strstric( name, "wheel" ) )
					continue;*/

				if (ctx.m_settings.misc_visuals_world_modulation[1])
					mat->ColorModulate(0.43f, 0.409f, 0.44f);
				if ( /*!strstric( name, "wood" ) &&*/ !strstric(name, sxor("wall")))
					mat->AlphaModulate(1.f - float(ctx.m_settings.visuals_props_alpha * 0.01f));
			}

			/*if (ctx.m_settings.misc_visuals_nightmode && strstr(tex_name, "SkyBox"))
			{
				mat->ColorModulate(228.f / 255.f, 35.f / 255.f, 157.f / 255.f);
			}*/

		}
	};

	static auto done = true;
	static auto last_setting = false;
	static auto last_transparency = 0;
	static auto was_ingame = false;

	if (!done)
	{
		if (last_setting || last_transparency)
		{
			reset();
			set();
			done = true;
		}
		else
		{
			reset();
			done = true;
		}
	}

	if (was_ingame != csgo.m_engine()->IsInGame() || last_setting != ctx.m_settings.misc_visuals_world_modulation[1] || last_transparency != ctx.m_settings.visuals_props_alpha)
	{
		last_setting = ctx.m_settings.misc_visuals_world_modulation[1];
		last_transparency = ctx.m_settings.visuals_props_alpha;
		was_ingame = csgo.m_engine()->IsInGame();

		done = false;
	}
}