#pragma once

#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>

enum severity_level {
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

static severity_level log_lv = debug;
static std::string boost_file_prefix{};
static std::string dump_file_path{};

extern boost::log::sources::severity_logger<severity_level> slg;

void init(const std::string &file_prefix);

std::string path_to_filename(std::string path);

template <typename ValueType>
ValueType set_get_attrib(const char *name, ValueType value)
{
    namespace logging = boost::log;
    namespace attrs = boost::log::attributes;

    auto attr = logging::attribute_cast<attrs::mutable_constant<ValueType>>(logging::core::get()->get_global_attributes()[name]);
    attr.set(value);
    return attr.get();
}

#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL

#define _WEBSERVER_CUSTOM_LOG(logger, sev) \
    BOOST_LOG_STREAM_WITH_PARAMS( \
                                  (logger), \
                                  (set_get_attrib("File", path_to_filename(__FILE__))) \
                                  (set_get_attrib("Line", __LINE__)) \
                                  (::boost::log::keywords::severity = sev) \
                                )

#define LOG_TRACE \
    _WEBSERVER_CUSTOM_LOG(slg, trace)

#define LOG_DEBUG \
    _WEBSERVER_CUSTOM_LOG(slg, debug)

#define LOG_INFO \
    _WEBSERVER_CUSTOM_LOG(slg, info)

#define LOG_WARN \
    _WEBSERVER_CUSTOM_LOG(slg, warning)

#define LOG_ERROR \
    _WEBSERVER_CUSTOM_LOG(slg, error)

#define LOG_FATAL \
    _WEBSERVER_CUSTOM_LOG(slg, fatal)