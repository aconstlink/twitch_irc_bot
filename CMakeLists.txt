cmake_minimum_required( VERSION 3.20 )
project( "twitch_irc_bot" )

# required by motor
set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build )

add_subdirectory( motor )
list( APPEND CMAKE_MODULE_PATH 
  "${CMAKE_CURRENT_LIST_DIR}/motor/cmake/macros" )

include( visual_studio_generate_bat_dll )
include( emacs_default_directory )

motor_emacs_default_directory( "${CMAKE_CURRENT_LIST_DIR}/motor" )

set( CMAKE_DEBUG_POSTFIX d )

set( programs
  "01_console_bot.cpp"
)

foreach( _prog ${programs} )

    string( LENGTH ${_prog} len )
    math( EXPR app_name_len "${len}-4")
    string( SUBSTRING ${_prog} 0 ${app_name_len} app_name )

    #message( length: ${app_name_len})
    #message( substr: ${app_name})

    motor_vs_src_dir( ${_prog} ) 

    add_executable( ${app_name} ${_prog} ) 
    target_link_libraries( ${app_name} 
      PRIVATE motor::social
      PRIVATE motor::application
      PRIVATE motor::platform
      PRIVATE motor::io
      PRIVATE nlohmann_json)

    set_target_properties( ${app_name} PROPERTIES FOLDER "programs" )

    set( data_path ${CMAKE_CURRENT_LIST_DIR} ) 
    target_compile_definitions( ${app_name} PRIVATE -DDATAPATH="${data_path}")

    install( TARGETS ${app_name} EXPORT targets 
      RUNTIME DESTINATION bin )

endforeach()


install( EXPORT targets DESTINATION "cmake" )


