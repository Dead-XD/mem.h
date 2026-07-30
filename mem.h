#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>

class Mem {
private:
	std::uintptr_t processID = 0;
	void* processHandle = nullptr;

public:
	Mem(const std::string_view processName) noexcept {
		//create a snapshot of all running processes
		::PROCESSENTRY32 entry{};
		entry.dwSize = sizeof(::PROCESSENTRY32);

		const auto processSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //0 to get all processes and TH32CS_SNAPPROCESS to get process information 
		//we used auto but CreateToolhelp32Snapshot returns a HANDLE which is a void* so we can use auto to avoid writing the type explicitly
		//so we need to make sure to close the handle after

		if (processSnapshot != INVALID_HANDLE_VALUE) {
			if (::Process32First(processSnapshot, &entry)) {
				do {
					//find the right process by name
					if (!processName.compare(entry.szExeFile)) {
						processID = entry.th32ProcessID;
						//get full modify access to the process
						processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
						break;
					}
				} while (::Process32Next(processSnapshot, &entry));
			}

			//close the snapshot handle
			::CloseHandle(processSnapshot);
		}
	}

	~Mem() {
		if (processHandle) {
			::CloseHandle(processHandle);
		}
	}

	//get module base address
	const std::uintptr_t getModuleBase(const std::string_view moduleName) const noexcept {

		::MODULEENTRY32 entry{};
		entry.dwSize = sizeof(::MODULEENTRY32);
		const auto moduleSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
		std::uintptr_t moduleBase = 0;

		if (moduleSnapshot != INVALID_HANDLE_VALUE) {
			if (::Module32First(moduleSnapshot, &entry)) {
				do {
					if (!moduleName.compare(entry.szModule)) {
						moduleBase = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
						break;
					}
				} while (::Module32Next(moduleSnapshot, &entry));
			}

			::CloseHandle(moduleSnapshot);
		}

		return moduleBase;
	}

	//Read process memory
	template<typename T> constexpr const T read(const std::uintptr_t& address) const noexcept {

		T value = {};
		::ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), NULL);
		return value;
	}

	//Write process memory
	template<typename T> constexpr void write(const std::uintptr_t& address, const T& value) const noexcept {
		::WriteProcessMemory(processHandle, reinterpret_cast<LPVOID>(address), &value, sizeof(T), NULL);
	}

	//just to test 
	uintptr_t getProcessID() {
		return processID;
	}

};
