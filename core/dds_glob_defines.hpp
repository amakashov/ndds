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