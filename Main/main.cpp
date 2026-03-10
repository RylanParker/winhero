#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <Windows.h>

using namespace std;

typedef LONG NTSTATUS;
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

RTL_OSVERSIONINFOW GetRealOSVersion() 
{
    // https://learn.microsoft.com/en-us/answers/questions/672988/(c-)-detect-windows-build

    RTL_OSVERSIONINFOW rovi = {0};
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    if (hMod) {
        RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
        if (fxPtr != nullptr) {
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            fxPtr(&rovi);
        }
    }
    return rovi;
}

list<string> read_file(string file_name)
{
    // Reads each line in the file, appends them to a file and then returns them as a list of strings
    list<string> results = {};
    int iter = 0;
    string line;
    ifstream command_file(file_name);
    
    while (getline (command_file, line)) {
        results.push_back(line);
    }

    // close the file
    command_file.close();

    return results;
}

string run_function(string command)
{
    // Runs a command and then returns the output from said command
    
    // Runs the command
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        return "Error running command: " + command;
    }

    char buffer[128];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) { // Reads each line and returns null when completed
        result += buffer;
    }
    
    // Frees resources
    pclose(pipe);

    // Return the result
    return result;
}

int main() {
    // retrieve windows type
    RTL_OSVERSIONINFOW osVersion = GetRealOSVersion();
    string win_type = "";
    
    // swap between the two versions depending on user response
    if (osVersion.dwMajorVersion == 10)
    {
        win_type = "win10commands.txt";
    }
    else if (osVersion.dwMajorVersion == 11)
    {
        win_type = "win11commands.txt";
    }

    cout << win_type;

    list<string> commands = read_file(win_type);

    for (int i = 0; i < commands.size(); i++)
    {
        string t = run_function(commands.front());
        cout << t;
        commands.pop_front();
    }

    return 0;
}
