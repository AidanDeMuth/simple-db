#pragma once

#include <stdexcept>

#define ERROR_KILL(err, message, action) \
        do { \
            if ((err) < 0) { \
                perror(message); \
                action(-1); \
            } \
        } while(0)

#define ERROR_THROW(err, type, message) \
        do { \
            if ((err) < 0) { \
                throw type(message); \
            } \
        } while(0)

#define IFERROR_CALLBACK(err, callback, args) \
        do { \
            if ((err) < 0) { \
                callback(args) \
            } \
        } while(0)
        