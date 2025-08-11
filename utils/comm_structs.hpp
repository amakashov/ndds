#pragma once
#include <string>

struct IPConnection
{
    struct IPNode
    {
        std::string address;
        uint16_t port;
    } sender, receiver;
    float frequency;
};