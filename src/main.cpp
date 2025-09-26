#include "Task.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

using namespace task;

int main(int argc, char** argv) {
    if (!fs::exists(CONFIG_NAME))
    {
       MakeDefaultConfig();
    }
    

    return 0;
}
