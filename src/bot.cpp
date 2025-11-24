#include "Bot.hpp"
#include "Task.hpp"

#include <iostream>
#include <stdexcept>

using namespace task;
using namespace bot;

int main(int argc, char* argv[]) {
    try {
        TaskManager task_manager;

        Bot bot(/**/);
        bot.Start();

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}