# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/fabrice/Detect

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fabrice/Detect/Build

# Include any dependencies generated for this target.
include CMakeFiles/Detect.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Detect.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Detect.dir/flags.make

CMakeFiles/Detect.dir/src/detect_tag.cpp.o: CMakeFiles/Detect.dir/flags.make
CMakeFiles/Detect.dir/src/detect_tag.cpp.o: ../src/detect_tag.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/fabrice/Detect/Build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Detect.dir/src/detect_tag.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Detect.dir/src/detect_tag.cpp.o -c /home/fabrice/Detect/src/detect_tag.cpp

CMakeFiles/Detect.dir/src/detect_tag.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Detect.dir/src/detect_tag.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/fabrice/Detect/src/detect_tag.cpp > CMakeFiles/Detect.dir/src/detect_tag.cpp.i

CMakeFiles/Detect.dir/src/detect_tag.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Detect.dir/src/detect_tag.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/fabrice/Detect/src/detect_tag.cpp -o CMakeFiles/Detect.dir/src/detect_tag.cpp.s

CMakeFiles/Detect.dir/src/detect_tag.cpp.o.requires:
.PHONY : CMakeFiles/Detect.dir/src/detect_tag.cpp.o.requires

CMakeFiles/Detect.dir/src/detect_tag.cpp.o.provides: CMakeFiles/Detect.dir/src/detect_tag.cpp.o.requires
	$(MAKE) -f CMakeFiles/Detect.dir/build.make CMakeFiles/Detect.dir/src/detect_tag.cpp.o.provides.build
.PHONY : CMakeFiles/Detect.dir/src/detect_tag.cpp.o.provides

CMakeFiles/Detect.dir/src/detect_tag.cpp.o.provides.build: CMakeFiles/Detect.dir/src/detect_tag.cpp.o

CMakeFiles/Detect.dir/src/main.cpp.o: CMakeFiles/Detect.dir/flags.make
CMakeFiles/Detect.dir/src/main.cpp.o: ../src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/fabrice/Detect/Build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Detect.dir/src/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Detect.dir/src/main.cpp.o -c /home/fabrice/Detect/src/main.cpp

CMakeFiles/Detect.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Detect.dir/src/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/fabrice/Detect/src/main.cpp > CMakeFiles/Detect.dir/src/main.cpp.i

CMakeFiles/Detect.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Detect.dir/src/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/fabrice/Detect/src/main.cpp -o CMakeFiles/Detect.dir/src/main.cpp.s

CMakeFiles/Detect.dir/src/main.cpp.o.requires:
.PHONY : CMakeFiles/Detect.dir/src/main.cpp.o.requires

CMakeFiles/Detect.dir/src/main.cpp.o.provides: CMakeFiles/Detect.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Detect.dir/build.make CMakeFiles/Detect.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/Detect.dir/src/main.cpp.o.provides

CMakeFiles/Detect.dir/src/main.cpp.o.provides.build: CMakeFiles/Detect.dir/src/main.cpp.o

# Object files for target Detect
Detect_OBJECTS = \
"CMakeFiles/Detect.dir/src/detect_tag.cpp.o" \
"CMakeFiles/Detect.dir/src/main.cpp.o"

# External object files for target Detect
Detect_EXTERNAL_OBJECTS =

Detect: CMakeFiles/Detect.dir/src/detect_tag.cpp.o
Detect: CMakeFiles/Detect.dir/src/main.cpp.o
Detect: CMakeFiles/Detect.dir/build.make
Detect: /usr/local/lib/libopencv_videostab.so.3.0.0
Detect: /usr/local/lib/libopencv_videoio.so.3.0.0
Detect: /usr/local/lib/libopencv_video.so.3.0.0
Detect: /usr/local/lib/libopencv_ts.a
Detect: /usr/local/lib/libopencv_superres.so.3.0.0
Detect: /usr/local/lib/libopencv_stitching.so.3.0.0
Detect: /usr/local/lib/libopencv_shape.so.3.0.0
Detect: /usr/local/lib/libopencv_photo.so.3.0.0
Detect: /usr/local/lib/libopencv_objdetect.so.3.0.0
Detect: /usr/local/lib/libopencv_ml.so.3.0.0
Detect: /usr/local/lib/libopencv_imgproc.so.3.0.0
Detect: /usr/local/lib/libopencv_imgcodecs.so.3.0.0
Detect: /usr/local/lib/libopencv_highgui.so.3.0.0
Detect: /usr/local/lib/libopencv_flann.so.3.0.0
Detect: /usr/local/lib/libopencv_features2d.so.3.0.0
Detect: /usr/local/lib/libopencv_core.so.3.0.0
Detect: /usr/local/lib/libopencv_calib3d.so.3.0.0
Detect: /usr/local/lib/libopencv_features2d.so.3.0.0
Detect: /usr/local/lib/libopencv_ml.so.3.0.0
Detect: /usr/local/lib/libopencv_highgui.so.3.0.0
Detect: /usr/local/lib/libopencv_videoio.so.3.0.0
Detect: /usr/local/lib/libopencv_imgcodecs.so.3.0.0
Detect: /usr/local/lib/libopencv_flann.so.3.0.0
Detect: /usr/local/lib/libopencv_video.so.3.0.0
Detect: /usr/local/lib/libopencv_imgproc.so.3.0.0
Detect: /usr/local/lib/libopencv_core.so.3.0.0
Detect: CMakeFiles/Detect.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable Detect"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Detect.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Detect.dir/build: Detect
.PHONY : CMakeFiles/Detect.dir/build

CMakeFiles/Detect.dir/requires: CMakeFiles/Detect.dir/src/detect_tag.cpp.o.requires
CMakeFiles/Detect.dir/requires: CMakeFiles/Detect.dir/src/main.cpp.o.requires
.PHONY : CMakeFiles/Detect.dir/requires

CMakeFiles/Detect.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Detect.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Detect.dir/clean

CMakeFiles/Detect.dir/depend:
	cd /home/fabrice/Detect/Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fabrice/Detect /home/fabrice/Detect /home/fabrice/Detect/Build /home/fabrice/Detect/Build /home/fabrice/Detect/Build/CMakeFiles/Detect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Detect.dir/depend

