cmake_minimum_required(VERSION 3.15)
project(RedisShoppingCart CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan nombró a este paquete como "redis++"
find_package(redis++ REQUIRED)

add_executable(shopping_cart main.cpp)

# Vinculamos la versión estática que Conan descargó
target_link_libraries(shopping_cart PRIVATE redis++::redis++_static)