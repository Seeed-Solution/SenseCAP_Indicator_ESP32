# Custom function to collect source files from a specified directory
function(collect_sources source_var)
  # Initialize the sources variable
  set(sources "")

  # Iterate over the directories to include
  foreach(dir IN LISTS ARGN)
    # Recursively glob all `.c` files in the directory
    file(GLOB_RECURSE dir_sources "${dir}/*.c")

    # Append the files to the sources variable
    list(APPEND sources ${dir_sources})
  endforeach()

  # Set the sources variable in the parent scope
  set(${source_var} ${sources} PARENT_SCOPE)
endfunction()

# Define a list of directories to include
set(DIRECTORIES_TO_INCLUDE "util" "storage" "display_ctrl" "./button" "./ui" "./esp32_rp2040" "wifi" "datetime" "sensor" "lorahub" "terminal" "./lora_pkt")

# Call the custom function to collect the source files
collect_sources(COLLECTED_SOURCES ${DIRECTORIES_TO_INCLUDE})

# Register the component
idf_component_register(
  SRCS "main.c" "lv_port.c" "indicator_model.c" "indicator_view.c" ${COLLECTED_SOURCES}
  INCLUDE_DIRS "." ${DIRECTORIES_TO_INCLUDE}
  EMBED_TXTFILES timeapi_cert.pem
)