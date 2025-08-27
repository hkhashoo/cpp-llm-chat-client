#pragma once
#include <string>
#include <vector>
#include "http_client.hpp"

class Chat {
public:
    std::string reply(const std::string& user_input);
    const std::vector<Message>& history() const { return history_; }

private:
    std::vector<Message> history_;
    int history_limit_ = 5; // simple trim; can read from env later
};
