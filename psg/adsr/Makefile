SOURCES = main.c ADSR.c timer.c
TEST_SOURCES = test-adsr.c ADSR.c timer.c

PROGRAM = MAIN
TEST_PROGRAM = TEST-ADSR

CC65_TARGET = cx16

CC	= cl65 
CFLAGS 	= --cpu 65c02 -t $(CC65_TARGET) --create-dep $(<:.c=.d) -Ors 
LDFLAGS	= -t $(CC65_TARGET) -m $(PROGRAM).map
OBJDIR  = .obj

#############################################
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:%.c=$(OBJDIR)/%.o)

.SUFFIXES: 
all: $(PROGRAM) $(TEST_PROGRAM)

ifneq (($MAKECMDGOALS),clean)
-include $(SOURCES:.c=.d)
-include $(TEST_SOURCES:.c=.d)
endif

clean:
	$(RM) $(OBJECTS) $(TEST_OBJECTS) $(SOURCES:.c=.d) $(TEST_SOURCES:.c=.d) $(PROGRAM) $(PROGRAM).map $(TEST_PROGRAM) $(TEST_PROGRAM).map

.PHONY: all clean

$(PROGRAM): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TEST_PROGRAM): $(TEST_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJECTS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(TEST_OBJECTS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<


