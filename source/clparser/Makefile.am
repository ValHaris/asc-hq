BUILT_SOURCES = asc.cpp mapedit.cpp

%.cpp %.h: %.gp
	genparse $< -l cpp -c cpp -o $*
