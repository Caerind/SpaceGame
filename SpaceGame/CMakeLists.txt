set(SRC_SPACEGAME
	AudioTrackMixer3000.cpp
	AudioTrackMixer3000.hpp
	Components.hpp
	GameSingleton.cpp
	GameSingleton.hpp
	GameState.cpp
	GameState.hpp
	main.cpp
)
source_group("" FILES ${SRC_SPACEGAME})

add_executable(SpaceGame ${SRC_SPACEGAME})
target_link_libraries(SpaceGame PUBLIC Enlivengine)