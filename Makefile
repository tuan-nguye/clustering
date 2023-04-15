
# directories
OBJDIR = ./build
SRCDIR = ./src

# commands
CPP = g++

# list of all cpp files in src directory
CPP_SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# names of subdirectories with source files
VPATH = $(sort $(dir $(CPP_SOURCES)))

# list of all cpp objects
CPP_OBJECTS = $(notdir $(CPP_SOURCES:.cpp=.o))

OBJPRE = $(addprefix $(OBJDIR)/, $(CPP_OBJECTS))

# output file name, outputs .exe on windows and .out on linux
OUTPUT = output

# default target builds the output file and executes it
all: $(OBJDIR)/$(OUTPUT) run

# rules to generate object files
$(OBJDIR)/%.o: %.cpp
	@echo "CPP $@"
	$(CPP) -c $< -o $@

# rule to build output file
$(OBJDIR)/$(OUTPUT): $(OBJPRE)
	$(CPP) $(OBJPRE) -o $@

# clean build directory by deleting all files
clean:
	rm -f $(OBJDIR)/*

# create build directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

run:
	$(OBJDIR)/$(OUTPUT)