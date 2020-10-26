set(CMAKE_CXX_STANDARD 11)

add_executable(sudokusolver_test SolverTest.cc SudokuSolver.cc)

add_executable(sudokuserver server.cc SudokuServer.cc SudokuSolver.cc)
target_link_libraries(sudokuserver muduo_net muduo_base pthread boost_program_options)
