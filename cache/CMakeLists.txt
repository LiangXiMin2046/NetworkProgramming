set(CMAKE_CXX_STANDARD 11)

add_executable(cacheserver Item.cc Session.cc CacheServer.cc  server.cc)
target_link_libraries(cacheserver muduo_base muduo_net muduo_inspect boost_program_options pthread tcmalloc)

add_executable(cached_footprint Item.cc CacheServer.cc footprint_test.cc Session.cc)
target_link_libraries(cached_footprint muduo_base muduo_net muduo_inspect pthread tcmalloc)

set_target_properties(cached_footprint PROPERTIES COMPILE_FLAGS "-DHAVE_TCMALLOC")
set_target_properties(cacheserver PROPERTIES COMPILE_FLAGS "-DHAVE_TCMALLOC")
