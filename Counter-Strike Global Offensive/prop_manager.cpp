#include "prop_manager.hpp"
#include "source.hpp"

#include <iomanip>

namespace Engine
{

	PropManager::~PropManager()
	{
		this->Destroy();
	}

	bool PropManager::Create(IBaseClientDLL* client)
	{
		if (!client)
		{
#ifdef DEBUG
			Win32::Error("IBaseClientDLL is nullptr (PropManager::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		auto entry = client->GetAllClasses();

		if (!entry)
		{
#ifdef DEBUG
			Win32::Error("ClientClass is nullptr (PropManager::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		while (entry)
		{
			auto table = entry->m_pRecvTable;

			if (table)
				this->m_pTables.emplace_back(table);

			entry = entry->m_pNext;
		}

		if (this->m_pTables.empty())
		{
#ifdef DEBUG
			Win32::Error("std::vector<RecvTable*> is empty (PropManager::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		return true;
	}

	void PropManager::Destroy()
	{
		this->m_pTables.clear();
	}

	int PropManager::GetOffset(const std::string& table_name, const std::string& prop_name)
	{
		auto offst = (this->GetProp(table_name, prop_name));

		if (!offst)
		{
#ifndef AUTH
			Win32::Warning("Prop '%s::%s' not found (PropManager::%s)", table_name.c_str(), prop_name.c_str(), __FUNCTION__);
#endif // !AUTH

			return 0;//Win32::Warning("Prop '%s::%s' not found (PropManager::%s)", table_name.c_str(), prop_name.c_str(), __FUNCTION__);
		}

		return offst;
	}

	RecvVarProxyFn PropManager::Hook(const RecvVarProxyFn hooked, const std::string& table_name, const std::string& prop_name)
	{
		RecvProp* prop = nullptr;

		if (!GetProp(table_name, prop_name, &prop))
		{
#ifndef AUTH
			Win32::Warning("Prop '%s::%s' not found (PropManager::%s)", table_name.c_str(), prop_name.c_str(), __FUNCTION__);
#endif // !AUTH
			//Win32::Warning("Prop '%s::%s' not found (PropManager::%s)", table_name.c_str(), prop_name.c_str(), __FUNCTION__);
			return nullptr;
		}

		auto restore = prop->m_ProxyFn;

		prop->m_ProxyFn = hooked;

		return restore;
	}

	int PropManager::GetProp(const std::string& table_name, const std::string& prop_name, RecvProp** out_prop /*= nullptr */)
	{
		return (this->GetProp(this->GetTable(table_name), prop_name, out_prop));
	}

	int PropManager::GetProp(RecvTable* table, const std::string& prop_name, RecvProp** out_prop /*= nullptr */)
	{
		int extra = 0;

		if (!table)
			return 0;

		for (int i = 0; i < table->m_nProps; i++)
		{
			auto prop = &table->m_pProps[i];
			auto child = prop->m_pDataTable;

			if (child && child->m_nProps)
			{
				int add = GetProp(child, prop_name, out_prop);

				if (add)
					extra += (prop->m_Offset + add);
			}

			if (prop_name.compare(prop->m_pVarName) == 0)
			{
				if (out_prop)
					* out_prop = prop;

				return (prop->m_Offset + extra);
			}
		}

		return extra;
	}

	RecvTable* PropManager::GetTable(const std::string& name)
	{
		if (!this->m_pTables.empty())
		{
			for (auto& table : m_pTables)
			{
				if (name.compare(table->m_pNetTableName) == 0)
					return table;
			}
		}


#ifndef AUTH
		Win32::Warning("Prop table '%s' not found (PropManager::%s)", name.c_str(), __FUNCTION__);
#endif // !AUTH
	//	Win32::Warning("Prop table '%s' not found (PropManager::%s)", name.c_str(), __FUNCTION__);
		return nullptr;
	}

	void PropManager::DumpTable(RecvTable* table, int depth)
	{
		std::string pre("");
		for (int i = 0; i < depth; i++)
			pre.append("\t");

		m_file << pre << table->m_pNetTableName << "\n";

		for (int i = 0; i < table->m_nProps; i++)
		{
			RecvProp* prop = &table->m_pProps[i];
			if (!prop) continue;

			std::string varName(prop->m_pVarName);

			if (varName.find(sxor("baseclass")) == 0 || varName.find("0") == 0 || varName.find("1") == 0 || varName.find("2") == 0)
				continue;

			m_file << pre << "\t " << varName << " "
				<< std::setfill('_') << std::setw(60 - varName.length() - (depth * 4))
				<< "[0x" << std::setfill('0') << std::setw(8) << std::hex
				<< std::uppercase << prop->m_Offset << "]\n";

			if (prop->m_pDataTable)
				DumpTable(prop->m_pDataTable, depth + 1);
		}
	}

	void PropManager::DumpProps()
	{
		//m_file.open("C:/sause/csgo.txt");
		//m_file << ("NetVars From Source Engined paste\n\n");
		//m_file << ("aye \n\n");


		for (ClientClass* pClass = csgo.m_client()->GetAllClasses(); pClass != NULL; pClass = pClass->m_pNext)
		{
			RecvTable* table = pClass->m_pRecvTable;
			DumpTable(table, 0);
		}

		m_file.close();
	}

}