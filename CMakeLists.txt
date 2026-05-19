cmake_minimum_required(VERSION 3.10)
project(LastResistance C)

# 실행 파일 출력 디렉토리
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 모든 소스 파일(.c) 자동 탐색
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

# 헤더 파일 검색 경로
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

# Windows GUI 실행 파일 생성
add_executable(game WIN32 ${SOURCES})

# 라이브러리 링크
target_link_libraries(game
    user32
    gdi32
)