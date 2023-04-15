# TODO
# - sources directory
# - listen von source files und o files
# - die listen kompilieren
# - dann die .out datei aus den o files bauen

# directories
OBJDIR = ./build
SRCDIR = ./src

# list of all cpp files
CPP_SOURCES = $(shell find . -name "*.cpp")

# list of all cpp objects
CPP_OBJECTS = $(notdir $(CPP_SOURCES:.cpp=.o))

output: %.o
	g++ %.o -o output



target: dependencies:
	action
	
