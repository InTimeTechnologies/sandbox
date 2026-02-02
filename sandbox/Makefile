# Compiler and flags
CXX ?= g++-15
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O0

# Debug arguments: -g -O0 -Wall -Wextra
# Release arguments: -O2 -DNDEBUG -Wall -Wextra
# Profiling: -pg -O2

# Directories
PROJECT_DIR := $(CURDIR)
PROJECT_NAME := $(notdir $(patsubst %/,%,$(PROJECT_DIR)))

SOLUTION_DIR := $(patsubst %/,%,$(dir $(CURDIR)))
SOLUTION_NAME := $(shell basename $(SOLUTION_DIR))

BUILD_DIR ?= $(PROJECT_DIR)/build
BUILD_DIR_NAME := $(notdir $(patsubst %/,%,$(BUILD_DIR)))

INSTALL_DIR ?= $(PROJECT_DIR)/install
INSTALL_DIR_NAME := $(notdir $(patsubst %/,%,$(INSTALL_DIR)))

# C/C++ compiler
INCLUDE_DIRS := "$(PROJECT_DIR)" "$(SOLUTION_DIR)/dependencies/glm" "$(SOLUTION_DIR)/dependencies/stb"
LIBRARY_DIRS :=
LIBRARIES :=
DEFINITIONS := STB_IMAGE_IMPLEMENTATION _CRT_SECURE_NO_WARNINGS STB_IMAGE_WRITE_IMPLEMENTATION

# Output
OUTPUT_FILE_NAME ?= $(PROJECT_NAME).bin
OUTPUT_FILE_PATH := $(INSTALL_DIR)/$(OUTPUT_FILE_NAME)

# SRC files excluded from build (relative to PROJECT_DIR)
EXCLUDED_SRC_FILE_PATHS :=

# SRC
SRC_FILE_PATHS := $(shell find $(PROJECT_DIR) \( -name "*.c" -o -name "*.cpp" \))
SRC_FILE_PATHS := $(filter-out $(EXCLUDED_SRC_FILE_PATHS),$(SRC_FILE_PATHS))
SRC_LOCAL_FILE_PATHS := $(subst $(PROJECT_DIR)/,,$(SRC_FILE_PATHS))
SRC_FILES := $(notdir $(SRC_LOCAL_FILE_PATHS))
SRC_DEPENDENCIES_DIR := $(PROJECT_DIR)/src_dependencies

# Object
OBJECT_FILE_PATHS := $(addprefix $(BUILD_DIR)/,$(SRC_LOCAL_FILE_PATHS:.cpp=.o))
OBJECT_FILE_PATHS := $(OBJECT_FILE_PATHS:.c=.o)
OBJECT_LOCAL_FILE_PATHS := $(SRC_LOCAL_FILE_PATHS:.cpp=.o)
OBJECT_LOCAL_FILE_PATHS := $(OBJECT_LOCAL_FILE_PATHS:.c=.o)
OBJECT_FILES := $(SRC_FILES:.cpp=.o)
OBJECT_FILES := $(OBJECT_FILES:.c=.o)
-include $(OBJECT_FILE_PATHS:.o=.d)

# Debugging
VAR ?= NULL
NULL := null
TEST := $(NULL)

build_project: $(OBJECT_FILE_PATHS)
	@mkdir -p $(INSTALL_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECT_FILE_PATHS) $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -D,$(DEFINITIONS)) -o $(INSTALL_DIR)/$(OUTPUT_FILE_NAME)
	chmod +x $(OUTPUT_FILE_PATH)

.PHONY: rebuild_project
rebuild_project: clean build_project

# $@ is the target
# $< is the first prerequisite
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -D,$(DEFINITIONS)) -MMD -MP -c "$<" -o "$@"
	@echo Compiled "$<" to "$@"
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -D,$(DEFINITIONS)) -MMD -MP -c "$<" -o "$@"

$(SRC_DEPENDENCIES_DIR)/%.d: %.cpp
	@echo "$<" "$@"


.PHONY: run
run: build_project
	$(OUTPUT_FILE_PATH)

.PHONY: run_whithout_building
run_whithout_building:
	$(OUTPUT_FILE_PATH)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(INSTALL_DIR)

#print: VAR = $(PROJECT_DIR)
print_var:
	@echo "$(VAR) = $($(VAR))"
	@echo "--------------------"
	@printf "%s\n" $($(VAR))

test:
	@echo "TEST = $(OBJECT_FILE_PATHS:.o=.d)"
