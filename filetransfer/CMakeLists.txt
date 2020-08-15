link_directories(/xmuduo/muduo/net/build)
link_directories(/xmuduo/muduo/base/build)

add_executable(download_server server/server.cc server/main.cc)
target_link_libraries(download_server muduo_net muduo_base pthread)

add_executable(receive_client client/client.cc client/main.cc)
target_link_libraries(receive_client muduo_net muduo_base pthread)
