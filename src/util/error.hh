#ifndef ERROR
#define ERROR

#include <stdexcept>

#define ERROR_KILL(err, function, action) \
        do { \
            if ((err) < 0) { \
                perror(function); \
                action(-1); \
            } \
        } while(0)

#define ERROR_THROW(err, type, message) \
        do { \
            if ((err) < 0) { \
                throw type(message); \
            } \
        } while(0)

#endif