cmake_policy(SET CMP0015 NEW)

aux_source_directory(. SRC_LIST)

include_directories(BEFORE ..)
include_directories(BEFORE ../beast/include/)
include_directories (${JSONCPP_ROOT}/include/)

set(EXECUTABLE ethminer)

file(GLOB HEADERS "*.h")

add_executable(httpclient httpclient.cpp ${HEADERS})

target_link_libraries(httpclient PRIVATE boost_system pthread)

#add_executable(${EXECUTABLE} ${SRC_LIST} ${HEADERS})
add_executable(${EXECUTABLE} main.cpp ${HEADERS})

target_link_libraries(ethminer PRIVATE ethcore poolprotocols devcore ethminer-buildinfo)

if(ETHDBUS)
	find_package(PkgConfig)
	set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/x86_64-linux-gnu/pkgconfig" )
	pkg_check_modules(DBUS dbus-1)
	include_directories(${DBUS_INCLUDE_DIRS})
	link_directories(${DBUS_LIBRARY_DIRS})
	target_link_libraries(${EXECUTABLE} ${DBUS_LIBRARIES})
endif()

if(APICORE)
	target_link_libraries(ethminer PRIVATE apicore)
endif()

include(GNUInstallDirs)
install(TARGETS ethminer DESTINATION ${CMAKE_INSTALL_BINDIR})
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:ethminer> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()
