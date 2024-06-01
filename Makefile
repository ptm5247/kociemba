SRCDIR = src
BINDIR = bin
TESTDIR = test
INCLUDEDIR = include
CACHEDIR = cache

CC = gcc
CFLAGS = -O3 -msse2 -fopt-info-vec-optimized -march=native

PYTHON = python3

TEST = test
TEST_INPUT = javares.txt

VERSION = 0.2

$(CACHEDIR): $(SRCDIR)/cache.py
	@echo Rewriting Cache...
	@rm -rf $(CACHEDIR)/*
	@$(PYTHON) $(SRCDIR)/cache.py
	@touch $(CACHEDIR)
	@echo

$(BINDIR)/$(TEST): $(TESTDIR)/test.c $(SRCDIR)/*.c $(INCLUDEDIR)/*.h
	@mkdir -p $(BINDIR) $(CACHEDIR)
	@$(CC) $(CFLAGS) -o $@ $(TESTDIR)/test.c $(SRCDIR)/*.c -I$(INCLUDEDIR) 2> $(BINDIR)/optimize

test: $(BINDIR)/$(TEST) $(CACHEDIR) FORCE
	@echo Version $(VERSION)
	@time $(BINDIR)/$(TEST) < $(TESTDIR)/$(TEST_INPUT)

ifneq ($(CACHEDIR),cache)
cache: $(CACHEDIR)
endif

FORCE: ;
