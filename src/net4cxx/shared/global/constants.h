//
// Created by yuwenyong.vincent on 2018/7/14.
//

#ifndef NET4CXX_SHARED_GLOBAL_CONSTANTS_H
#define NET4CXX_SHARED_GLOBAL_CONSTANTS_H

#include "net4cxx/common/common.h"

NS_BEGIN


class NET4CXX_COMMON_API WatchKeys {
public:
    static const char *WebSocketServerProtocolCount;
    static const char *WebSocketClientProtocolCount;

    static const char *PeriodicCallbackCount;
    static const char *IOStreamCount;
    static const char *SSLIOStreamCount;
    static const char *HTTPClientCount;
    static const char *HTTPClientConnectionCount;
    static const char *HTTPConnectionCount;
    static const char *HTTPServerRequestCount;
    static const char *RequestHandlerCount;
};


NS_END

#endif //NET4CXX_SHARED_GLOBAL_CONSTANTS_H
