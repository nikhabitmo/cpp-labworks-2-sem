add_executable(${PROJECT_NAME} main.cpp ../lib/CCircularBufferExt.cpp ../lib/CCircularBufferExt.h)

target_link_libraries(${PROJECT_NAME} PRIVATE CCircularBuffer)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})