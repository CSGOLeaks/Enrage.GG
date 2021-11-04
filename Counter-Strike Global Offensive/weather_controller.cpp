#include "player.hpp"
#include "weather_controller.hpp"
#include "Visuals.hpp"
#include "source.hpp"
#include <algorithm>
#include "menu.hpp"
#include "hooked.hpp"
#include "prop_manager.hpp"

void c_weather_controller::find_precipitation()
{
	if (m_precipitation != nullptr && (csgo.m_globals()->tickcount - last_update_tick) < 10) {

		/*if (m_index > 0)
		{
			auto ent = csgo.m_entity_list()->GetClientEntity(m_index);

			if (ent != m_precipitation_ent)
				m_precipitation = nullptr;
		}*/

		return;
	}

	last_update_tick = csgo.m_globals()->tickcount;

	//init
	m_precipitation = nullptr;

	/*int last_classid_searched = -1;

	for (auto i = 64; i < csgo.m_entity_list()->GetHighestEntityIndex(); i++)
	{
		auto ent = csgo.m_entity_list()->GetClientEntity(i);

		if (!ent || !ent->GetClientClass())
			continue;

		auto cc = ent->GetClientClass();

		if (cc->m_ClassID == class_ids::CPrecipitation)
		{
			m_precipitation = ent;
			_events.push_back({ "precipitation updated." });
			break;
		}
		else if (cc->m_ClassID == last_classid_searched)
			continue;

		last_classid_searched = cc->m_ClassID;
	}*/

	static ClientClass* pClass;

	if (!pClass)
		pClass = csgo.m_client()->GetAllClasses();
	while (pClass)
	{
		if (pClass->m_ClassID == class_ids::CPrecipitation)
			break;
		pClass = pClass->m_pNext;
	}

	m_index = csgo.m_entity_list()->GetHighestEntityIndex() + 1;
	int iSerial = RandomInt(0x0, 0xFFF);

	auto m_networkable = pClass->m_pCreateFn(m_index, iSerial);

	m_precipitation = m_networkable->GetIClientUnknown()->GetBaseEntity();

	if (m_precipitation == nullptr)
		_events.push_back({ "precipitation creation failed." });
	else
		_events.push_back({ "precipitation created successfully." });

	auto m_as_entity = (C_BasePlayer*)m_precipitation;

	//update data.
	m_networkable->PreDataUpdate(0);
	m_networkable->OnPreDataChanged(0);

	static auto m_nPrecipType = Engine::PropManager::Instance()->GetOffset("DT_Precipitation","m_nPrecipType");
	static auto m_vecMins = Engine::PropManager::Instance()->GetOffset("DT_BaseEntity","m_vecMins");
	static auto m_vecMaxs = Engine::PropManager::Instance()->GetOffset("DT_BaseEntity","m_vecMaxs");

	*(int*)(uintptr_t(m_as_entity) + m_nPrecipType) = 1;
	//*(float*)(uintptr_t(m_as_entity) + m_nPrecipType - 8) = 0.2f;
	//*(float*)(uintptr_t(m_as_entity) + m_nPrecipType - 32) = 0.2f;
	*(Vector*)(uintptr_t(m_as_entity) + m_vecMaxs) = Vector(32768, 32768, 32768);
	*(Vector*)(uintptr_t(m_as_entity) + m_vecMins) = Vector(-32768, -32768, -32768);

	m_as_entity->OBBMaxs().Set(32768, 32768, 32768);
	m_as_entity->OBBMins().Set(-32768, -32768, -32768);

	//m_as_entity->set_abs_origin((m_as_entity->OBBMaxs() + m_as_entity->OBBMins()) * 0.5f);
	//m_as_entity->m_vecOrigin() = (m_as_entity->OBBMaxs() + m_as_entity->OBBMins()) * 0.5f;

	m_as_entity->on_data_changed(0);
	m_as_entity->post_data_update(0);

	_events.push_back({ "precipitation entity is spawned." });
}