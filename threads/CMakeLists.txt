cmake_minimum_required(VERSION 3.16)

project(threads)

if (WIN32)
    add_executable(thread_win
        thread_win.cpp
    )
elseif(UNIX)
    set(sub_thread_sleep 2)
    set(main_thread_sleep 1)
    set(sub_thread_loop 8)
    set(main_thread_loop 6)
    add_compile_definitions(
        SUB_THREAD_LOOP=${sub_thread_loop} 
        SUB_THREAD_SLEEP=${sub_thread_sleep} 
        MAIN_THREAD_LOOP=${main_thread_loop}
        MAIN_THREAD_SLEEP=${main_thread_sleep}
    )
    
    add_executable(thread_linux
        thread_linux.cpp
    )
    target_link_libraries(thread_linux pthread)
endif()

