#include "shared.h"

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

FILE* retrieve_tasks(){ return _popen("tasklist", "r"); }

// Retrieve an element in a dict by index
vector<string> index_dict(unordered_map<string, vector<string>> t_dict, string target_p)
{
    auto t = t_dict.find(target_p);

    if (t != t_dict.end())
    {
        return t->second;
    }
    else
    {
        return vector<string>{"None"};
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

        for (int x = 0; x < found_files.size(); x++) // go through each new file and append it, we will search them later
        {
            new_dir.push_back(found_files[x]); // append it to new files
        }

        new_dir.erase(new_dir.begin() + i); // if the wrong file is removed, its probably coming from here
    }

    for (int i = 0; i < read_dir.size(); i++)
    {
        vector<string> t = retrieve_files(read_dir[i]);
        
 /*        for (int x = 0; x < t.size(); x++)
        {
            cout << t[x] << "\n";
        } */
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


void investigate_tasks()
{
    // Iterates through all running tasks and investigates the resources they use
    // Recommends killing them if they use an insane amount of resources

    unordered_map<string, vector<string>> process_commands = {
        {"DesktopWindowsManager", {"sfc /scannow", "dism /online /cleanup-image /restorehealth   "}},
        {"svchost.exe", {"ipconfig /flushdns", "rundll32.exe advapi32.dll,ProcessIdleTasks"}}
    };

    // Track all ran commands otherwise it'll repeat commands
    vector<string> ran_commands;

    FILE* tasks = retrieve_tasks();

    char buffer[256];

    while (fgets(buffer, sizeof(buffer), tasks) != NULL) {
        string line = buffer;
        int line_len = line.length();

        if (line_len > 1)
        {
            // Format the task manager into only the things we want
            string name = line.substr(0, 15);
            name.erase(remove(name.begin(), name.end(), ' '), name.end());
            bool is_service = line.find("Services");
            string mem_usage = line.substr(line_len-10, line_len);
            
            // Format out task manager garbage
            if (mem_usage.find("===") == string::npos && mem_usage.find("M") == string::npos)
            {
                mem_usage.erase(remove(mem_usage.begin(), mem_usage.end(), ','), mem_usage.end()); // Format out commas
                mem_usage.erase(remove(mem_usage.begin(), mem_usage.end(), 'K'), mem_usage.end()); // Format out K
                mem_usage.erase(remove(mem_usage.begin(), mem_usage.end(), ' '), mem_usage.end());
 
                int memory = stoi(mem_usage);
                
                if (memory >= 150) // Anything below 15MB isn't even worth culling
                {
                   // cout << name << " is using: " << memory / 1000 << " megabytes! \n";
                    vector<string> commands = index_dict(process_commands, name);
                    std::vector<string>::iterator it = find(commands.begin(), commands.end(), "None");
                    
                    if (it == commands.end() && 
                    find(ran_commands.begin(), ran_commands.end(), name) == ran_commands.end())
                    {
                        ran_commands.push_back(name);
                        for (const string command : commands)
                        {
                            cout << "Running command: " << command << "\n";
                            RunFunction(command);
                        }
                    }
                }
            }
        }
    }
}


/* int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
} */