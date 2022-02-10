project(HW1 C CXX)

set(HW1_SOURCE
  ${HW1_SOURCE_DIR}/glcontext.cpp
  ${HW1_SOURCE_DIR}/main.cpp
)

set(HW1_INCLUDE_DIR ${HW1_SOURCE_DIR}/../include)

add_executable(HW1 ${HW1_SOURCE})
target_include_directories(HW1 PRIVATE ${HW1_INCLUDE_DIR})

add_dependencies(HW1 glad glfw eigen)
# Can include glfw and glad in arbitrary order
target_compile_definitions(HW1 PRIVATE GLFW_INCLUDE_NONE)
# More warnings
if (NOT MSVC)
  target_compile_options(HW1
    PRIVATE "-Wall"
    PRIVATE "-Wextra"
    PRIVATE "-Wpedantic"
  )
endif()
# Prefer std c++20, at least need c++17 to compile
set_target_properties(HW1 PROPERTIES
  CXX_STANDARD 20
  CXX_EXTENSIONS OFF
)

target_link_libraries(HW1
  PRIVATE glad
  PRIVATE glfw
  PRIVATE eigen
)
