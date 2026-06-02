#include "engine/Engine.hpp"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "utils/Logger.hpp"

void endeus::initLogger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/endeus.log", true);

    console_sink->set_level(spdlog::level::trace);
    file_sink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
    auto logger = std::make_shared<spdlog::logger>("endeus", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%H:%M:%S.%e] %^%-5l%$ [%n] %-20s:%-3# | %v");
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(3));
}

#ifdef _WIN32
#include <windows.h>
#endif

void setupConsole() {
#ifdef _WIN32
    // 设置控制台输出代码页为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
}

int main() {
    setupConsole();
    endeus::initLogger();
    SPDLOG_INFO("Logger initialized, testing...");
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif // _DEBUG

    endeus::Engine engine;
    engine.run();

    spdlog::shutdown();     // 显式清理，在 CRT 泄漏检测之前释放它们的静态缓存
    return 0;
}