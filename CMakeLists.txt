cmake_minimum_required(VERSION 3.10)
project(MyUtility)

set(CMAKE_CXX_STANDARD 11)

# Добавьте пути к сгенерированным файлам Protobuf
list(APPEND CMAKE_PREFIX_PATH "C:/Users/Daulet/Desktop/gamatektest")

find_package(Protobuf REQUIRED)

# Сгенерированные файлы .pb.cc и .pb.h добавляются к вашему исполняемому файлу
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS device_info.proto)

# Укажите имя вашего исполняемого файла и все исходные файлы
add_executable(main main.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(client client.cpp ${PROTO_SRCS} ${PROTO_HDRS})

# Подключите библиотеку Protobuf
target_link_libraries( main PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite )
target_link_libraries( client PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite )

# Дополнительные настройки CMake по вашему усмотрению
# ...

