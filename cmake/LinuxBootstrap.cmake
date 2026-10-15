# First POSIX loader target: mapping/import audit only; no game execution.
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
  message(FATAL_ERROR "The native guest loader requires Linux x86-64")
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Threads REQUIRED)
set(loader "${CMAKE_CURRENT_SOURCE_DIR}/third_party/godzilla-core/src/loader.c")
execute_process(COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/tools/prepare_linux_bootstrap.py"
  "${loader}" "${CMAKE_CURRENT_BINARY_DIR}/linux_loader.c"
  COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${loader}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/prepare_linux_bootstrap.py")
add_executable(kyoei-linux-bootstrap "${CMAKE_CURRENT_BINARY_DIR}/linux_loader.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/linux/kyoei_linux_tls.c")
target_include_directories(kyoei-linux-bootstrap PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/godzilla-core/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/linux")
target_compile_options(kyoei-linux-bootstrap PRIVATE -Wall -Wextra -Werror)
target_link_libraries(kyoei-linux-bootstrap PRIVATE Threads::Threads)
enable_testing()
add_executable(test_kyoei_linux_tls
  "${CMAKE_CURRENT_SOURCE_DIR}/src/linux/test_kyoei_linux_tls.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/linux/kyoei_linux_tls.c")
target_compile_options(test_kyoei_linux_tls PRIVATE -Wall -Wextra -Werror)
target_link_libraries(test_kyoei_linux_tls PRIVATE Threads::Threads)
add_test(NAME linux_tls COMMAND test_kyoei_linux_tls)
add_test(NAME linux_bootstrap_validation COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_linux_bootstrap.py"
  "$<TARGET_FILE:kyoei-linux-bootstrap>")
