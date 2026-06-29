# Makefile for Black Hole Travel Simulator
# Alternative to CMake for quick builds

# Compiler
CXX := g++
CC := gcc

# Directories
SFML_DIR := C:/Dev/SFML-2.6.2
GLAD_DIR := external/glad
GLAD_SRC := external/glad/src

# Flags
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
CFLAGS := -std=c11
INCLUDES := -I$(SFML_DIR)/include -I$(GLAD_DIR)/include -I.
LDFLAGS := -L$(SFML_DIR)/lib
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lgdi32

# Build type (Debug or Release)
BUILD_TYPE ?= Debug

ifeq ($(BUILD_TYPE),Debug)
    CXXFLAGS += -g -O0 -DDEBUG
    CFLAGS += -g -O0 -DDEBUG
else
    CXXFLAGS += -O3 -march=native -DNDEBUG
    CFLAGS += -O3 -march=native -DNDEBUG
endif

# Sanitizers (optional)
ifdef ASAN
    CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address
endif

ifdef UBSAN
    CXXFLAGS += -fsanitize=undefined
    LDFLAGS += -fsanitize=undefined
endif

# Target
TARGET := BlackHoleTravelSimulator.exe

# Source files
SOURCES := \
    main.cpp \
    Body.cpp \
    Camera.cpp \
    CelestialBody.cpp \
    CollisionPredictor.cpp \
    CoordinateGrid.cpp \
    EngineParticles.cpp \
    Entity.cpp \
    GalaxyBackground.cpp \
    HohmannTransfer.cpp \
    LagrangePoints.cpp \
    NebulaBackground.cpp \
    OrbitalMechanicsHUD.cpp \
    OrbitalPath.cpp \
    PhysicsEngine.cpp \
    PlanetaryRings.cpp \
    PredictionMarker.cpp \
    Renderer.cpp \
    Shader.cpp \
    SolarSystemFactory.cpp \
    Spaceship.cpp \
    Sphere.cpp \
    StarField.cpp \
    StoryNarrator.cpp \
    TextureLoader.cpp \
    Trail.cpp \
    VectorRenderer.cpp

C_SOURCES := $(GLAD_SRC)/glad.c

# Object files
OBJECTS := $(SOURCES:.cpp=.o)
C_OBJECTS := $(notdir $(C_SOURCES:.c=.o))

# Rules
.PHONY: all clean run debug release help

all: $(TARGET)

$(TARGET): $(OBJECTS) $(C_OBJECTS)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "Build complete: $@"

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

glad.o: $(GLAD_SRC)/glad.c
	@echo "Compiling GLAD..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -f $(OBJECTS) $(C_OBJECTS) $(TARGET)
	@echo "Clean complete."

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

debug:
	$(MAKE) BUILD_TYPE=Debug

release:
	$(MAKE) BUILD_TYPE=Release

asan:
	$(MAKE) BUILD_TYPE=Debug ASAN=1

ubsan:
	$(MAKE) BUILD_TYPE=Debug UBSAN=1

help:
	@echo "Black Hole Travel Simulator - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make              Build in Debug mode"
	@echo "  make release      Build in Release mode"
	@echo "  make debug        Build in Debug mode (explicit)"
	@echo "  make run          Build and run"
	@echo "  make clean        Clean build files"
	@echo "  make asan         Build with Address Sanitizer"
	@echo "  make ubsan        Build with UB Sanitizer"
	@echo "  make help         Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make clean && make release && make run"
	@echo "  make asan run"
