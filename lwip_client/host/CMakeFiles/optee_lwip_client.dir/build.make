# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/minho/optee_qemu/optee_examples/lwip_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/minho/optee_qemu/optee_examples/lwip_client/host

# Include any dependencies generated for this target.
include CMakeFiles/optee_lwip_client.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/optee_lwip_client.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/optee_lwip_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/optee_lwip_client.dir/flags.make

CMakeFiles/optee_lwip_client.dir/main.o: CMakeFiles/optee_lwip_client.dir/flags.make
CMakeFiles/optee_lwip_client.dir/main.o: main.c
CMakeFiles/optee_lwip_client.dir/main.o: CMakeFiles/optee_lwip_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/minho/optee_qemu/optee_examples/lwip_client/host/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/optee_lwip_client.dir/main.o"
	/usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/optee_lwip_client.dir/main.o -MF CMakeFiles/optee_lwip_client.dir/main.o.d -o CMakeFiles/optee_lwip_client.dir/main.o -c /home/minho/optee_qemu/optee_examples/lwip_client/host/main.c

CMakeFiles/optee_lwip_client.dir/main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/optee_lwip_client.dir/main.i"
	/usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/minho/optee_qemu/optee_examples/lwip_client/host/main.c > CMakeFiles/optee_lwip_client.dir/main.i

CMakeFiles/optee_lwip_client.dir/main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/optee_lwip_client.dir/main.s"
	/usr/bin/aarch64-linux-gnu-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/minho/optee_qemu/optee_examples/lwip_client/host/main.c -o CMakeFiles/optee_lwip_client.dir/main.s

# Object files for target optee_lwip_client
optee_lwip_client_OBJECTS = \
"CMakeFiles/optee_lwip_client.dir/main.o"

# External object files for target optee_lwip_client
optee_lwip_client_EXTERNAL_OBJECTS =

optee_lwip_client: CMakeFiles/optee_lwip_client.dir/main.o
optee_lwip_client: CMakeFiles/optee_lwip_client.dir/build.make
optee_lwip_client: CMakeFiles/optee_lwip_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/minho/optee_qemu/optee_examples/lwip_client/host/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable optee_lwip_client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/optee_lwip_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/optee_lwip_client.dir/build: optee_lwip_client
.PHONY : CMakeFiles/optee_lwip_client.dir/build

CMakeFiles/optee_lwip_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/optee_lwip_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/optee_lwip_client.dir/clean

CMakeFiles/optee_lwip_client.dir/depend:
	cd /home/minho/optee_qemu/optee_examples/lwip_client/host && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/minho/optee_qemu/optee_examples/lwip_client /home/minho/optee_qemu/optee_examples/lwip_client /home/minho/optee_qemu/optee_examples/lwip_client/host /home/minho/optee_qemu/optee_examples/lwip_client/host /home/minho/optee_qemu/optee_examples/lwip_client/host/CMakeFiles/optee_lwip_client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/optee_lwip_client.dir/depend

