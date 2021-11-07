#cmake_minimum_required(VERSION 3.17)
cmake_minimum_required(VERSION 3.4.1)
project(rtmp)

#set(CMAKE_CXX_STANDARD 14)
#-D_GNU_SOURCE error: ‘struct hostent’ has no member named ‘h_addr’
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -DNO_CRYPTO -D_GNU_SOURCE")

add_executable(rtmp
        main.cpp

        librtmp/amf.c
        librtmp/amf.h
        librtmp/bytes.h
        librtmp/dh.h
        librtmp/dhgroups.h
        librtmp/handshake.h
        librtmp/hashswf.c
        librtmp/http.h
        librtmp/log.c
        librtmp/log.h
        librtmp/parseurl.c
        librtmp/rtmp.c
        librtmp/rtmp.h
        librtmp/rtmp_sys.h
        )