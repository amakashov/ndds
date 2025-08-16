#pragma once

#ifdef _WIN32
    #ifdef NDDS_CORE_EXPORTS
        #define NDDS_EXPORT __declspec(dllexport)
    #else
        #define NDDS_EXPORT __declspec(dllimport)
    #endif
#else
    #define NDDS_EXPORT    
#endif

namespace Core
{
enum class LocatorType : uint8_t{LOCATOR_INVALID, LOKATOR_KIND_INVALID, LOCATOR_KIND_RESERVED, LOCATOR_KIND_UDPv4,
                                 LOCATOR_KIND_UDPv6, LOCATOR_ADDRESS_INVALID, LOCATOR_PORT_INVALID};
enum class TopicType : uint8_t {NO_KEY, WITH_KEY};
enum class ReliabilityType : uint8_t{BEST_EFFORT, RELIABLE};
enum class ProtocolVersionType :uint8_t {PROTOCOLVERSION, PROTOCOLVERSION_1_0, PROTOCOLVERSION_1_1, PROTOCOLVERSION_2_0,
                                        PROTOCOLVERSION_2_1, PROTOCOLVERSION_2_2, PROTOCOLVERSION_2_4};
}