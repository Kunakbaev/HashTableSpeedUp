#ifndef COMMON_ERRORS_HANDLER_DEFINES_HPP
#define COMMON_ERRORS_HANDLER_DEFINES_HPP

#include <assert.h>

#include "../external/LoggerLib/include/logLib.hpp"

// bruh, auto is C++ feature, can pass error type to define, no big deal
#define COMMON_IF_ERR_RETURN(errorTmp, getErrorMessageFunc, OK_STATUS)      \
    do {                                                                    \
        auto tmpErr = errorTmp;                                             \
        if (tmpErr != OK_STATUS) {                                          \
            LOG_ERROR(getErrorMessageFunc(tmpErr));                         \
            assert(tmpErr == OK_STATUS);                                    \
            return tmpErr;                                                  \
        }                                                                   \
    } while(0)

#define COMMON_IF_NOT_COND_RETURN(condition, error, getErrorMessageFunc)    \
    do {                                                                    \
        bool tmpCondition = (condition);                                    \
        if (!tmpCondition) {                                                \
            LOG_ERROR(getErrorMessageFunc(error));                          \
            LOG_ERROR("condition not satisfied: " #condition);              \
            assert(#condition && tmpCondition);                             \
            return error;                                                   \
        }                                                                   \
    } while(0)

// ASK: should I create tmp variable for this case too?
#define COMMON_IF_ARG_NULL_RETURN(arg, error, getErrorMessageFunc)          \
    do {                                                                    \
        if (arg == NULL) {                                                  \
            LOG_ERROR(getErrorMessageFunc(error));                          \
            assert(arg != NULL);                                            \
            return error;                                                   \
        }                                                                   \
    } while (0)





#define COMMON_IF_SUBMODULE_ERR_RETURN(errorTmp, getErrorMessageFunc, OK_STATUS, returnError)                                        \
    do {                                                                    \
        auto bruh = errorTmp;                                               \
        if (bruh != OK_STATUS) {                                            \
            LOG_ERROR(getErrorMessageFunc(bruh));                           \
            assert(bruh != OK_STATUS);                                      \
            return returnError;                                             \
        }                                                                   \
    } while(0)


#endif