# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jaeger/lsquic-fork

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jaeger/lsquic-fork

# Include any dependencies generated for this target.
include test/unittests/CMakeFiles/test_frame_reader.dir/depend.make

# Include the progress variables for this target.
include test/unittests/CMakeFiles/test_frame_reader.dir/progress.make

# Include the compile flags for this target's objects.
include test/unittests/CMakeFiles/test_frame_reader.dir/flags.make

test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o: test/unittests/CMakeFiles/test_frame_reader.dir/flags.make
test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o: test/unittests/test_frame_reader.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jaeger/lsquic-fork/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o"
	cd /home/jaeger/lsquic-fork/test/unittests && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o   -c /home/jaeger/lsquic-fork/test/unittests/test_frame_reader.c

test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_frame_reader.dir/test_frame_reader.c.i"
	cd /home/jaeger/lsquic-fork/test/unittests && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jaeger/lsquic-fork/test/unittests/test_frame_reader.c > CMakeFiles/test_frame_reader.dir/test_frame_reader.c.i

test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_frame_reader.dir/test_frame_reader.c.s"
	cd /home/jaeger/lsquic-fork/test/unittests && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jaeger/lsquic-fork/test/unittests/test_frame_reader.c -o CMakeFiles/test_frame_reader.dir/test_frame_reader.c.s

test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.requires:

.PHONY : test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.requires

test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.provides: test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.requires
	$(MAKE) -f test/unittests/CMakeFiles/test_frame_reader.dir/build.make test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.provides.build
.PHONY : test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.provides

test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.provides.build: test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o


# Object files for target test_frame_reader
test_frame_reader_OBJECTS = \
"CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o"

# External object files for target test_frame_reader
test_frame_reader_EXTERNAL_OBJECTS =

test/unittests/test_frame_reader: test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o
test/unittests/test_frame_reader: test/unittests/CMakeFiles/test_frame_reader.dir/build.make
test/unittests/test_frame_reader: src/liblsquic/liblsquic.a
test/unittests/test_frame_reader: test/unittests/CMakeFiles/test_frame_reader.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jaeger/lsquic-fork/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_frame_reader"
	cd /home/jaeger/lsquic-fork/test/unittests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_frame_reader.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/unittests/CMakeFiles/test_frame_reader.dir/build: test/unittests/test_frame_reader

.PHONY : test/unittests/CMakeFiles/test_frame_reader.dir/build

test/unittests/CMakeFiles/test_frame_reader.dir/requires: test/unittests/CMakeFiles/test_frame_reader.dir/test_frame_reader.c.o.requires

.PHONY : test/unittests/CMakeFiles/test_frame_reader.dir/requires

test/unittests/CMakeFiles/test_frame_reader.dir/clean:
	cd /home/jaeger/lsquic-fork/test/unittests && $(CMAKE_COMMAND) -P CMakeFiles/test_frame_reader.dir/cmake_clean.cmake
.PHONY : test/unittests/CMakeFiles/test_frame_reader.dir/clean

test/unittests/CMakeFiles/test_frame_reader.dir/depend:
	cd /home/jaeger/lsquic-fork && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jaeger/lsquic-fork /home/jaeger/lsquic-fork/test/unittests /home/jaeger/lsquic-fork /home/jaeger/lsquic-fork/test/unittests /home/jaeger/lsquic-fork/test/unittests/CMakeFiles/test_frame_reader.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/unittests/CMakeFiles/test_frame_reader.dir/depend

