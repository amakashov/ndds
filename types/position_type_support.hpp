#pragma once

#include "position.hpp"
#include <cstdint>
#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

namespace serialization
{
    inline std::vector<uint8_t> serialize_position(const Position& pos)
    {
        eprosima::fastcdr::FastBuffer buffer;
        eprosima::fastcdr::Cdr cdr(buffer);
        cdr << pos.x << pos.y << pos.z << pos.psi << pos.theta << pos.gamma;
        return std::vector<uint8_t>(buffer.getBuffer(), buffer.getBuffer() + buffer.getBufferSize());
    }

    inline Position deserialize_position(std::vector<uint8_t> data)
    {
        eprosima::fastcdr::FastBuffer buffer(reinterpret_cast<char*>(data.data()), data.size());
        eprosima::fastcdr::Cdr cdr(buffer);
        Position pos;
         
        cdr >> pos.x >> pos.y >> pos.z >> pos.psi >> pos.theta >> pos.gamma;
        return pos;
    }
} // namespace serialization