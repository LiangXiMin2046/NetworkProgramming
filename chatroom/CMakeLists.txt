link_directories(/xmuduo/muduo/base/build)
link_directories(/xmuduo/muduo/net/build)

add_executable(chatserver server/server.cc server/main.cc)
target_link_libraries(chatserver muduo_base muduo_net pthread)

add_executable(chatclient client/client.cc client/main.cc)
target_link_libraries(chatclient muduo_base muduo_net pthread)
