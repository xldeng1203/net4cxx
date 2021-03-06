//
// Created by yuwenyong on 17-9-15.
//

#ifndef NET4CXX_COMMON_LOGGING_SINKS_H
#define NET4CXX_COMMON_LOGGING_SINKS_H

#include "net4cxx/common/common.h"
#include <boost/log/sinks.hpp>
#include <boost/log/utility/functional.hpp>
#include "net4cxx/common/logging/attributes.h"

NS_BEGIN

namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

using SinkPtr = boost::shared_ptr<sinks::sink>;


class NET4CXX_COMMON_API SinkBuilder {
public:
    typedef sinks::basic_formatting_sink_frontend<char> FrontendSink;
    typedef boost::shared_ptr<FrontendSink> FrontendSinkPtr;

    virtual ~SinkBuilder() = default;

    void setAsync(bool async) {
        _async = async;
    }

    void setFormatter() {
        _formatter = expr::stream << '[' << expr::format_date_time<DateTime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                << "][" << expr::attr<Severity>("Severity") << ']' << expr::smessage;
    }

    void setFormatter(const std::string &formatter) {
        _formatter = logging::parse_formatter(formatter);
    }

    void setFormatter(const logging::formatter &formatter) {
        _formatter = formatter;
    }

    void resetFormatter() {
        _formatter = boost::none;
    }

    void setFilter(Severity severity) {
        _filter = (attr_severity >= severity);
    }

    void setFilter(Severity severity, const std::string &name, bool beginsWith=false) {
        if (beginsWith) {
            _filter = (expr::begins_with(attr_channel, name) && attr_severity >= severity);
        } else {
            _filter = (attr_channel == name && attr_severity >= severity);
        }
    }

    void setFilter(const std::string &filter) {
        _filter = logging::parse_filter(filter);
    }

    void setFilter(const logging::filter &filter) {
        _filter = filter;
    }

    void resetFilter() {
        _filter = boost::none;
    }

    SinkPtr build() const;
protected:
    virtual FrontendSinkPtr createSink() const= 0;

    virtual FrontendSinkPtr createAsyncSink() const= 0;

    void onSetFilter(FrontendSinkPtr sink) const {
        if (_filter) {
            sink->set_filter(*_filter);
        }
    }

    void onSetFormatter(FrontendSinkPtr sink) const {
        if (_formatter) {
            sink->set_formatter(*_formatter);
        }
    }

    bool _async{false};
    boost::optional<logging::formatter> _formatter;
    boost::optional<logging::filter> _filter;
};


class NET4CXX_COMMON_API ConsoleSinkBuilder: public SinkBuilder {
public:
    typedef sinks::text_ostream_backend BackendSink;
    typedef boost::shared_ptr<BackendSink> BackendSinkPtr;

    explicit ConsoleSinkBuilder(bool autoFlush=true)
            : _autoFlush(autoFlush) {

    }

    void setAutoFlush(bool autoFlush) {
        _autoFlush = autoFlush;
    }

protected:
    FrontendSinkPtr createSink() const override;

    FrontendSinkPtr createAsyncSink() const override;

    virtual BackendSinkPtr createBackend() const;

    bool _autoFlush;
};


class NET4CXX_COMMON_API FileSinkBuilder: public ConsoleSinkBuilder {
public:
    explicit FileSinkBuilder(std::string fileName,
                             bool autoFlush=true)
            : ConsoleSinkBuilder(autoFlush)
            , _fileName(std::move(fileName)) {

    }
protected:
    BackendSinkPtr createBackend() const override;

    std::string _fileName;
};


class NET4CXX_COMMON_API RotatingFileSinkBuilder: public SinkBuilder {
public:
    typedef sinks::text_file_backend BackendSink;
    typedef boost::shared_ptr<BackendSink> BackendSinkPtr;

    explicit RotatingFileSinkBuilder(std::string fileName,
                                     size_t maxFileSize = 5 * 1024 * 1024,
                                     std::ios_base::openmode mode = std::ios_base::app|std::ios_base::out,
                                     bool autoFlush = true)
            : _fileName(std::move(fileName))
            , _maxFileSize(maxFileSize)
            , _mode(mode)
            , _autoFlush(autoFlush) {

    }

    void setOpenMode(std::ios_base::openmode mode) {
        _mode = mode;
    }

    void setMaxFileSize(size_t maxFileSize) {
        _maxFileSize = maxFileSize;
    }

    void setAutoFlush(bool autoFlush) {
        _autoFlush = autoFlush;
    }
protected:
    FrontendSinkPtr createSink() const override;

    FrontendSinkPtr createAsyncSink() const override;

    virtual BackendSinkPtr createBackend() const;

    std::string _fileName;
    size_t _maxFileSize;
    std::ios_base::openmode _mode;
    bool _autoFlush;
};


class TimedRotatingFileSinkBuilder: public RotatingFileSinkBuilder {
public:
    using TimePoint = sinks::file::rotation_at_time_point;
    using TimeInterval = sinks::file::rotation_at_time_interval;

    class RotationTimeVisitor: public boost::static_visitor<BackendSinkPtr> {
    public:
        RotationTimeVisitor(std::string fileName,
                            size_t maxFileSize, std::ios_base::openmode mode,
                            bool autoFlush)
                : _fileName(std::move(fileName))
                , _maxFileSize(maxFileSize)
                , _mode(mode)
                , _autoFlush(autoFlush) {

        }

        BackendSinkPtr operator()(const TimePoint &timePoint) const {
            auto backend = boost::make_shared<BackendSink>(
                    keywords::file_name = _fileName,
                    keywords::rotation_size = _maxFileSize,
                    keywords::open_mode = _mode,
                    keywords::auto_flush = _autoFlush,
                    keywords::time_based_rotation = timePoint
            );
            return backend;
        }

        BackendSinkPtr operator()(const TimeInterval &timeInterval) const {
            auto backend = boost::make_shared<BackendSink>(
                    keywords::file_name = _fileName,
                    keywords::rotation_size = _maxFileSize,
                    keywords::open_mode = _mode,
                    keywords::auto_flush = _autoFlush,
                    keywords::time_based_rotation = timeInterval
            );
            return backend;
        }

    protected:
        std::string _fileName;
        size_t _maxFileSize;
        std::ios_base::openmode _mode;
        bool _autoFlush;
    };

    explicit TimedRotatingFileSinkBuilder(std::string fileName,
                                          const TimePoint &timePoint = sinks::file::rotation_at_time_point(0, 0, 0),
                                          size_t maxFileSize = 5 * 1024 * 1024,
                                          std::ios_base::openmode mode = std::ios_base::app | std::ios_base::out,
                                          bool autoFlush = true)
            : RotatingFileSinkBuilder(std::move(fileName), maxFileSize, mode, autoFlush)
            , _rotationTime(timePoint) {

    }

    TimedRotatingFileSinkBuilder(std::string fileName,
                                 const TimeInterval &timeInterval,
                                 size_t maxFileSize = 5 * 1024 * 1024,
                                 std::ios_base::openmode mode = std::ios_base::app | std::ios_base::out,
                                 bool autoFlush = true)
            : RotatingFileSinkBuilder(std::move(fileName), maxFileSize, mode, autoFlush)
            , _rotationTime(timeInterval) {

    }

    void setRotationTimePoint(const TimePoint &timePoint) {
        _rotationTime = timePoint;
    }

    void setRotationTimeInterval(const TimeInterval &timeInterval) {
        _rotationTime = timeInterval;
    }
protected:
    BackendSinkPtr createBackend() const override;

    boost::variant<TimePoint, TimeInterval> _rotationTime;
};

#ifndef BOOST_LOG_WITHOUT_SYSLOG

using SyslogFacility = sinks::syslog::facility;

#define SYSLOG_FACILITY_KERNEL      sinks::syslog::kernel
#define SYSLOG_FACILITY_USER        sinks::syslog::user
#define SYSLOG_FACILITY_MAIL        sinks::syslog::mail
#define SYSLOG_FACILITY_DAEMON      sinks::syslog::daemon
#define SYSLOG_FACILITY_SECURITY0   sinks::syslog::security0
#define SYSLOG_FACILITY_SYSLOGD     sinks::syslog::syslogd
#define SYSLOG_FACILITY_PRINTER     sinks::syslog::printer
#define SYSLOG_FACILITY_NEWS        sinks::syslog::news
#define SYSLOG_FACILITY_UUCP        sinks::syslog::uucp
#define SYSLOG_FACILITY_CLOCK0      sinks::syslog::clock0
#define SYSLOG_FACILITY_SECURITY1   sinks::syslog::security1
#define SYSLOG_FACILITY_FTP         sinks::syslog::ftp
#define SYSLOG_FACILITY_NTP         sinks::syslog::ntp
#define SYSLOG_FACILITY_LOG_AUDIT   sinks::syslog::log_audit
#define SYSLOG_FACILITY_LOG_ALERT   sinks::syslog::log_alert
#define SYSLOG_FACILITY_CLOCK1      sinks::syslog::clock1
#define SYSLOG_FACILITY_LOCAL0      sinks::syslog::local0
#define SYSLOG_FACILITY_LOCAL1      sinks::syslog::local1
#define SYSLOG_FACILITY_LOCAL2      sinks::syslog::local2
#define SYSLOG_FACILITY_LOCAL3      sinks::syslog::local3
#define SYSLOG_FACILITY_LOCAL4      sinks::syslog::local4
#define SYSLOG_FACILITY_LOCAL5      sinks::syslog::local5
#define SYSLOG_FACILITY_LOCAL6      sinks::syslog::local6
#define SYSLOG_FACILITY_LOCAL7      sinks::syslog::local7


class NET4CXX_COMMON_API SyslogSinkBuilder: public SinkBuilder {
public:
    typedef sinks::syslog_backend BackendSink;
    typedef boost::shared_ptr<BackendSink> BackendSinkPtr;

    explicit SyslogSinkBuilder(SyslogFacility facility=SYSLOG_FACILITY_USER)
            : _facility(facility) {

    }

    explicit SyslogSinkBuilder(std::string targetAddress,
                               unsigned short targetPort=514,
                               SyslogFacility facility=SYSLOG_FACILITY_USER)
            : _targetAddress(std::move(targetAddress))
            , _targetPort(targetPort)
            , _facility(facility) {

    }
protected:
    FrontendSinkPtr createSink() const override;

    FrontendSinkPtr createAsyncSink() const override;

    BackendSinkPtr createBackend() const;

    boost::optional<std::string> _targetAddress;
    unsigned short _targetPort{514};
    SyslogFacility _facility{SYSLOG_FACILITY_USER};
};

#endif


#if !defined(BOOST_LOG_WITHOUT_EVENT_LOG) && (COMPILER == COMPILER_MICROSOFT)

using RegistrationMode = sinks::event_log::registration_mode;

#define REGISTRATION_MODE_NEVER     sinks::event_log::never
#define REGISTRATION_MODE_ON_DEMAND sinks::event_log::on_demand
#define REGISTRATION_MODE_FORCED    sinks::event_log::forced


class NET4CXX_COMMON_API SimpleEventLogSinkBuilder: public SinkBuilder {
public:
    typedef sinks::simple_event_log_backend BackendSink;
    typedef boost::shared_ptr<BackendSink> BackendSinkPtr;

    explicit SimpleEventLogSinkBuilder(std::string logName="",
                                       std::string logSource="",
                                       RegistrationMode registrationMode=REGISTRATION_MODE_ON_DEMAND)
            : _logName(std::move(logName))
            , _logSource(std::move(logSource))
            , _registrationMode(registrationMode) {

    }

    void setLogName(std::string logName) {
        _logName = std::move(logName);
    }

    void setLogSource(std::string logSource) {
        _logSource = std::move(logSource);
    }

    void setRegistrationMode(RegistrationMode registrationMode) {
        _registrationMode = registrationMode;
    }
protected:
    FrontendSinkPtr createSink() const override;

    FrontendSinkPtr createAsyncSink() const override;

    BackendSinkPtr createBackend() const;

    std::string _logName;
    std::string _logSource;
    RegistrationMode _registrationMode{REGISTRATION_MODE_ON_DEMAND};
};

#endif


#ifndef BOOST_LOG_WITHOUT_DEBUG_OUTPUT

class NET4CXX_COMMON_API DebuggerSinkBuilder: public SinkBuilder {
public:
    typedef sinks::debug_output_backend BackendSink;
    typedef boost::shared_ptr<BackendSink> BackendSinkPtr;

    DebuggerSinkBuilder() = default;
protected:
    FrontendSinkPtr createSink() const override;

    FrontendSinkPtr createAsyncSink() const override;

    BackendSinkPtr createBackend() const;
};

#endif


class BasicSinkFactory: public logging::sink_factory<char> {
public:
    typedef logging::sink_factory<char> base_type;
    typedef typename base_type::char_type char_type;
    typedef typename base_type::string_type string_type;
    typedef typename base_type::settings_section settings_section;

protected:
    static bool paramCastToBool(const string_type &param);

    template <typename BackendT>
    static boost::shared_ptr<sinks::sink> initSink(boost::shared_ptr<BackendT> const& backend,
                                                   settings_section const& params) {
        typedef BackendT backend_t;
        logging::filter filt;
        if (boost::optional<string_type> filterParam = params["Filter"]) {
            filt = logging::parse_filter(filterParam.get());
        }
        boost::shared_ptr<sinks::basic_sink_frontend> p;
        bool async = false;
        if (boost::optional<string_type> asyncParam = params["Asynchronous"]) {
            async = paramCastToBool(asyncParam.get());
        }
        if (!async) {
            p = initFormatter(boost::make_shared<sinks::synchronous_sink<backend_t>>(backend), params);
        } else {
            p = initFormatter(boost::make_shared<sinks::asynchronous_sink<backend_t>>(backend), params);
            p->set_exception_handler(logging::nop());
        }
        p->set_filter(filt);
        return p;
    }

    template <typename SinkT>
    static boost::shared_ptr<SinkT> initFormatter(boost::shared_ptr<SinkT> const& sink,
                                                  settings_section const& params) {
        if (boost::optional<string_type> formatParam = params["Format"]) {
            sink->set_formatter(logging::parse_formatter(formatParam.get()));
        }
        return sink;
    }
};


class FileSinkFactory: public BasicSinkFactory {
public:
    typedef sinks::text_ostream_backend BackendSink;
    typedef boost::shared_ptr<BackendSink> BackendSinkPtr;

    boost::shared_ptr<sinks::sink> create_sink(settings_section const& settings) override;
};

NS_END

#endif //NET4CXX_COMMON_LOGGING_SINKS_H
