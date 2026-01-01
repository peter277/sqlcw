# sqlcw: SQL Code Wrapper - Build Configuration

# Directory structure
BUILD_DIR := $(CURDIR)/build
SRC_DIR := $(CURDIR)/src
OBJ_DIR := $(BUILD_DIR)/obj
BUILD_INFO_FILE := $(BUILD_DIR)/build_info.h
PACKAGE_DIR := $(BUILD_DIR)/package

# Discover all C++ source files in the source directory
SOURCE_FILES := $(wildcard $(SRC_DIR)/*.cpp)

# Generate object file paths by transforming source file paths
# Example: src/main.cpp -> build/obj/main.o
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_FILES))

# External library dependencies
BOOST_LIBS := boost_filesystem boost_program_options

# Build metadata - captured at build time
BUILD_DATE := $(shell date +'%-d %B %Y')
BUILD_COMPILER := $(shell g++ --version | head -n 1)

# Version number from environment variables, defaulting to 0 if not set
BUILD_VERSION_MAJOR := $(or $(ENV_BUILD_VERSION_MAJOR),0)
BUILD_VERSION_MINOR := $(or $(ENV_BUILD_VERSION_MINOR),0)
BUILD_VERSION_PATCH := $(or $(ENV_BUILD_VERSION_PATCH),0)

# Generate version string: "X.Y.Z" if environment vars are set, otherwise "dev"
BUILD_VERSION_STR := $(if $(and $(ENV_BUILD_VERSION_MAJOR),$(ENV_BUILD_VERSION_MINOR)),$(BUILD_VERSION_MAJOR).$(BUILD_VERSION_MINOR).$(BUILD_VERSION_PATCH),dev)

# Alternative: Pass build info as preprocessor definitions instead of build_info.h
# Uncomment these lines and remove $(BUILD_INFO_FILE) from compilation prerequisites
#DEFINE_VARS := BUILD_DATE BUILD_COMPILER BUILD_VERSION_STR
#CPPFLAGS += $(foreach VAR, $(DEFINE_VARS), -D$(VAR)=\"$(value $(VAR))\")

# Platform-specific configuration
ifdef MSYSTEM
	# Building on Windows (MSYS2/MinGW)
	PLATFORM := win64
	TARGET := $(BUILD_DIR)/sqlcw.exe
	# Boost libraries on Windows use -mt suffix (multi-threaded)
	LIBS := $(addsuffix -mt, $(addprefix -l,$(BOOST_LIBS)))
else
	# Building on Linux/Unix
	PLATFORM := linux_$(shell uname -m)
	TARGET := $(BUILD_DIR)/sqlcw
	LIBS := $(addprefix -l,$(BOOST_LIBS))
endif

# Phony targets (not actual files)
.PHONY: all clean package .build-windres

# Default target: build executable and create package
all: $(TARGET) package

# Link object files into final executable
# Note: We explicitly list prerequisites rather than using $^ because .build-windres
# is a phony target that shouldn't be passed to the linker
# $@ = target name
$(TARGET): $(OBJ_FILES) .build-windres | $(BUILD_DIR)
	g++ $(OBJ_FILES) $(WINDRES_OBJECT_FILE) -static $(LIBS) -o $@ -O3 -s

# Compile each C++ source file into an object file
# $< = first prerequisite (source file)
# $@ = target (object file)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_INFO_FILE) | $(OBJ_DIR)
	g++ -include $(BUILD_INFO_FILE) -c $< -o $@ -O3 -Wall

# Create object files directory (order-only prerequisite)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create build directory (order-only prerequisite)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate build_info.h header with build metadata
$(BUILD_INFO_FILE): | $(BUILD_DIR)
	@echo "Generating build_info.h..."
	@echo "#ifndef _BUILD_INFO_H" > $(BUILD_INFO_FILE)
	@echo "#define _BUILD_INFO_H" >> $(BUILD_INFO_FILE)
	@echo "#define BUILD_DATE \"$(BUILD_DATE)\"" >> $(BUILD_INFO_FILE)
	@echo "#define BUILD_VERSION_STR \"$(BUILD_VERSION_STR)\"" >> $(BUILD_INFO_FILE)
	@echo "#define BUILD_VERSION_MAJOR $(BUILD_VERSION_MAJOR)" >> $(BUILD_INFO_FILE)
	@echo "#define BUILD_VERSION_MINOR $(BUILD_VERSION_MINOR)" >> $(BUILD_INFO_FILE)
	@echo "#define BUILD_VERSION_PATCH $(BUILD_VERSION_PATCH)" >> $(BUILD_INFO_FILE)
	@echo "#define BUILD_COMPILER \"$(BUILD_COMPILER)\"" >> $(BUILD_INFO_FILE)
	@echo "#endif" >> $(BUILD_INFO_FILE)

# Compile Windows resource file (icon, version info, etc.)
.build-windres: $(BUILD_INFO_FILE)
ifdef MSYSTEM
	windres -I $(BUILD_DIR) -i $(SRC_DIR)/sqlcw.rc -o $(OBJ_DIR)/windres.o
WINDRES_OBJECT_FILE := $(OBJ_DIR)/windres.o
else
WINDRES_OBJECT_FILE :=
endif

# Package application executable and documentation for distribution
package: $(TARGET)
	@echo "Generating documentation and creating distribution packages..."
	BUILD_VERSION=$(BUILD_VERSION_STR) pandoc README.md -o $(BUILD_DIR)/README.html --embed-resources --standalone  --lua-filter=scripts/pandoc-link-rewriter.lua
	BUILD_VERSION=$(BUILD_VERSION_STR) pandoc LICENSE.md -o $(BUILD_DIR)/LICENSE.html --embed-resources --standalone  --lua-filter=scripts/pandoc-link-rewriter.lua

	mkdir -p $(PACKAGE_DIR)/bin
	cp $(TARGET) $(PACKAGE_DIR)/bin
	cp $(BUILD_DIR)/README.html $(BUILD_DIR)/LICENSE.html -t $(PACKAGE_DIR)
	cp -r doc/library-licenses doc/examples -t $(PACKAGE_DIR)
	cd $(PACKAGE_DIR) && zip -r $(BUILD_DIR)/sqlcw-$(BUILD_VERSION_STR)-$(PLATFORM).zip .
	rm -rf $(PACKAGE_DIR)

ifeq ($(OS),Windows_NT)
	iscc -Obuild -Q installer/sqlcw_installer.iss
endif

# Remove all generated files
clean:
	@echo "Cleaning generated files..."
	@rm -rf $(BUILD_DIR)
