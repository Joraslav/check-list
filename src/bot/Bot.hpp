#pragma once

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace bot {

class Bot {
 public:
    using json = nlohmann::json;

    Bot(/* args */);
    ~Bot();

 private:
    /* data */
};

}  // namespace bot
