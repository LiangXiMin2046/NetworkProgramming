link_directories(/xmuduo/muduo/base/build)
link_directories(/xmuduo/muduo/net/build)
link_directories(/xmuduo/muduo/net/build/http)

add_executable(plot_test plot_test.cc plot.cc)
target_link_libraries(plot_test muduo_base gd)

add_executable(procmon procmon.cc plot.cc)
target_link_libraries(procmon muduo_http muduo_base muduo_net gd pthread)
