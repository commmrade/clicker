cmake_minimum_required(VERSION 3.10.0)
project(proj VERSION 0.1.0 LANGUAGES C CXX)

# Find Drogon package
find_package(Drogon REQUIRED)

# Specify the path to the lib directory
set(LIBRARY_PATH "${CMAKE_SOURCE_DIR}/lib")

# Include directories for MySQL Connector/C++
include_directories(${LIBRARY_PATH})

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)


# Add the executable
add_executable(proj src/main.cpp src/DatabaseHandler.cpp src/SessionManager.cpp src/utils/bcrypt.cpp src/utils/blowfish.cpp src/handlers/Game.cpp src/filters/JwtFilter.cpp src/handlers/AuthHandler.cpp src/utils/utils.cpp src/ConnectionPool.cpp) 
# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Link OpenSSL libraries to your project
target_link_libraries(proj PRIVATE OpenSSL::Crypto OpenSSL::SSL)

# Include directories for the project
target_include_directories(proj PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(proj PRIVATE ${MySQL_LIBRARIES})  # Use PRIVATE for MySQL libraries
target_link_libraries(proj PUBLIC Drogon::Drogon)  # Drogon should be PUBLIC

# Link the MySQL Connector/C++ shared library
target_link_libraries(proj PRIVATE ${LIBRARY_PATH}/libmysqlcppconn.so.10)

