add_library(${TB_CORELIB_TARGET_NAME} STATIC
	implementation/matricesModule.cpp
	implementation/globals.cpp
	implementation/Config.cpp
	implementation/TournamentConfig.cpp
	implementation/TeamArray.cpp
	implementation/strat.cpp
	implementation/to_strings.cpp
	implementation/TournamentBuilderAPI.cpp
)


target_compile_definitions(${TB_CORELIB_TARGET_NAME} PRIVATE 
	$<$<CONFIG:Debug>:DEBUG>
	$<$<CONFIG:Release>:NDEBUG>
)

if (${TB_CURRENT_COMPILER} STREQUAL COMPILER_MSVC)
add_link_options(
	$<$<CONFIG:Debug>:/INCREMENTAL> 
	$<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>
)
endif()

set_target_properties(${TB_CORELIB_TARGET_NAME} PROPERTIES LINK_DEPENDS_NO_SHARED TRUE)

target_include_directories(${TB_CORELIB_TARGET_NAME} PRIVATE 
	./include
	${TB_LIBS_DIR}
)


# Declare dependency on the absl::strings library
target_link_libraries(${TB_CORELIB_TARGET_NAME} PRIVATE 
	absl::flat_hash_set
	absl::hash absl::btree
)
