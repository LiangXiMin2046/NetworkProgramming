link_directories(/xmuduo/muduo/base/build)
add_executable(ttcp_blocking ttcp_blocking.cc common.cc main.cc)
target_link_libraries(ttcp_blocking muduo_base boost_program_options)
set_target_properties(ttcp_blocking PROPERTIES COMPILE_FLAGS
"-Wno-error=old-style-cast -Wno-error=conversion")
