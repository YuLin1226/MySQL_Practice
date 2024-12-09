cmake_minimum_required(VERSION 3.10)
project(inventory_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 手動指定SOCI和MySQL的庫位置
find_library(SOCI_CORE_LIB NAMES soci_core)
find_library(SOCI_MYSQL_LIB NAMES soci_mysql)
# find_package(MySQL REQUIRED)
set(MySQL_INCLUDE_DIR "/usr/include/mysql")

# 添加執行檔
add_executable(inventory_system src/main.cpp)

# Include directories
target_include_directories(inventory_system 
    PRIVATE 
    /usr/include/soci
    /usr/include/mysql
    ${MYSQL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)


# 連結庫
target_link_libraries(inventory_system
    PRIVATE
    ${SOCI_CORE_LIB}
    ${SOCI_MYSQL_LIB}
    mysqlclient
)