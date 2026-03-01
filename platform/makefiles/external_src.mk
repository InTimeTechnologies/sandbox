# Arguments passed from the main Makefile
CXX_COMPILE ?=
SRC ?=
OBJS ?=

# Error check: arguments must not be empty
ifeq ($(CXX_COMPILE),)
$(error CXX_COMPILE is empty)
endif

# Ensure SRC and OBJS have the same number of elements
ifeq ($(words $(SRC)),$(words $(OBJS)))
else
$(error SRC and OBJS must have the same number of elements)
endif

compile: $(OBJS)

#.PHONY: check
#check:
#	@if [ -z "$(SRC)" ]; then \
#		echo "No source files specified for external src. Skipping build_external_src target."; \
#		exit 0; \
#	fi

# SRC and OBJS 1:1 rule macro
define COMPILE_TEMPLATE
$2: $1
	@mkdir -p $(dir $2)
	$(CXX_COMPILE) $1 -o $2
endef

# Generate rules for each SRC and OBJS pair
$(foreach i,$(shell seq 1 $(words $(SRC))), \
	$(eval $(call COMPILE_TEMPLATE, \
		$(word $(i),$(SRC)), \
		$(word $(i),$(OBJS)) \
	)) \
)

# Include dependency files
-include $(OBJS:.o=.d)
