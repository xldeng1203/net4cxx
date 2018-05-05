//
// Created by yuwenyong on 18-1-11.
//

#include "net4cxx/plugins/websocket/util.h"


NS_BEGIN

boost::optional<Duration> Timings::diffDura(const std::string &startKey, const std::string &endKey) {
    boost::optional<Duration> d;
    auto start = _timings.find(startKey);
    auto end = _timings.find(endKey);
    if (start != _timings.end() && end != _timings.end()) {
        d = end->second - start->second;
    }
    return d;
}

boost::optional<double> Timings::diff(const std::string &startKey, const std::string &endKey) {
    auto duration = diffDura(startKey, endKey);
    boost::optional<double> d;
    if (duration) {
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(*duration);
        *duration -= seconds;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(*duration);
        *duration -= milliseconds;
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(*duration);
        *duration -= microseconds;
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(*duration);
        d = 1.0 * seconds.count() + 0.001 * milliseconds.count() + 0.000001 * microseconds.count() +
            0.000000001 * nanoseconds.count();
    }
    return d;
}

std::string Timings::diffFormatted(const std::string &startKey, const std::string &endKey) {
    auto d = diff(startKey, endKey);
    std::string s;
    if (d) {
        if (*d < 0.00001) {
            // < 10us
            s = std::to_string((long long int)std::round(*d * 1000000000.)) + " ns";
        } else if (*d < 0.01) {
            // < 10ms
            s = std::to_string((long long int)std::round(*d * 1000000.)) + " us";
        } else if (*d < 10) {
            // < 10s
            s = std::to_string((long long int)std::round(*d * 1000.)) + " ms";
        } else {
            s = std::to_string((long long int)std::round(*d)) + " s";
        }
    } else {
        s = "n.a.";
    }
    StrUtil::rjustInplace(s, 8);
    return s;
}


WebSocketUtil::ParseResult WebSocketUtil::parseUrl(const std::string &url) {
    auto parsed = URLParse::urlParse(url);
    const auto &scheme = parsed.getScheme();
    if (scheme != "ws" && scheme != "wss") {
        NET4CXX_THROW_EXCEPTION(Exception, "invalid WebSocket URL: protocol scheme '" + scheme +
                                           "' is not for WebSocket");
    }
    auto hostname = parsed.getHostName();
    if (!parsed.getHostName() || (*hostname).empty()) {
        NET4CXX_THROW_EXCEPTION(Exception, "invalid WebSocket URL: missing hostname");
    }
    auto port = parsed.getPort();
    if (!port) {
        port = scheme == "ws" ? 80 : 443;
    }
    const auto &fragment = parsed.getFragment();
    if (!fragment.empty()) {
        NET4CXX_THROW_EXCEPTION(Exception, "invalid WebSocket URL: non-empty fragment '" + fragment + "'");
    }
    std::string path, ppath;
    if (!parsed.getPath().empty()) {
        ppath = parsed.getPath();
        path = URLParse::unquote(ppath);
    } else {
        ppath = "/";
        path = ppath;
    }
    std::string resource;
    QueryArgListMap params;
    if (!parsed.getQuery().empty()) {
        resource = ppath + "?" + parsed.getQuery();
        params = URLParse::parseQS(parsed.getQuery());
    } else {
        resource = ppath;
    }
    return std::make_tuple(scheme == "wss", std::move(*hostname), *port, std::move(resource), std::move(path),
                           std::move(params));
}

std::vector<boost::regex> WebSocketUtil::wildcardsToPatterns(const StringVector &wildcards) {
    std::vector<boost::regex> patterns;
    for (auto wc: wildcards) {
        boost::replace_all(wc, ".", "\\.");
        boost::replace_all(wc, "*", ".*");
        wc = "^" + wc + "$";
        patterns.emplace_back(wc);
    }
    return patterns;
}

//void TrafficStats::reset() {
//    _outgoingOctetsWireLevel = 0;
//    _outgoingOctetsWebSocketLevel = 0;
//    _outgoingOctetsAppLevel = 0;
//    _outgoingWebSocketFrames = 0;
//    _outgoingWebSocketMessages = 0;
//
//    _incomingOctetsWireLevel = 0;
//    _incomingOctetsWebSocketLevel = 0;
//    _incomingOctetsAppLevel = 0;
//    _incomingWebSocketFrames = 0;
//    _incomingWebSocketMessages = 0;
//
//    _preopenOutgoingOctetsWireLevel = 0;
//    _preopenIncomingOctetsWireLevel = 0;
//}
//


NS_END