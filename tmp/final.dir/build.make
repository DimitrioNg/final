# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_SOURCE_DIR = /home/box/final

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/box/final/tmp

# Include any dependencies generated for this target.
include CMakeFiles/../final.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/../final.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/../final.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/../final.dir/flags.make

CMakeFiles/../final.dir/src/main.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/../final.dir/src/main.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/../final.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/main.cpp.o -MF CMakeFiles/../final.dir/src/main.cpp.o.d -o CMakeFiles/../final.dir/src/main.cpp.o -c /home/box/final/src/main.cpp

CMakeFiles/../final.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/main.cpp > CMakeFiles/../final.dir/src/main.cpp.i

CMakeFiles/../final.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/main.cpp -o CMakeFiles/../final.dir/src/main.cpp.s

CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o: ../src/lib/daemonizator.cpp
CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o -MF CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o.d -o CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o -c /home/box/final/src/lib/daemonizator.cpp

CMakeFiles/../final.dir/src/lib/daemonizator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/lib/daemonizator.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/lib/daemonizator.cpp > CMakeFiles/../final.dir/src/lib/daemonizator.cpp.i

CMakeFiles/../final.dir/src/lib/daemonizator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/lib/daemonizator.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/lib/daemonizator.cpp -o CMakeFiles/../final.dir/src/lib/daemonizator.cpp.s

CMakeFiles/../final.dir/src/lib/errorproc.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/lib/errorproc.cpp.o: ../src/lib/errorproc.cpp
CMakeFiles/../final.dir/src/lib/errorproc.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/../final.dir/src/lib/errorproc.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/lib/errorproc.cpp.o -MF CMakeFiles/../final.dir/src/lib/errorproc.cpp.o.d -o CMakeFiles/../final.dir/src/lib/errorproc.cpp.o -c /home/box/final/src/lib/errorproc.cpp

CMakeFiles/../final.dir/src/lib/errorproc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/lib/errorproc.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/lib/errorproc.cpp > CMakeFiles/../final.dir/src/lib/errorproc.cpp.i

CMakeFiles/../final.dir/src/lib/errorproc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/lib/errorproc.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/lib/errorproc.cpp -o CMakeFiles/../final.dir/src/lib/errorproc.cpp.s

CMakeFiles/../final.dir/src/lib/unblock.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/lib/unblock.cpp.o: ../src/lib/unblock.cpp
CMakeFiles/../final.dir/src/lib/unblock.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/../final.dir/src/lib/unblock.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/lib/unblock.cpp.o -MF CMakeFiles/../final.dir/src/lib/unblock.cpp.o.d -o CMakeFiles/../final.dir/src/lib/unblock.cpp.o -c /home/box/final/src/lib/unblock.cpp

CMakeFiles/../final.dir/src/lib/unblock.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/lib/unblock.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/lib/unblock.cpp > CMakeFiles/../final.dir/src/lib/unblock.cpp.i

CMakeFiles/../final.dir/src/lib/unblock.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/lib/unblock.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/lib/unblock.cpp -o CMakeFiles/../final.dir/src/lib/unblock.cpp.s

CMakeFiles/../final.dir/src/lib/worker.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/lib/worker.cpp.o: ../src/lib/worker.cpp
CMakeFiles/../final.dir/src/lib/worker.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/../final.dir/src/lib/worker.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/lib/worker.cpp.o -MF CMakeFiles/../final.dir/src/lib/worker.cpp.o.d -o CMakeFiles/../final.dir/src/lib/worker.cpp.o -c /home/box/final/src/lib/worker.cpp

CMakeFiles/../final.dir/src/lib/worker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/lib/worker.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/lib/worker.cpp > CMakeFiles/../final.dir/src/lib/worker.cpp.i

CMakeFiles/../final.dir/src/lib/worker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/lib/worker.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/lib/worker.cpp -o CMakeFiles/../final.dir/src/lib/worker.cpp.s

CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o: ../src/lib/fdtransceiver.cpp
CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o -MF CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o.d -o CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o -c /home/box/final/src/lib/fdtransceiver.cpp

CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/lib/fdtransceiver.cpp > CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.i

CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/lib/fdtransceiver.cpp -o CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.s

CMakeFiles/../final.dir/src/lib/webproc.cpp.o: CMakeFiles/../final.dir/flags.make
CMakeFiles/../final.dir/src/lib/webproc.cpp.o: ../src/lib/webproc.cpp
CMakeFiles/../final.dir/src/lib/webproc.cpp.o: CMakeFiles/../final.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/../final.dir/src/lib/webproc.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/../final.dir/src/lib/webproc.cpp.o -MF CMakeFiles/../final.dir/src/lib/webproc.cpp.o.d -o CMakeFiles/../final.dir/src/lib/webproc.cpp.o -c /home/box/final/src/lib/webproc.cpp

CMakeFiles/../final.dir/src/lib/webproc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/../final.dir/src/lib/webproc.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/box/final/src/lib/webproc.cpp > CMakeFiles/../final.dir/src/lib/webproc.cpp.i

CMakeFiles/../final.dir/src/lib/webproc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/../final.dir/src/lib/webproc.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/box/final/src/lib/webproc.cpp -o CMakeFiles/../final.dir/src/lib/webproc.cpp.s

# Object files for target ../final
__/final_OBJECTS = \
"CMakeFiles/../final.dir/src/main.cpp.o" \
"CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o" \
"CMakeFiles/../final.dir/src/lib/errorproc.cpp.o" \
"CMakeFiles/../final.dir/src/lib/unblock.cpp.o" \
"CMakeFiles/../final.dir/src/lib/worker.cpp.o" \
"CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o" \
"CMakeFiles/../final.dir/src/lib/webproc.cpp.o"

# External object files for target ../final
__/final_EXTERNAL_OBJECTS =

../final: CMakeFiles/../final.dir/src/main.cpp.o
../final: CMakeFiles/../final.dir/src/lib/daemonizator.cpp.o
../final: CMakeFiles/../final.dir/src/lib/errorproc.cpp.o
../final: CMakeFiles/../final.dir/src/lib/unblock.cpp.o
../final: CMakeFiles/../final.dir/src/lib/worker.cpp.o
../final: CMakeFiles/../final.dir/src/lib/fdtransceiver.cpp.o
../final: CMakeFiles/../final.dir/src/lib/webproc.cpp.o
../final: CMakeFiles/../final.dir/build.make
../final: CMakeFiles/../final.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/box/final/tmp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable ../final"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/../final.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/../final.dir/build: ../final
.PHONY : CMakeFiles/../final.dir/build

CMakeFiles/../final.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/../final.dir/cmake_clean.cmake
.PHONY : CMakeFiles/../final.dir/clean

CMakeFiles/../final.dir/depend:
	cd /home/box/final/tmp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/box/final /home/box/final /home/box/final/tmp /home/box/final/tmp /home/box/final/tmp/final.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/../final.dir/depend
