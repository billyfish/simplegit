CMAKE_DIRECTORY=$(abspath CMake)
BUILD_DIRECTORY=$(abspath build)
TESTS_DIRECTORY=$(abspath tests)
LIBGIT2_DIRECTORY=$(abspath libgit2)
LIBGIT2_INCLUDE_DIRECTORY=${LIBGIT2_DIRECTORY}/include
LIBGIT2_BUILD_DIRECTORY=${LIBGIT2_DIRECTORY}/build
CMAKE_MAKEFILE=${BUILD_DIRECTORY}/Makefile
CMAKE_FILE_LISTS=${CMAKE_DIRECTORY}/CMakeLists.txt
BIN_DIRECTORY=$(abspath .)/bin
GIT2=${BIN_DIRECTORY}/sgit_linux

COMMITS := $(shell git rev-list HEAD --count)

main: dirs ${CMAKE_MAKEFILE}
	@rm -rf "${GIT2}";
	@${MAKE} -C "${BUILD_DIRECTORY}";
	cd "${BIN_DIRECTORY}" && ln -sf sgit_linux git-init

.PHONY: dirs
dirs:
	mkdir -p $(BUILD_DIRECTORY)

${LIBGIT2_INCLUDE_DIRECTORY}:
	git submodule init
	git submodule update;

${CMAKE_MAKEFILE}:${LIBGIT2_INCLUDE_DIRECTORY} ${CMAKE_FILE_LISTS}
	mkdir -p "${BUILD_DIRECTORY}"
	echo "#define SIMPLEGIT_REVISION_STRING \"$(COMMITS)\"" >${BUILD_DIRECTORY}/sgit-version.h
	echo "#define SIMPLEGIT_DATE_STRING \"$(shell date +%d.%m.%Y)\"" >>${BUILD_DIRECTORY}/sgit-version.h
	cd "${BUILD_DIRECTORY}" && cmake "${CMAKE_DIRECTORY}" -DCMAKE_C_FLAGS="\
		-imacros ${BUILD_DIRECTORY}/sgit-version.h"

.PHONY:${CMAKE_MAKEFILE}

test:main
	@${MAKE} -C "${TESTS_DIRECTORY}" test;

.DEFAULT:${CMAKE_MAKEFILE}
	@${MAKE} -C "${BUILD_DIRECTORY}" "$@";

clean:
	rm -rf "${BUILD_DIRECTORY}"
#	rm -rf "${LIBGIT2_BUILD_DIRECTORY}"

.PHONY: libgit2
libgit2:
	cd libgit2 && mkdir -p build && cd build && cmake .. -DBUILD_CLAR=0 && $(MAKE)
