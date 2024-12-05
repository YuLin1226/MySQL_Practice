cmake_minimum_required(VERSION 3.10)
project(LibrarySystem VERSION 1.0)

# 設置 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 找到 MySQL 依賴
# find_package(MySQL REQUIRED)

# 如果在非標準位置安裝了 MySQL，可以取消註釋下面的行並修改路徑
# set(MYSQL_INCLUDE_DIR "/usr/local/mysql/include")
# set(MYSQL_LIBRARY "/usr/local/mysql/lib/libmysqlclient.so")
set(MySQL_INCLUDE_DIR "/usr/include/mysql")
set(MySQL_LIBRARY_DIR "/usr/lib/x86_64-linux-gnu")  # 一般庫文件位置

# 添加可執行文件
add_executable(library_system
    # src/main.cpp
    src/main_user_test.cpp
    src/database.cpp
    src/database_operation.cpp
)

# 包含目錄
target_include_directories(library_system PRIVATE 
    ${MYSQL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    # ${CMAKE_CURRENT_SOURCE_DIR}
)

# 鏈接庫
target_link_libraries(library_system PRIVATE
    ${MYSQL_LIBRARY}
    mysqlclient
)

# 添加編譯選項
if(MSVC)
    target_compile_options(library_system PRIVATE /W4)
else()
    target_compile_options(library_system PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 設置輸出目錄
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 顯示一些配置信息
message(STATUS "MySQL include dir: ${MYSQL_INCLUDE_DIR}")
message(STATUS "MySQL library: ${MYSQL_LIBRARY}")