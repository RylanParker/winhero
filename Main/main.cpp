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

#include <cstdio>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <Windows.h>
#include <filesystem>
#include <lmcons.h>
#include <vector>
#include <cstdlib>

// TO DO LIST:
// Iteration depth is too shallow 
// Compare metadata and other data to find malware
// Potentialy UI for user convienience


namespace fs = std::filesystem;

using namespace std;

typedef LONG NTSTATUS;
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

vector<string> retrieve_files(string path)
{
    // Iterate through a directory and add it a folder
    vector<string> files;
    std::error_code ec;

    for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied, ec))
    {
        if (entry.is_regular_file(ec))
            files.push_back(entry.path().string());
    }

    return files;
}

vector<string> iter_directory(string path)
{
    // Iterate through a directory and add it a folder
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

// Retrieves all currently running tasks
FILE* retrieve_tasks(){ return _popen("tasklist", "r"); }

void investigate_tasks()
{
    // Iterates through all running tasks and investigates the resources they use
    // Recommends killing them if they use an insane amount of resources

    FILE* tasks = retrieve_tasks();

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), tasks) != NULL) {
        string line = buffer;
        int line_len = line.length();

        if (line_len > 1)
        {
            // Format the task manager into only the things we want
            string name = line.substr(0, 15);
            bool is_service = line.find("Services");
            string mem_usage = line.substr(line_len-10, line_len);
            
            // Format out task manager garbage
            if (mem_usage.find("===") == string::npos && mem_usage.find("M") == string::npos)
            {
                mem_usage.erase(remove(mem_usage.begin(), mem_usage.end(), ','), mem_usage.end()); // Format out commas
                mem_usage.erase(remove(mem_usage.begin(), mem_usage.end(), 'K'), mem_usage.end()); // Format out K
 
                int memory = stoi(mem_usage);
                
                if (memory >= 15000) // Anything below 15MB isn't even worth culling
                {
                    cout << name << " is using: " << memory / 1000 << " megabytes! \n";
                }
            }
        }
    }
}

void get_directory(string path)
{
    vector<string> files;
    vector<string> read_dir;
    vector<string> new_dir = iter_directory(path);

    for (int i = 0; i < new_dir.size(); i++)
    {
        read_dir.push_back(new_dir[i]);
        vector<string> found_files = iter_directory(new_dir[i]); // get the current file from the found directories
        cout << "okay";

        for (int x = 0; x < found_files.size(); x++) // go through each new file and append it, we will search them later
        {
            new_dir.push_back(found_files[x]); // append it to new files
        }

        new_dir.erase(new_dir.begin() + i); // if the wrong file is removed, its probably coming from here
    }

    for (int i = 0; i < read_dir.size(); i++)
    {
        vector<string> t = retrieve_files(read_dir[i]);
        
        for (int x = 0; x < t.size(); x++)
        {
            cout << t[x] << "\n";
        }
    }

/*     for (int i = 0; i < read_dir.size(); i++)
    {
        cout << read_dir[i];
    } */

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
