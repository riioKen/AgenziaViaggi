cmake_minimum_required(VERSION 3.21)
project(Agenzia_Viaggi_C C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(Agenzia_Viaggi_C
        src/admin_funcs.c
        src/booking_funcs.c
        src/city_funcs.c
        src/country_funcs.c
        src/header.h
        src/main.c
        src/UI_funcs.c
        src/user_funcs.c)
