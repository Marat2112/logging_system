# Project configuration
PROJECT := logging-system
VERSION := 1.0.0

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude -fPIC
LDFLAGS := -shared
LDLIBS := -lpthread

# Build directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

# Targets
LIBRARY := liblogger.so
APP := logger_app

# Colors
GREEN := \033[0;32m
NC := \033[0m

.PHONY: all clean install uninstall info

all: $(LIBRARY) $(APP)
	@echo "$(GREEN)[SUCCESS]$(NC) Project built successfully!"

$(LIBRARY): $(SRC_DIR)/logger.cpp $(INCLUDE_DIR)/logger.h
	@echo "$(GREEN)[BUILD]$(NC) Building library..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/$@ $<
	@ln -sf $(BUILD_DIR)/$@ .
	@echo "$(GREEN)[LIBRARY]$(NC) Built: $(BUILD_DIR)/$@"

$(APP): $(SRC_DIR)/app.cpp $(LIBRARY)
	@echo "$(GREEN)[BUILD]$(NC) Building application..."
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@ $< -L. -llogger $(LDLIBS)
	@ln -sf $(BUILD_DIR)/$@ .
	@echo "$(GREEN)[APP]$(NC) Built: $(BUILD_DIR)/$@"

install: $(LIBRARY)
	@echo "$(GREEN)[INSTALL]$(NC) Installing library..."
	@sudo cp $(BUILD_DIR)/$(LIBRARY) /usr/local/lib
	@sudo ldconfig
	@echo "$(GREEN)[SUCCESS]$(NC) Library installed to /usr/local/lib"

uninstall:
	@echo "$(GREEN)[UNINSTALL]$(NC) Removing library..."
	@sudo rm -f /usr/local/lib/$(LIBRARY)
	@sudo ldconfig
	@echo "$(GREEN)[SUCCESS]$(NC) Library uninstalled"

clean:
	@echo "$(GREEN)[CLEAN]$(NC) Removing build artifacts..."
	@rm -rf $(BUILD_DIR) $(LIBRARY) $(APP)
	@echo "$(GREEN)[SUCCESS]$(NC) Clean complete"

info:
	@echo "$(GREEN)[INFO]$(NC) Project: $(PROJECT) v$(VERSION)"
	@echo "$(GREEN)[INFO]$(NC) Compiler: $(shell $(CXX) --version | head -n1)"
	@echo "$(GREEN)[INFO]$(NC) Flags: $(CXXFLAGS)"
