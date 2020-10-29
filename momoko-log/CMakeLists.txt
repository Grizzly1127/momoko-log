set(SOURCE_FILE
    async_log.cc
    log_file.cc
    log_stream.cc
    logger.cc
    timestamp.cc
    )

add_library(momoko STATIC ${SOURCE_FILE})
target_link_libraries(momoko pthread)

install(TARGETS momoko DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include)
