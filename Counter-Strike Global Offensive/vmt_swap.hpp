#pragma once

#include "auto.hpp"

namespace Horizon::Memory
{

	class VFTableHook {
		VFTableHook(const VFTableHook&) = delete;
	public:

		template<class Type>
		static Type HookManual(uintptr_t* instance, int offset, Type hook)
		{
			DWORD Dummy;
			Type fnOld = (Type)instance[offset];
			VirtualProtect((void*)(instance + offset * 0x4), 0x4, PAGE_EXECUTE_READWRITE, &Dummy);
			instance[offset] = (uintptr_t)hook;
			VirtualProtect((void*)(instance + offset * 0x4), 0x4, Dummy, &Dummy);
			return fnOld;
		}

	private:

		static int Unprotect(void* region)
		{
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery((LPCVOID)region, &mbi, sizeof(mbi));
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);
			return mbi.Protect;
		}

		static void Protect(void* region, int protection)
		{
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery((LPCVOID)region, &mbi, sizeof(mbi));
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, protection, &mbi.Protect);
		}
	};

	class VmtSwap
	{
	public:
		using Shared = std::shared_ptr<VmtSwap>;
	public:
		VmtSwap(const void* instance = nullptr)
		{
			if (instance)
				Create(instance);
			else
			{
				//printf("one of VMT instances is nullptr (Source::%s)\n", __FUNCTION__); //not Win32::Error cause autistic headers errors
			}
		}

	public:
		~VmtSwap()
		{
			Destroy();
		}

	public:
		bool Create(const void* instance)
		{
			m_ppInstance = (std::uintptr_t * *)instance;
			m_pRestore = *m_ppInstance;

			if (!m_pRestore)
				return false;

			while (m_pRestore[m_nSize])
				m_nSize++;

			if (!m_nSize)
				return false;

			m_pReplace = std::make_unique<std::uintptr_t[]>(m_nSize+1);
			//memcpy(m_pReplace.get(), m_pRestore, m_nSize * sizeof(std::uintptr_t));
			memcpy(m_pReplace.get(), &m_pRestore[-1], (m_nSize+1) * sizeof(std::uintptr_t));

			Replace();

			return true;
		}

		void Destroy()
		{
			Restore();

			m_ppInstance = nullptr;
			m_pRestore = nullptr;
			m_pReplace.reset();
			m_nSize = 0u;
			is_hooked = false;
		}

		void Replace()
		{
			if (!this || !m_ppInstance || !m_pReplace)
				return;

			//*m_ppInstance = m_pReplace.get();
			*m_ppInstance = &m_pReplace.get()[1];
			is_hooked = true;
		}

		void Restore()
		{
			if (!this || !m_ppInstance || !m_pRestore)
				return;

			*m_ppInstance = m_pRestore;
			is_hooked = false;
		}

		void Hook(/*const*/ void* hooked, const std::uint32_t index)
		{
			if (!m_pReplace)
				return;

			//m_pReplace[index] = (std::uintptr_t)hooked;
			m_pReplace[index + 1] = (std::uintptr_t)hooked;
			is_hooked = true;
		}

		template<typename T>
		T VCall(const std::size_t index)
		{
			if (!this || (DWORD)this < 0x1000 || index > m_nSize)
				return nullptr;

			return (T)(m_pRestore[index]);
		}

	private:
		std::uintptr_t** m_ppInstance = nullptr;
		std::unique_ptr<std::uintptr_t[]> m_pReplace = nullptr;
		std::uint32_t m_nSize = 0u;
	public:
		bool is_hooked = false;
		std::uintptr_t* m_pRestore = nullptr;
	};

}