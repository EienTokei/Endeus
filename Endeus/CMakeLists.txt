# CMakeList.txt: Endeus 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 查找 SFML（vcpkg 会自动提供路径）
find_package(SFML COMPONENTS Graphics Window System REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (Endeus "Endeus.cpp" "Endeus.h")

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

# TODO: 如有需要，请添加测试并安装目标。
