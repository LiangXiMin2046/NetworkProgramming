link_directories(/xmuduo/muduo/net/build)
link_directories(/xmuduo/muduo/base/build)

add_executable(simple_echo echo/echo.cc echo/main.cc)
target_link_libraries(simple_echo muduo_net muduo_base pthread)

add_executable(simple_chargen chargen/chargen.cc chargen/main.cc)
target_link_libraries(simple_chargen muduo_net muduo_base pthread)
