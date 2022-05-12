cmake_minimum_required(VERSION 3.0)

project("flatbuffer_study")

set(LIB_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(OUTPUT_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/build/output)
set(FB_GEN_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/fb_gen)
set(FLATBUFFERS_SRC_DIR ${LIB_DIR}/flatbuffers)

set(ALL_LIBS 

    flatbuffer_study
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FILE(
    GLOB_RECURSE MAIN_SRC_FILES
    
    #include files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp

    #source files
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)



set(FBPROTO_GEN_SUFFIX "_generated")
set(FBPROTO_GEN_EXT "h")
set(FBPROTO_GEN_EXT_CS ".cs")

add_definitions(-DSAVED_FILE_PATH=\"${CMAKE_SOURCE_DIR}/serialize_result/file\")

#flatbuffer
IF(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
message(STATUS "Use Windows flatc.exe")
set(FLATBUFFERS_FLATC_EXECUTABLE ${LIB_DIR}/flatbuffers/bin/windows/flatc.exe)
endif()

IF(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
message(STATUS "Use Linux flatc")
set(FLATBUFFERS_FLATC_EXECUTABLE ${LIB_DIR}/flatbuffers/bin/linux/flatc)
endif()

IF(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
message(STATUS "Use Mac flatc")
set(FLATBUFFERS_FLATC_EXECUTABLE ${LIB_DIR}/flatbuffers/bin/macos/flatc)
endif()

message(STATUS "flatc executable : ${FLATBUFFERS_FLATC_EXECUTABLE}" )


file(GLOB_RECURSE FLATBUFFER_PROPTS "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.fbs")

foreach(proto ${FLATBUFFER_PROPTS})
    get_filename_component(PROTO_FILE_NAME ${proto} NAME_WE)
    set(PROTO_GEN_FILE_NAME "${PROTO_FILE_NAME}${FBPROTO_GEN_SUFFIX}.${FBPROTO_GEN_EXT}")
    set(PROTO_GEN_FILE ${FB_GEN_DIR}/${PROTO_GEN_FILE_NAME})
    set(PROTO_GEN_FILE_NAME_CS "${PROTO_FILE_NAME}${FBPROTO_GEN_SUFFIX}.${FBPROTO_GEN_EXT_CS}")
    #set(PROTO_GEN_FILE_CS ${SANDBOX_FBPROTO_GEN_ROOT_CS}/${PROTO_GEN_FILE_NAME_CS})
    add_custom_command(
        OUTPUT ${PROTO_GEN_FILE}
        COMMAND ${FLATBUFFERS_FLATC_EXECUTABLE} -c -o ${FB_GEN_DIR} ${proto} --filename-suffix ${FBPROTO_GEN_SUFFIX} --filename-ext ${FBPROTO_GEN_EXT}
        DEPENDS ${proto}
        COMMENT "[flatc] generating ${proto} to	${PROTO_GEN_FILE}"
        VERBATIM
    )
	list(APPEND MAIN_SRC_FILES ${PROTO_GEN_FILE})
	list(APPEND MAIN_SRC_FILES ${PROTO_GEN_FILE_CS})
endforeach()


add_executable(flatbuffer_study ${MAIN_SRC_FILES})

target_include_directories(flatbuffer_study 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${FLATBUFFERS_SRC_DIR}/include/
    PUBLIC ${FB_GEN_DIR}
)


foreach(EACH_LIB ${ALL_LIBS})
    set_target_properties(${EACH_LIB} PROPERTIES DEBUG_POSTFIX "_d")
    set_target_properties(${EACH_LIB} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/runtime)
    set_target_properties(${EACH_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/library)
    set_target_properties(${EACH_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/archive)
endforeach(EACH_LIB ${ALL_LIBS})


add_custom_command(TARGET flatbuffer_study POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)