cmake_minimum_required(VERSION 3.15)
project(TCResearchSolver VERSION 1.0.0)

add_library(CPP23 INTERFACE)
target_compile_features(CPP23 INTERFACE cxx_std_23)

add_executable(TCResearchSolver
	"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/HexPosition.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Node.cpp"
)
target_include_directories(TCResearchSolver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TCResearchSolver PUBLIC CPP23)

install(TARGETS TCResearchSolver DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION .)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${TCResearchSolver_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TCResearchSolver_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${TCResearchSolver_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES "/.git/;/.vscode/;/build/;/dist/;/.gitignore;${CPACK_SOURCE_IGNORE_FILES}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")

include(CPack)