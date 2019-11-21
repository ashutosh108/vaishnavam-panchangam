#include(ExternalProject)
#ExternalProject_Add(
#	tzdata
#	#	EXCLUDE_FROM_ALL 1
#	URL 	ftp://ftp.iana.org/tz/tzdata-latest.tar.gz
#	CONFIGURE_COMMAND ""
#	BUILD_COMMAND ""
#	INSTALL_COMMAND ""
#	SOURCE_DIR ${TOP_DIR}/vendor/tzdata
#)
get_filename_component(TOP_DIR ${CMAKE_SCRIPT_MODE_FILE} DIRECTORY)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E echo_append "Downloading tz data..."
)
FILE(
	DOWNLOAD ftp://ftp.iana.org/tz/tzdata-latest.tar.gz
	${TOP_DIR}/vendor/tzdata/tzdata-latest.tar.gz
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E echo " done"
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E echo_append "Downloading windowsZones.xml..."
)
FILE(
	DOWNLOAD        https://raw.githubusercontent.com/unicode-org/cldr/master/common/supplemental/windowsZones.xml
	${TOP_DIR}/vendor/tzdata/windowsZones.xml
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E echo " done"
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E echo_append "Unpacking tz data..."
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E make_directory ${TOP_DIR}/vendor/tzdata
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E tar xzf ${TOP_DIR}/vendor/tzdata/tzdata-latest.tar.gz
	WORKING_DIRECTORY ${TOP_DIR}/vendor/tzdata
)
FILE(REMOVE ${TOP_DIR}/vendor/tzdata/tzdata-latest.tar.gz)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E echo " done"
)
FILE(STRINGS ${TOP_DIR}/vendor/tzdata/version TZ_VERSION LIMIT_COUNT 1)
MESSAGE(STATUS "tzdata version: ${TZ_VERSION}")
FILE(STRINGS ${TOP_DIR}/vendor/tzdata/windowsZones.xml WZ_VERSION_LINE REGEX "typeVersion=")
MESSAGE(STATUS "windowsZones version: ${WZ_VERSION_LINE}")
