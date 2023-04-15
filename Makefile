
# directories
OBJDIR = ./build
SRCDIR = ./src

# commands
CPP = g++

# list of all cpp files
CPP_SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# names of subdirectories with source files
VPATH = $(sort $(dir $(CPP_SOURCES)))

# list of all cpp objects
CPP_OBJECTS = $(notdir $(CPP_SOURCES:.cpp=.o))

OBJPRE = $(addprefix $(OBJDIR)/, $(CPP_OBJECTS))

# output file name
OUTPUT = output

# default target
all: $(OBJDIR)/$(OUTPUT)

# rules to generate object files

$(OBJDIR)/%.o: %.cpp
	@echo "CPP $@"
	$(CPP) -c $< -o $@

# rule to build output file
$(OBJDIR)/$(OUTPUT): $(OBJPRE)
	$(CPP) $(OBJPRE) -o $@

clean:
	rm -f $(OBJDIR)/*

# create build directory if it doesn't exits
$(shell mkdir -p $(OBJDIR))