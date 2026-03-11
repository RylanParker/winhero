// WINHERO
// Winhero is a open-source windows optimizer and anti-virus
// Runs basic commands to disable telemetry and optimize windows (extremely shallow optimization)
// Scans through files to compare meta-data and whatnot to find malware
// Iterate through running tasks and figur eout disk usage
// Looks for hidden window registry settings to optimize
// Max out refresh rate
// Reboots computer after optimization to finish clean up

// To research
// hidusbf
// TCPOptimizer

#include "commandprompt.cpp"


// TO DO LIST:
// Iteration depth is too shallow 
// Compare metadata and other data to find malware
// Potentialy UI for user convienience

using namespace std;
namespace fs = std::filesystem;
typedef LONG NTSTATUS;
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

RTL_OSVERSIONINFOW GetRealOSVersion() 
{
    // https://learn.microsoft.com/en-us/answers/questions/672988/(c-)-detect-windows-build

    RTL_OSVERSIONINFOW os_info = {0};
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");

    if (hMod) 
    {
        // retrieve operating version
        RtlGetVersionPtr os_raw = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");

        if (os_raw != nullptr) {
            os_info.dwOSVersionInfoSize = sizeof(os_info);
            os_raw(&os_info);
        }
    }

    return os_info;
}



int main() {
    // retrieve windows type
    RTL_OSVERSIONINFOW osVersion = GetRealOSVersion();
    string win_type = "";
//  get_files();
    investigate_tasks();
    
    // swap between the two versions depending on user response
    if (osVersion.dwMajorVersion == 10)
    {
        win_type = "win10commands.txt";
    }
    else if (osVersion.dwMajorVersion == 11)
    {
        win_type = "win11commands.txt";
    }
/* 
    list<string> commands = ReadFile(win_type);

    for (int i = 0; i < commands.size(); i++)
    {
        string t = RunFunction(commands.front());
        cout << t;
        commands.pop_front();
    } */

    return 0;
}
