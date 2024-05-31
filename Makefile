SRCDIR = src
BINDIR = bin
TESTDIR = test
INCLUDEDIR = include
CACHEDIR = cache

CC = gcc
CFLAGS = -O3

TEST = test
TEST_INPUT = javares.txt

VERSION = 0.0

$(BINDIR)/$(TEST): $(TESTDIR)/test.c $(SRCDIR)/* $(INCLUDEDIR)/*
	@mkdir -p $(BINDIR) $(CACHEDIR)
	@$(CC) $(CFLAGS) -o $@ $(TESTDIR)/test.c $(SRCDIR)/* -I$(INCLUDEDIR)

test: $(BINDIR)/$(TEST) FORCE
	@echo Version $(VERSION)
	@time $(BINDIR)/$(TEST) < $(TESTDIR)/$(TEST_INPUT)

clean:
	rm -rf $(CACHEDIR)/*

FORCE: ;
