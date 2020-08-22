link_directories(/xmuduo/muduo/net/build)
link_directories(/xmuduo/muduo/base/build)

add_executable(rttserver server/server.cc server/main.cc)
target_link_libraries(rttserver muduo_net muduo_base pthread)

add_executable(rttclient client/client.cc client/main.cc)
target_link_libraries(rttclient muduo_net muduo_base pthread)
