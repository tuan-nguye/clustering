# TODO
# - sources directory
# - listen von source files und o files
# - die listen kompilieren
# - dann die .out datei aus den o files bauen

# directories
OBJDIR = ./build
SRCDIR = ./src

# commands
CPP = g++

# list of all cpp files
CPP_SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# list of all cpp objects
CPP_OBJECTS = $(notdir $(CPP_SOURCES:.cpp=.o))

OBJPRE = $(addprefix $(OBJDIR)/, $(CPP_OBJECTS))

# output file name
OUTPUT = output

# default target
all: $(OBJDIR)/$(OUTPUT)

# rules to generate object files

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "CPP $@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(CPP) -c $< -o $@

# rule to build output file
$(OBJDIR)/$(OUTPUT): $(OBJPRE)
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(CPP) $(OBJPRE) -o $@

clean:
	rm -f $(OBJDIR)/*