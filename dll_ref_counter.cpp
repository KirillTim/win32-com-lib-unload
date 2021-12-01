#include "dll_ref_counter.h"

#include <stdio.h>
#include <wchar.h>

int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {
    //puts("in filter.");
    if (code == EXCEPTION_ACCESS_VIOLATION) {
        //puts("caught AV as expected.");
        return EXCEPTION_EXECUTE_HANDLER;
    }
    else {
        puts("in filter.");
        puts("didn't catch AV, unexpected.");
        return EXCEPTION_CONTINUE_SEARCH;
    };
}

bool DllRefCountReader::init() {
    HMODULE hModule = LoadLibrary((const char *) "NTDLL.dll");

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId()); /* Get current prcess handle */

    auto ZwQueryInformationProcessPtr = (ZwQueryInformationProcess) GetProcAddress(hModule,
                                                                                   "ZwQueryInformationProcess");

    if (ZwQueryInformationProcessPtr) {
        ZwQueryInformationProcessPtr(hProcess, 0, &stProcessBasicInformation, sizeof(stProcessBasicInformation), 0);
    } else {
        return false;
    }

    SIZE_T dwSize = 0;
    bool bStatus;
    /* Get list of loaded DLLs from PEB. */
    bStatus = ReadProcessMemory(hProcess, stProcessBasicInformation.PebBaseAddress, &peb, sizeof(peb), &dwSize);
    if (!bStatus) return false;
    bStatus = ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(peb.Ldr), &peb_ldr_data, sizeof(peb_ldr_data), &dwSize);
    if (!bStatus) return false;
    return true;
}

int DllRefCountReader::getRefCount(const wchar_t* dllName) {
    void *readAddr = (void*) peb_ldr_data.InLoadOrderModuleList.Flink;
    void* startAddr = nullptr;

    SIZE_T dwSize = 0;
    // Go through each modules one by one in their load order.
    while( ReadProcessMemory(hProcess, readAddr, &peb_ldr_module, sizeof(peb_ldr_module), &dwSize) )
    {

        __try{
                // Get the reference count of the DLL
                int loadCount = (signed short)peb_ldr_module.LoadCount;

                if (wcscmp(dllName, peb_ldr_module.BaseDllName.Buffer) == 0) {
                    return loadCount;
                }
                 //std::wcout << "DLL Name: " << peb_ldr_module.BaseDllName.Buffer << std::endl;
                 //std::cout << "DLL Load Count: " << peb_ldr_module.LoadCount << std::endl;
        }__except(filter(GetExceptionCode(), GetExceptionInformation())){
            //outputfile << "DLL Name: " << "No Name Found" << endl;
            //outputfile << "DLL Load Count: " << peb_ldr_module.LoadCount << endl;
            readAddr = (void *) peb_ldr_module.InLoadOrderModuleList.Flink;
            continue;
        }
        readAddr = (void *) peb_ldr_module.InLoadOrderModuleList.Flink;
        if (startAddr == nullptr) {
            startAddr = readAddr;
        } else {
            if (readAddr == startAddr) break;
        }
    }
    return 0;
}
