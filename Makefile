# TODO
# - sources directory
# - listen von source files und o files
# - die listen kompilieren
# - dann die .out datei aus den o files bauen


OBJDIR = ./build

output: %.o
	g++ %.o -o output



target: dependencies:
	action
	
