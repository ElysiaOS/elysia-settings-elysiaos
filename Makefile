# Makefile for ElysiaOS Settings App

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -march=x86-64-v2 -mtune=generic
LDFLAGS=-Wl,-z,x86-64-v2 -Wl,--no-as-needed
PKG_CONFIG = pkg-config

# GTK4 and NetworkManager flags
GTK_CFLAGS = $(shell $(PKG_CONFIG) --cflags gtk4)
GTK_LIBS = $(shell $(PKG_CONFIG) --libs gtk4)
NM_CFLAGS = $(shell $(PKG_CONFIG) --cflags libnm)
NM_LIBS = $(shell $(PKG_CONFIG) --libs libnm)

# Target executable
TARGET = ElysiaSettings

# Source files (all in main directory)
SOURCES = main.cpp MainWindow.cpp components/AboutManager.cpp components/StorageManager.cpp components/NetworkManager.cpp components/BluetoothManager.cpp components/SoundManager.cpp components/AppearanceManager.cpp components/BatteryManager.cpp components/DisplayManager.cpp components/PowerManager.cpp components/ApplicationsManager.cpp components/LanguageManager.cpp translations/translations.cpp translations/en_US.cpp translations/zh_CN.cpp translations/ja_JP.cpp translations/ko_KR.cpp translations/ru_RU.cpp translations/de_DE.cpp translations/fr_FR.cpp translations/vi_VN.cpp translations/id_ID.cpp translations/es_ES.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(GTK_LIBS) $(NM_LIBS) -o $(TARGET)
	@echo "Fixing x86-64 ISA level requirements..."
	@objcopy --remove-section=.note.gnu.property $@
	@echo "Binary is now compatible with x86-64-v2 CPUs"

# Compile source files
%.o: %.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(GTK_CFLAGS) $(NM_CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install target (optional)
install: $(TARGET)
	install -D $(TARGET) /usr/local/bin/$(TARGET)

# Uninstall target (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Create components directory for future use
setup:
	mkdir -p components

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Run the application
run: $(TARGET)
	./$(TARGET)

# Check if dependencies are installed
check-deps:
	@echo "Checking dependencies..."
	@$(PKG_CONFIG) --exists gtk4 && echo "GTK4: OK" || (echo "GTK4: NOT FOUND - Please install libgtk-4-dev" && exit 1)
	@$(PKG_CONFIG) --exists libnm && echo "NetworkManager: OK" || (echo "NetworkManager: NOT FOUND - Please install libnm-dev" && exit 1)
	@which pactl > /dev/null && echo "PulseAudio pactl: OK" || (echo "PulseAudio pactl: NOT FOUND - Please install pulseaudio-utils" && exit 1)

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the application (default)"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  setup    - Create components directory"
	@echo "  debug    - Build with debug symbols"
	@echo "  run      - Build and run the application"
	@echo "  check-deps - Check if dependencies are installed"
	@echo "  help     - Show this help message"

.PHONY: all clean install uninstall setup debug run check-deps help