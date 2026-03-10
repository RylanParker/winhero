#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

int main() {
    list<string> commands = read_file();

    for (int i = 0; i < commands.size(); i++)
    {
        run_function(commands.front());
        commands.pop_front();
    }

    return 0;
}

list<string> read_file()
{
    // Reads each line in the file, appends them to a file and then returns them as a list of strings
    list<string> results = {};
    int iter = 0;
    string line;
    ifstream command_file("commands.txt");
    
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