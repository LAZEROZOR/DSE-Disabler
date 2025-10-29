#include "../memory/memory.h"
#include <iostream>
#include <string>

uintptr_t offset = 0x4e004;

ULONG EnableDSECode = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
ULONG DisableDSECode = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
ULONG UnloadCode = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

struct Request 
{
	PVOID address;
};

HANDLE driverHandle;

bool initDriver()
{
	driverHandle = CreateFileA("\\\\.\\DSEdriver", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (driverHandle == INVALID_HANDLE_VALUE) {
		std::cout << "[!] Failed to open handle to driver: " << GetLastError() << std::endl;
		return false;
	}
	std::cout << "[+] Driver handle opened successfully.\n";
	return true;

}

int main() 
{
	if (!initDriver())
	{
		std::cout << "[-] Failed to initialize driver" << std::endl;
		std::cin.get();
		return 1;
	}
	LPVOID baseAddress = GetCiDllBaseAddress();

	if (!baseAddress)
	{
		std::cout << "[-] CI.dll not found" << std::endl;
		std::cin.get();
		return 1;
	}

	std::cout << "[+] CI.dll base address: " << baseAddress << std::endl;

	LPVOID targetAddress = (LPVOID)((uintptr_t)baseAddress + offset);

	std::cout << "[+] Target address: " << targetAddress << std::endl;

	while (true)
	{
		std::cout << "=============================== \n";
		std::cout << "[1] Disable DSE\n";
		std::cout << "[2] Enable DSE\n";
		std::cout << "[3] Unload driver\n";

		std::string line;
		if (!std::getline(std::cin, line)) break;

		if (line.empty()) continue;

		char c = line[0];

		if (c == '1')
		{
			Request r;
			r.address = targetAddress;
			DeviceIoControl(driverHandle, DisableDSECode, &r, sizeof(r), nullptr, 0, nullptr, nullptr);
			std::cout << "[+] DSE Disabled." << std::endl;
		}
		if (c == '2')
		{
			Request r;
			r.address = targetAddress;
			DeviceIoControl(driverHandle, EnableDSECode, &r, sizeof(r), nullptr, 0, nullptr, nullptr);
			std::cout << "[+] DSE Enabled." << std::endl;
		}
		if (c == '3')
		{
			Request r;
			DeviceIoControl(driverHandle, UnloadCode, &r, sizeof(r), nullptr, 0, nullptr, nullptr);
			std::cout << "[+] Driver unloaded" << std::endl;
			break;
		}
	}

	CloseHandle(driverHandle);

	std::cin.get();
    return 0;
}