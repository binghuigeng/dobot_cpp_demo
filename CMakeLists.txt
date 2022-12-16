cmake_minimum_required(VERSION 3.9)
project(dobot_cpp_demo)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Gui SerialPort)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)


add_executable(dobot_cpp_demo
  SerialPort.cpp
  util.cpp
  main.cpp
  api/BitConverter.cpp
  api/Feedback.cpp
  api/DobotMove.cpp
  api/ErrorInfoHelper.cpp
  api/DobotClient.cpp
  api/ErrorInfoBean.cpp
  api/Dashboard.cpp
  build/dobot_cpp_demo_autogen/EWIEGA46WW/moc_Demo.cpp
  build/dobot_cpp_demo_autogen/EWIEGA46WW/moc_SerialPort.cpp
  log/log.cpp
  protocol.cpp
  Demo.cpp
  Rs232Linux.cpp
)
target_link_libraries(dobot_cpp_demo Qt5::Core Qt5::Gui Qt5::SerialPort)
