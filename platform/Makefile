# ---------------------------------------------------------------------------------------------------------------------
# Project and solution directories
# ---------------------------------------------------------------------------------------------------------------------
# Project
PROJECT_DIR := $(CURDIR)
PROJECT_NAME := $(notdir $(patsubst %/,%,$(PROJECT_DIR)))

# Solution
SOLUTION_DIR := $(patsubst %/,%,$(dir $(CURDIR)))
SOLUTION_NAME := $(shell basename $(SOLUTION_DIR))

# ---------------------------------------------------------------------------------------------------------------------
# Compiler commands, flags and directories
# ---------------------------------------------------------------------------------------------------------------------
# Compiler
CXX ?= g++-15
PLATFORM ?= $(shell $(CXX) -dumpmachine)
CPP_STD ?= c++20
C_STD ?= c11
WARNINGS ?= all extra
OPTIMIZATION ?= 0

# Compiler arguments
ARG_CPP_STD := $(addprefix -std=,$(CPP_STD))
ARG_C_STD := $(addprefix -std=,$(C_STD))
ARG_WARNINGS := $(addprefix -W,$(WARNINGS))
ARG_OPTIMIZATION := $(addprefix -O,$(OPTIMIZATION))
ARG_DEBUG_SYMBOLS ?=
ifeq ($(DEBUG_SYMBOLS),)
ifeq ($(OPTIMIZATION),0)
ARG_DEBUG_SYMBOLS := -g
endif
endif
ARG_FLAGS ?= $(ARG_CPP_STD) $(ARG_C_STD) $(ARG_WARNINGS) $(ARG_OPTIMIZATION) $(ARG_DEBUG_SYMBOLS)

# Debug arguments: -g -O0 -Wall -Wextra
# Release arguments: -O2 -DNDEBUG -Wall -Wextra
# Profiling: -pg -O2

# Out directories
INTERMIDIARY_DIR ?= intermidiary/$(PLATFORM)/O$(OPTIMIZATION)
BUILD_DIR ?= build/$(PLATFORM)/O$(OPTIMIZATION)

# Definitions, header include, library directory include and library include
DEFINITIONS := STB_IMAGE_IMPLEMENTATION _CRT_SECURE_NO_WARNINGS STB_IMAGE_WRITE_IMPLEMENTATION
INCLUDE_DIRS := "$(PROJECT_DIR)"
LIBRARY_DIRS :=
LIBRARIES := glfw

# Output
OUTPUT_NAME ?= $(PROJECT_NAME).bin
OUTPUT_PATH := $(BUILD_DIR)/$(OUTPUT_NAME)

# ---------------------------------------------------------------------------------------------------------------------
# SRC
# ---------------------------------------------------------------------------------------------------------------------
# src
SRC := $(shell find $(PROJECT_DIR) \( -name "*.c" -o -name "*.cpp" \))
SRC := $(subst $(PROJECT_DIR)/,,$(SRC))
# remove specified src files
ifneq ($(SRC_EXCLUDED),)
SRC := $(filter-out $(SRC_EXCLUDED),$(SRC))
endif
# remove src files within excluded directories
ifneq ($(SRC_EXCLUDED_DIR),)
SRC := $(filter-out $(SRC_EXCLUDED_DIR)%,$(SRC))
endif
SRC_DEPENDENCIES_DIR := $(PROJECT_DIR)/src_dependencies
# src files excluded from build (relative to PROJECT_DIR)
SRC_EXCLUDED_DIR ?=
SRC_EXCLUDED ?=
# external src files
SRC_EXTERNAL_DIR ?=
SRC_EXTERNAL ?=
ifneq ($(SRC_EXTERNAL_DIR),)
SRC_EXTERNAL := $(SRC_EXTERNAL) $(shell find $(SRC_EXTERNAL_DIR) \( -name "*.c" -o -name "*.cpp" \))
endif
SRC_EXTERNAL := $(sort $(SRC_EXTERNAL))

# ---------------------------------------------------------------------------------------------------------------------
# Object
# ---------------------------------------------------------------------------------------------------------------------
# project objects
OBJS := $(SRC:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(addprefix $(INTERMIDIARY_DIR)/,$(OBJS))
# external objects
OBJS_EXTERNAL := 
ifneq ($(SRC_EXTERNAL),)
OBJS_EXTERNAL := $(INTERMIDIARY_DIR)/$(notdir $(SRC_EXTERNAL:.cpp=.o))
endif

# ---------------------------------------------------------------------------------------------------------------------
# Compiler commands
# ---------------------------------------------------------------------------------------------------------------------
# Compile and link commands
CXX_COMPILE := $(PLATFORM)-$(CXX) $(ARG_FLAGS) $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -D,$(DEFINITIONS)) $(addprefix -L,$(LIBRARY_DIRS)) $(addprefix -l,$(LIBRARIES)) -MMD -MP -c
CXX_LINK := $(PLATFORM)-$(CXX) $(ARG_FLAGS) $(OBJS) $(OBJS_EXTERNAL) $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -D,$(DEFINITIONS)) $(addprefix -L,$(LIBRARY_DIRS)) $(addprefix -l,$(LIBRARIES)) -o

# Debugging
NULL := null
TEST := $(NULL)
VAR ?= $(NULL)

build_project: $(OBJS) build_external_src
	@mkdir -p $(BUILD_DIR)
	@echo Compiling $($(OBJS))
	$(CXX_LINK) $(OUTPUT_PATH)
	chmod +x $(OUTPUT_PATH)
build_external_src:
	@echo Building external src
	$(MAKE) -f makefiles/external_src.mk CXX_COMPILE="$(CXX_COMPILE)" SRC="$(SRC_EXTERNAL)" OBJS="$(OBJS_EXTERNAL)"
	@echo Finished building external src
-include $(OBJS:.o=.d)

.PHONY: rebuild_project
rebuild_project: clean build_project

# $@ is the target
# $< is the first prerequisite
$(INTERMIDIARY_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX_COMPILE) "$<" -o "$@"
$(INTERMIDIARY_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX_COMPILE) "$<" -o "$@"

$(SRC_DEPENDENCIES_DIR)/%.d: %.cpp
	@echo "$<" "$@"

.PHONY: run
run: build_project
	$(OUTPUT_PATH)

.PHONY: run_whithout_building
run_whithout_building:
	$(OUTPUT_PATH)

.PHONY: clean
clean:
	rm -rf $(word 1, $(subst /, ,$(BUILD_DIR))) $(word 1, $(subst /, ,$(INTERMIDIARY_DIR)))

# Print variable and its contents
# example usage: make print VAR=PROJECT_DIR
print_var:
	@echo "$(VAR) = $($(VAR))"
	@echo "--------------------"
	@printf "%s\n" $($(VAR))

.PHONEY: test
test:
	@echo Test: "$(word 1, $(subst /, ,$(BUILD_DIR)))"
