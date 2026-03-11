#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <Windows.h>
#include <filesystem>
#include <lmcons.h>
#include <vector>

namespace fs = std::filesystem;

using namespace std;

typedef LONG NTSTATUS;
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

vector<string> iter_directory(string path)
{
    vector<string> t;

    if (fs::is_directory(path))
    {
        try {
            for (const auto& entry : fs::directory_iterator(path))
                t.push_back(entry.path().string());
        }
        catch (const fs::filesystem_error&) {
            cout << "Could not open!";
        }
    }

    return t;
}


void get_directory(string path)
{
    vector<string> read_files;
    vector<string> new_files = iter_directory(path);

    for (int i = 0; i < new_files.size(); i++)
    {
        read_files.push_back(new_files[i]);
        vector<string> found_files = iter_directory(new_files[i]); // get the current file from the found directories
        cout << "okay";

        for (int x = 0; x < found_files.size(); x++) // go through each new file and append it, we will search them later
        {
            new_files.push_back(found_files[x]); // append it to new files
        }

        new_files.erase(new_files.begin() + i); // if the wrong file is removed, its probably coming from here
    }

    for (int i = 0; i < read_files.size(); i++)
    {
        cout << read_files[i];
    }

}

void get_files()
{
    std::string path = std::string(getenv("USERPROFILE"));
    std::cout << path << std::endl;
    get_directory(path);

}

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

list<string> ReadFile(string file_name)
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

string RunFunction(string command)
{
    // Runs a command and then returns the output from said command
    
    // Runs the command
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        return "Failed to run command: " + command;
    }

    char buffer[128];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) { // Reads each line and returns null when completed
        result += buffer;
    }
    
    // Frees resources
    pclose(pipe);

    cout << "Successfully ran: " + command;

    // Return the result
    return result;
}

int main() {
    // retrieve windows type
    RTL_OSVERSIONINFOW osVersion = GetRealOSVersion();
    string win_type = "";
    get_files();
    
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
