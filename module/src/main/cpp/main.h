//
// Created by canyie on 2020/8/12.
//

#ifndef NBINJECTION_MAIN_H
#define NBINJECTION_MAIN_H


#ifdef __LP64__
constexpr const char* kZygoteNiceName = "zygote64";
constexpr const char* nextLoadSo = "/system/lib64/libgadget.so";
#else
constexpr const char* kZygoteNiceName = "zygote";
constexpr const char* nextLoadSo = "/system/lib/libgadget.so";
#endif

#endif //NBINJECTION_MAIN_H
