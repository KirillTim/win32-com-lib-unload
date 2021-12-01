#ifndef LIBDIA_UNLOAD_EXAMPLE_DLL_REF_COUNTER_H
#define LIBDIA_UNLOAD_EXAMPLE_DLL_REF_COUNTER_H

#include <iostream>
#include <string>
#include <excpt.h>
#include <fstream>

#include <Windows.h>
#include <winternl.h>

struct _PROCESS_BASIC_INFORMATION_COPY
{
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
};


struct _LDR_MODULE_COPY
{
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID BaseAddress;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashTableEntry;
    ULONG TimeDateStamp;
};


struct _PEB_LDR_DATA_COPY
{
    ULONG Length;
    UCHAR Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID EntryInProgress;
};


typedef ULONG (WINAPI * ZwQueryInformationProcess)( HANDLE ProcessHandle,
                                                    ULONG  ProcessInformationClass,
                                                    PVOID  ProcessInformation,
                                                    ULONG  ProcessInformationLength,
                                                    PULONG ReturnLength );

int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep);


// https://stackoverflow.com/a/36917055
// https://www.securityxploded.com/dllrefcount.php
class DllRefCountReader {
public:
    bool init();

    int getRefCount(const wchar_t* dllName);

private:
    HANDLE hProcess = nullptr;
    _PROCESS_BASIC_INFORMATION_COPY stProcessBasicInformation = {nullptr};
    _PEB_LDR_DATA_COPY peb_ldr_data = {0};
    _LDR_MODULE_COPY peb_ldr_module = {nullptr};
    PEB peb = {0};
};

#endif //LIBDIA_UNLOAD_EXAMPLE_DLL_REF_COUNTER_H
