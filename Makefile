SRCDIR = src
BINDIR = bin
TESTDIR = test
INCLUDEDIR = include
CACHEDIR = cache

CC = gcc
CFLAGS = -Ofast -march=native -msse2

TEST = test
TEST_INPUT = javares.txt

VERSION = 0.5

test: $(BINDIR)/$(TEST) $(CACHEDIR) FORCE
	@echo Version $(VERSION)
	@time $(BINDIR)/$(TEST) < $(TESTDIR)/$(TEST_INPUT)

$(CACHEDIR): $(SRCDIR)/gen-h-tables.c $(SRCDIR)/gen-m-tables.c
	@echo Rewriting Cache...
	@$(CC) $(CFLAGS) -o $(BINDIR)/move $(SRCDIR)/gen-m-tables.c $(SRCDIR)/tables.c $(SRCDIR)/cube.c -I$(INCLUDEDIR)
	@$(CC) $(CFLAGS) -o $(BINDIR)/heuristic $(SRCDIR)/gen-h-tables.c $(SRCDIR)/tables.c -I$(INCLUDEDIR)
	@rm -rf $(CACHEDIR)/*
	@$(BINDIR)/move
	@$(BINDIR)/heuristic
	@touch $(CACHEDIR)

$(BINDIR)/$(TEST): $(TESTDIR)/test.c $(SRCDIR)/*.c $(INCLUDEDIR)/*.h
	@mkdir -p $(BINDIR) $(CACHEDIR)
	@$(CC) $(CFLAGS) -o $@ $(TESTDIR)/test.c $(SRCDIR)/search.c $(SRCDIR)/cube.c $(SRCDIR)/tables.c -I$(INCLUDEDIR)

ifneq ($(CACHEDIR),cache)
cache: $(CACHEDIR)
endif

FORCE: ;
