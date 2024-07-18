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
CMAKE_SOURCE_DIR = /home/minho/optee_qemu/optee_examples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/minho/optee_qemu/optee_examples

# Include any dependencies generated for this target.
include secure_storage/CMakeFiles/optee_example_secure_storage.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include secure_storage/CMakeFiles/optee_example_secure_storage.dir/compiler_depend.make

# Include the progress variables for this target.
include secure_storage/CMakeFiles/optee_example_secure_storage.dir/progress.make

# Include the compile flags for this target's objects.
include secure_storage/CMakeFiles/optee_example_secure_storage.dir/flags.make

secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.o: secure_storage/CMakeFiles/optee_example_secure_storage.dir/flags.make
secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.o: secure_storage/host/main.c
secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.o: secure_storage/CMakeFiles/optee_example_secure_storage.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/minho/optee_qemu/optee_examples/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.o"
	cd /home/minho/optee_qemu/optee_examples/secure_storage && ccache /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.o -MF CMakeFiles/optee_example_secure_storage.dir/host/main.c.o.d -o CMakeFiles/optee_example_secure_storage.dir/host/main.c.o -c /home/minho/optee_qemu/optee_examples/secure_storage/host/main.c

secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/optee_example_secure_storage.dir/host/main.c.i"
	cd /home/minho/optee_qemu/optee_examples/secure_storage && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/minho/optee_qemu/optee_examples/secure_storage/host/main.c > CMakeFiles/optee_example_secure_storage.dir/host/main.c.i

secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/optee_example_secure_storage.dir/host/main.c.s"
	cd /home/minho/optee_qemu/optee_examples/secure_storage && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/minho/optee_qemu/optee_examples/secure_storage/host/main.c -o CMakeFiles/optee_example_secure_storage.dir/host/main.c.s

# Object files for target optee_example_secure_storage
optee_example_secure_storage_OBJECTS = \
"CMakeFiles/optee_example_secure_storage.dir/host/main.c.o"

# External object files for target optee_example_secure_storage
optee_example_secure_storage_EXTERNAL_OBJECTS =

secure_storage/optee_example_secure_storage: secure_storage/CMakeFiles/optee_example_secure_storage.dir/host/main.c.o
secure_storage/optee_example_secure_storage: secure_storage/CMakeFiles/optee_example_secure_storage.dir/build.make
secure_storage/optee_example_secure_storage: secure_storage/CMakeFiles/optee_example_secure_storage.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/minho/optee_qemu/optee_examples/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable optee_example_secure_storage"
	cd /home/minho/optee_qemu/optee_examples/secure_storage && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/optee_example_secure_storage.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
secure_storage/CMakeFiles/optee_example_secure_storage.dir/build: secure_storage/optee_example_secure_storage
.PHONY : secure_storage/CMakeFiles/optee_example_secure_storage.dir/build

secure_storage/CMakeFiles/optee_example_secure_storage.dir/clean:
	cd /home/minho/optee_qemu/optee_examples/secure_storage && $(CMAKE_COMMAND) -P CMakeFiles/optee_example_secure_storage.dir/cmake_clean.cmake
.PHONY : secure_storage/CMakeFiles/optee_example_secure_storage.dir/clean

secure_storage/CMakeFiles/optee_example_secure_storage.dir/depend:
	cd /home/minho/optee_qemu/optee_examples && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/minho/optee_qemu/optee_examples /home/minho/optee_qemu/optee_examples/secure_storage /home/minho/optee_qemu/optee_examples /home/minho/optee_qemu/optee_examples/secure_storage /home/minho/optee_qemu/optee_examples/secure_storage/CMakeFiles/optee_example_secure_storage.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : secure_storage/CMakeFiles/optee_example_secure_storage.dir/depend

