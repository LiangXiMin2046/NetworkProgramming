link_directories(/xmuduo/muduo/base/build)
link_directories(/xmuduo/muduo/net/build)

add_executable(roundtrip roundtrip.cc)
target_link_libraries(roundtrip muduo_base muduo_net pthread)

