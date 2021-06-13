#include "../log.h"

int main(int argc, char const *argv[])
{
    init("WebServer");
    LOG_TRACE << "A trace severity message";
    LOG_DEBUG << "A debug severity message";
    LOG_INFO << "A info severity message";
    LOG_WARN << "A warning severity message";
    LOG_ERROR << "A error severity message";
    LOG_FATAL << "A fatal severity message";
    return 0;
}
