link_directories(/xmuduo/muduo/net/build)
link_directories(/xmuduo/muduo/base/build)

add_executable(echoserver main.cc server.cc)
target_link_libraries(echoserver muduo_net muduo_base pthread)
