# Detect platform
UNAME := $(shell uname)

BIN_DIR := ./bin

ifeq ($(UNAME), Linux)
    PLATFORM := Linux
    CXXFLAGS += -DLINUX
    TARGET := $(BIN_DIR)/main
    INSTALL_DIR := /usr/local/bin
    RESOURCE_DIR := /usr/local/share/aifilesorter
else ifeq ($(UNAME), Darwin)
    PLATFORM := MacOS
    CXXFLAGS += -DMACOS
    TARGET := $(BIN_DIR)/main
    INSTALL_DIR := /usr/local/bin
    RESOURCE_DIR := /usr/local/share/aifilesorter
else ifeq ($(UNAME), MINGW32_NT)
    PLATFORM := Windows (32-bit)
    CXXFLAGS += -DWINDOWS
    LDFLAGS += -lwinlib32
    TARGET := $(BIN_DIR)/main.exe
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
    PLATFORM := Linux
    CXXFLAGS += -DLINUX
    TARGET := $(BIN_DIR)/main
    INSTALL_DIR := /usr/local/bin
    RESOURCE_DIR := /usr/local/share/aifilesorter
else ifeq ($(UNAME), Darwin)
    PLATFORM := MacOS
    CXXFLAGS += -DMACOS
    TARGET := $(BIN_DIR)/main
    INSTALL_DIR := /usr/local/bin
    RESOURCE_DIR := /usr/local/share/aifilesorter
else ifeq ($(UNAME), MINGW32_NT)
    PLATFORM := Windows (32-bit)
    CXXFLAGS += -DWINDOWS
    LDFLAGS += -lwinlib32
    TARGET := $(BIN_DIR)/main.exe
    INSTALL_DIR := C:/Program\ Files\ \(x86\)/AiFileSorter
    RESOURCE_DIR := C:/Program\ Files\ \(x86\)/AiFileSorter/resources
else ifeq ($(UNAME), MINGW64_NT)
    PLATFORM := Windows (64-bit)
    CXXFLAGS += -DWINDOWS
    LDFLAGS += -lwinlib64
    TARGET := $(BIN_DIR)/main.exe
    INSTALL_DIR := C:/Program\ Files/AiFileSorter
    RESOURCE_DIR := C:/Program\ Files/AiFileSorter/resources
endif

# Compiler and flags
CXX = g++
CXXFLAGS += -std=c++20 -Wall $(shell pkg-config --cflags gtkmm-3.0)
LDFLAGS += $(shell pkg-config --libs gtkmm-3.0) -lcurl -ljsoncpp -lsqlite3 -lcrypto -lfmt -lspdlog -lssl
INCLUDE_DIRS = -I./include

# Directories
SRC_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
RESOURCES = resources/resources.c

# Source files
SRCS = main.cpp $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

.PHONY: all clean install uninstall

# Main rules
all: $(TARGET)
	@echo "Finished building AI File Sorter for $(PLATFORM)"

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(RESOURCES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

install: $(TARGET)
ifeq ($(PLATFORM), Linux)
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter
	cp -r resources/* $(RESOURCE_DIR)
else ifeq ($(PLATFORM), MacOS)
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter
	cp -r resources/* $(RESOURCE_DIR)
else ifeq ($(PLATFORM), Windows (32-bit))
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter.exe
	cp -r resources/* $(RESOURCE_DIR)
else ifeq ($(PLATFORM), Windows (64-bit))
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter.exe
	cp -r resources/* $(RESOURCE_DIR)
endif

uninstall:
ifeq ($(PLATFORM), Linux)
	rm -f $(INSTALL_DIR)/aifilesorter
	rm -rf $(RESOURCE_DIR)
else ifeq ($(PLATFORM), MacOS)
	rm -f $(INSTALL_DIR)/aifilesorter
	rm -rf $(RESOURCE_DIR)
else ifeq ($(PLATFORM), Windows (32-bit))
	rm -rf $(INSTALL_DIR)
else ifeq ($(PLATFORM), Windows (64-bit))
	rm -rf $(INSTALL_DIR)
endif
    INSTALL_DIR := C:/Program\ Files\ \(x86\)/AiFileSorter
    RESOURCE_DIR := C:/Program\ Files\ \(x86\)/AiFileSorter/resources
else ifeq ($(UNAME), MINGW64_NT)
    PLATFORM := Windows (64-bit)
    CXXFLAGS += -DWINDOWS
    LDFLAGS += -lwinlib64
    TARGET := $(BIN_DIR)/main.exe
    INSTALL_DIR := C:/Program\ Files/AiFileSorter
    RESOURCE_DIR := C:/Program\ Files/AiFileSorter/resources
endif

# Compiler and flags
CXX = g++
CXXFLAGS += -std=c++20 -Wall $(shell pkg-config --cflags gtkmm-3.0)
LDFLAGS += $(shell pkg-config --libs gtkmm-3.0) -lcurl -ljsoncpp -lsqlite3 -lcrypto -lfmt -lspdlog -lssl
INCLUDE_DIRS = -I./include

# Directories
SRC_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
RESOURCES = resources/resources.c

# Source files
SRCS = main.cpp $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

.PHONY: all clean install uninstall

# Main rules
all: $(TARGET)
	@echo "Building for $(PLATFORM)"

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(RESOURCES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

install: $(TARGET)
ifeq ($(PLATFORM), Linux)
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter
	cp -r resources/* $(RESOURCE_DIR)
else ifeq ($(PLATFORM), MacOS)
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter
	cp -r resources/* $(RESOURCE_DIR)
else ifeq ($(PLATFORM), Windows (32-bit))
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter.exe
	cp -r resources/* $(RESOURCE_DIR)
else ifeq ($(PLATFORM), Windows (64-bit))
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(RESOURCE_DIR)
	cp $(TARGET) $(INSTALL_DIR)/aifilesorter.exe
	cp -r resources/* $(RESOURCE_DIR)
endif

uninstall:
ifeq ($(PLATFORM), Linux)
	rm -f $(INSTALL_DIR)/aifilesorter
	rm -rf $(RESOURCE_DIR)
else ifeq ($(PLATFORM), MacOS)
	rm -f $(INSTALL_DIR)/aifilesorter
	rm -rf $(RESOURCE_DIR)
else ifeq ($(PLATFORM), Windows (32-bit))
	rm -rf $(INSTALL_DIR)
else ifeq ($(PLATFORM), Windows (64-bit))
	rm -rf $(INSTALL_DIR)
endif