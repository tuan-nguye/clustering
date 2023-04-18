rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# directories
OBJDIR = ./build
SRCDIR = ./src

CPPFLAGS = -I $(SRCDIR)

# commands
CPP = g++

# list of all cpp files in src directory
CPP_SOURCES = $(call rwildcard, $(SRCDIR), *.cpp)

# names of subdirectories with source files
VPATH = $(sort $(dir $(CPP_SOURCES)))

# list of all cpp objects
CPP_OBJECTS = $(notdir $(CPP_SOURCES:.cpp=.o))

OBJPRE = $(addprefix $(OBJDIR)/, $(CPP_OBJECTS))

# output file name, outputs .exe on windows and .out on linux
OUTPUT = cluster

# default target builds the output file and executes it
all: $(OBJDIR)/$(OUTPUT) run

# rules to generate object files
$(OBJDIR)/%.o: %.cpp
	@echo "CPP $@"
	$(CPP) $(CPPFLAGS) -c $< -o $@

# rule to build output file
$(OBJDIR)/$(OUTPUT): $(OBJPRE)
	$(CPP) $(CPPFLAGS) $(OBJPRE) -o $@

# clean build directory by deleting all files
clean:
	rm -f $(OBJDIR)/*

# create build directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

run:
	$(OBJDIR)/$(OUTPUT)