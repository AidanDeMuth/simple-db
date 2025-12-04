#pragma once

#include <cstdint>

/* Global Types */
using byte = unsigned char;
using int16 = std::int16_t;
using int32 = std::int32_t;

/* Global Definitions */
constexpr int32 SYSERR = -1; /* For system level errors (e.g. POSIX API) */