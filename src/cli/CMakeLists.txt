########################################
###                CLI               ###
########################################

project(cli)

#### Dependencies ###
#### Project configuration ####

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS *.h)

add_executable(drampower_cli ${SOURCE_FILES})
target_link_libraries(drampower_cli 
	PRIVATE 
		DRAMSys::DRAMPower
)

build_source_group()