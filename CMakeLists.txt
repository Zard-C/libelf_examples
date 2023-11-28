cmake_minimum_required(VERSION 3.15)
project(libelf_examples)

set(CMAKE_C_STANDARD 99)

add_executable(getting_started src/getting_started.c)
target_link_libraries(getting_started elf)

add_executable(print_elf_header src/print_elf_header.c)
target_link_libraries(print_elf_header elf bsd)

add_executable(read_header_table src/read_header_table.c)
target_link_libraries(read_header_table elf bsd)

# install
install(
TARGETS 
    getting_started 
    print_elf_header 
DESTINATION 
    bin
)