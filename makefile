IDIR = ./includes
CC = g++
CXXFLAGS += -I$(IDIR) -std=c++11 -g

ODIR = ./build
CPPDIR = ./src

_DEPS = main.h option.h storyline.h gameController.h extras.h checkCondition.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o option.o storyline.o gameController.o extras.o checkCondition.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(CPPDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

game: $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(INCDIR)/*~ rm game
