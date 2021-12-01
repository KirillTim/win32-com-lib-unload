#include <iostream>
#include <atlbase.h>
#include <dia2.h>
#include <diacreate.h>
#include <string>

using namespace std;

const static constexpr wchar_t *msdia_dll_w = L"msdia140.dll";
const static constexpr char *msdia_dll = "msdia140.dll";

const char *libdia_path = R"(..\lib\msdia140.dll)";

void report_step(const std::string& step_name) {
    cout << step_name << endl;
    //cout << "Press enter..." << endl;
    //std::string line;
    //getline(cin, line);
}

bool checkLibdiaLoaded() {
    if (GetModuleHandleA(msdia_dll)) {
        std::cout << msdia_dll << " is loaded" << endl;
        return true;
    }
    std::cout << msdia_dll << " is not loaded" << endl;
    return false;
}

bool doFreeLibrary(HMODULE lib) {
    auto res = FreeLibrary(lib);
    std::cout << "FreeLibrary return: " << res;
    if (res == 0) {
        std::cout << ", GetLastError(): " << GetLastError();
    }
    std::cout << endl;
    return res;
}

bool loadLibDia() {
    checkLibdiaLoaded();
    HMODULE libdia = LoadLibraryA(libdia_path);
    if (!libdia) return false;
    std::cout << "LoadLibraryA return: " << libdia << endl;

    HRESULT HR;
    CComPtr<IDiaDataSource> _diaDataSource;
    // TODO: SHOULD WE DO THIS IN NEW CREATED THREAD
    if (FAILED(HR = NoRegCoCreate(msdia_dll_w, CLSID_DiaSource, IID_IDiaDataSource,
                                  reinterpret_cast<LPVOID *>(&_diaDataSource)))) {
        std::cout << "NoRegCoCreate: FAILED" << endl;
        return false;
    }
    std::cout << "NoRegCoCreate: OK" << endl;

    report_step("libdia loaded...");
    checkLibdiaLoaded();

    _diaDataSource.Release();

    CoUninitialize();

    // first time
    doFreeLibrary(libdia);
    checkLibdiaLoaded();

    // TODO WHY DO WE NEED TO CALL THIS TWICE???
    // second time
    doFreeLibrary(libdia);
    checkLibdiaLoaded();

    // module unloaded, FreeLibrary return false, GetLastError(): 126
    doFreeLibrary(libdia);
    checkLibdiaLoaded();

    return true;
}

int main() {
    report_step("process (pid: " + to_string(::_getpid()) + ") started...");

    auto res = loadLibDia();
    std::cout << "result: " << res << endl;

    return 0;
}