#pragma once

#include "sdk.hpp"
#include <ostream>
#include <iostream>
#include <string>
#include <future>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>

namespace Engine
{

	class PropManager : public Core::Singleton<PropManager>
	{
	public:
		~PropManager();

	public:
		bool Create(IBaseClientDLL* client);
		void Destroy();

		int GetOffset(const std::string& table_name, const std::string& prop_name);
		RecvVarProxyFn Hook(const RecvVarProxyFn hooked, const std::string& table_name, const std::string& prop_name);
		void DumpProps();

	private:
		int GetProp(const std::string& table_name, const std::string& prop_name, RecvProp** out_prop = nullptr);
		int GetProp(RecvTable* table, const std::string& prop_name, RecvProp** out_prop = nullptr);

		RecvTable* GetTable(const std::string& name);

		void DumpTable(RecvTable* table, int depth);

	private:
		std::vector<RecvTable*> m_pTables;
		std::ofstream m_file;
	};

}