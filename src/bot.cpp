#include "Bot.hpp"
#include "Task.hpp"

#include <iostream>
#include <stdexcept>

using namespace task;
using namespace bot;

int main(int argc, char* argv[]) {
    try {
        TaskManager task_manager;

        Bot bot("8350921003:AAF0ImPD67kchBcxtsRezaLHb05rlBAGB2Q");
        bot.Start();

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}