# CMakeList.txt: Endeus 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 查找 SFML（vcpkg 会自动提供路径）
find_package(SFML COMPONENTS Graphics Window System Audio REQUIRED)
# 查找 spdlog
find_package(spdlog CONFIG REQUIRED) 

# 将源代码添加到此项目的可执行文件。
set(SOURCES
    "src/main.cpp"
    "src/engine/Engine.cpp"
    "src/director/Director.cpp"
    "src/anemoi/Anemoi.cpp"
    "src/executor/Executor.cpp"
    "src/renderer/Renderer.cpp"
    "src/world/WorldManager.cpp"
    "src/album/WhiteAlbum.cpp"
    "src/audio/GloriousDays.cpp"
)

add_executable(Endeus ${SOURCES})

# 强制使用 C++20 标准
set_target_properties(Endeus PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# 链接 SFML 库
target_link_libraries(Endeus PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    spdlog::spdlog
)

target_compile_options(Endeus PRIVATE /utf-8)

# 将可执行文件输出到构建目录下的 bin/ 文件夹
set_target_properties(Endeus PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 清理并复制 assets 文件夹到可执行文件输出目录
add_custom_command(TARGET Endeus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:Endeus>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Endeus>/assets"
    COMMENT "Copying fresh assets to executable directory"
)

# TODO: 如有需要，请添加测试并安装目标。

# 查找 GTest
find_package(GTest CONFIG REQUIRED)

# 准备测试用的源文件列表：所有 .cpp 文件，但排除 main.cpp
file(GLOB_RECURSE TEST_SRC_FILES "src/*.cpp")
list(REMOVE_ITEM TEST_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# 添加测试可执行文件
add_executable(EndeusTests
    tests/test_instruction.cpp
    "tests/test_leyline.cpp"
    tests/test_world_manager.cpp
    ${TEST_SRC_FILES}   # 直接包含业务源文件（不含 main）
)

# 链接 SFML 和 GTest
target_link_libraries(EndeusTests PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    GTest::gtest
    GTest::gtest_main
    spdlog::spdlog
)

target_compile_options(EndeusTests PRIVATE /utf-8)

# 设置 C++ 标准和输出目录
set_target_properties(EndeusTests PROPERTIES
    CXX_STANDARD 20
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 启用测试并自动发现
enable_testing()
add_test(NAME EndeusTests COMMAND $<TARGET_FILE:EndeusTests>)