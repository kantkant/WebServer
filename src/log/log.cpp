#include "log.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/keywords/permissions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>


#include <ios>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

BOOST_LOG_ATTRIBUTE_KEYWORD(log_severity,
                            "Severity", severity_level)
typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink;

src::severity_logger<severity_level> slg;
static bool log_init_flag = false;

std::string get_current_format_time()
{
    std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    unsigned long pos = strTime.find('T');
    strTime.replace(pos, 1, std::string(" "));
    strTime.replace(pos + 3, 0, std::string(":"));
    strTime.replace(pos + 6, 0, std::string(":"));
    strTime.replace(6, 0, std::string("-"));
    strTime.replace(4, 0, std::string("-"));
    return strTime;
}

void open_log_hook(sinks::text_file_backend::stream_type &file)
{

    file << "==================================================" << "\n"
         << "[Start]\t\t" << get_current_format_time().c_str() << "\n"
         << "==================================================" << "\n";
}

void close_log_hook(sinks::text_file_backend::stream_type &file)
{
    file << "==================================================" << "\n"
         << "[End]\t\t" << get_current_format_time().c_str() << "\n"
         << "==================================================" << "\n";
}

// The formatting logic for the severity level
template<typename CharT, typename TraitsT>
inline std::basic_ostream<CharT, TraitsT> &operator<<(
        std::basic_ostream<CharT, TraitsT> &strm, severity_level lvl)
{
    static const char *const str[] = {
            "TRACE",
            "DEBUG",
            "INFO ",
            "WARN ",
            "ERROR",
            "FATAL"
    };
    if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str))) {
        strm << str[lvl];
    } else {
        strm << static_cast<int>(lvl);
    }
    return strm;
}

// Convert file path to only the filename
std::string path_to_filename(std::string path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}


void init(const std::string &file_prefix, const std::string &log_dir)
{
    if (log_init_flag)
        return;

    boost_file_prefix = file_prefix;
    auto file_name = log_dir + "/" + "%Y-%m-%d_" + file_prefix + "_%3N.log";
    auto message_format = (expr::stream
            << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
            << "] [" << log_severity
            << "] [" << expr::attr<attrs::current_thread_id::value_type>("ThreadID")
            << "] [" << expr::attr<std::string>("File") << ':' << expr::attr<int>("Line")
            << "] " << expr::message);

    boost::shared_ptr<logging::core> core = logging::core::get();
    boost::shared_ptr<text_sink> textSink = boost::make_shared<text_sink>();
    boost::shared_ptr<file_sink> fileSink(new file_sink(
            keywords::file_name = file_name,
            keywords::rotation_size = 20 * 1024 * 1024,
            keywords::auto_flush = true,
            keywords::permissions = 0644,
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
            keywords::open_mode = std::ios_base::app
    ));

    auto console_sink = logging::add_console_log(
            std::cout,
            keywords::auto_flush = true,
            keywords::format = message_format);

    fileSink->locked_backend()->set_file_collector(sinks::file::make_collector(
            keywords::target = log_dir,
            keywords::max_size = 200 * 1024 * 1024,
            keywords::min_free_space = 200 * 1024 * 1024
    ));

    fileSink->locked_backend()->scan_for_files(sinks::file::scan_matching, false);
    fileSink->set_formatter(message_format);

    fileSink->locked_backend()->set_open_handler(&open_log_hook);
    fileSink->locked_backend()->set_close_handler(&close_log_hook);

    core->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
    core->add_global_attribute("Line", attrs::mutable_constant<int>(0));
    core->set_filter(log_severity >= log_lv);

    logging::add_common_attributes();

    core->add_sink(textSink);
    core->add_sink(console_sink);
    core->add_sink(fileSink);
    log_init_flag = true;
}

void init(const std::string &file_prefix)
{
    init(file_prefix,"/home/ubuntu");
}