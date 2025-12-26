// src/core/Register.h
#pragma once
#include "Registry.h"

#define REGISTER_DAY(DAY, TYPE) \
    static bool _reg_##TYPE = [] { \
        Registry::instance().register_day(DAY, [] { \
            return std::make_unique<TYPE>(); \
        }); \
        return true; \
    }()