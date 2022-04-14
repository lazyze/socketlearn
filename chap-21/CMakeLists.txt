add_executable(pollserver pollserver.c)
target_link_libraries(pollserver yolanda)

add_executable(pollserverlz pollserverlz.c)
target_link_libraries(pollserverlz yolanda)
