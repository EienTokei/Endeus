# CMakeList.txt: Endeus 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 查找 SFML（vcpkg 会自动提供路径）
find_package(SFML COMPONENTS Graphics Window System REQUIRED)

# 将源代码添加到此项目的可执行文件。
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.hpp")
add_executable(Endeus ${SRC_FILES})

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
)

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
