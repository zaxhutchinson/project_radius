################################################################################
### CUSTOMIZE BELOW HERE #######################################################

CC=g++ # define the compiler to use
TARGET=radius # define the name of the executable

# RELEASE
CFLAGS= -c -std=c++20 -O3 -fopenmp -Wall
LFLAGS= -Llib -lm -pthread -fopenmp -pg
# DEBUG
#CFLAGS= -c -std=c++20 -g3 -pg -fopenmp -Wall -DDEBUG
#LFLAGS= -Llib -lm -pthread -fopenmp -pg
SRCDIR = src
OBJDIR = obj
INCLUDE = -Iinc
SOURCES = $(foreach d,$(SRCDIR),$(wildcard $(d)/*.cpp))
OBJECTS = $(foreach d,$(SRCDIR),$(subst $(d),$(OBJDIR),$(wildcard $(d)/*.cpp)))

################################################################################
### DO NOT EDIT THE FOLLOWING LINES ############################################

# define list of objects
OBJS=$(subst .cpp,.o,$(OBJECTS))
# the target is obtained linking all .o files
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -pg -o $@ $^ $(LFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

purge: clean
	rm -f $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

show:
	@echo 'SOURCES:   ' $(SOURCES)
	@echo 'OBJECTS:   ' $(OBJECTS)
	@echo 'OBJS:      ' $(OBJS)

################################################################################
################################################################################

# LFLAGS= -Llib -lm -lsfml-graphics -lsfml-window -lsfml-system -lconfig++ -pthread -fopenmp -pg