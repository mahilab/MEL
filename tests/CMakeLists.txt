add_executable(testing0 testing0.cpp)
target_link_libraries(testing0 MEL)

add_executable(testing1 testing1.cpp)
target_link_libraries(testing1 MEL)

add_executable(testing2 testing2.cpp ../src/MEL/Engine/Static/Engine.cpp ../src/MEL/Engine/Static/Component.cpp ../src/MEL/Engine/Static/Object.cpp)
target_link_libraries(testing2 MEL)

add_executable(testing3 testing3.cpp)
target_link_libraries(testing3 MEL)

add_executable(testing4 testing4.cpp)
target_link_libraries(testing4 MEL)
