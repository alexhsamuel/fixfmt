GTEST_DIR       = ./googletest/googletest
GTEST_INCDIR    = $(GTEST_DIR)/include
GTEST_LIB       = $(GTEST_DIR)/make/gtest_main.a

CPPFLAGS        = -I./src
CXXFLAGS        = -std=c++14 -g
LDLIBS          = -lpthread

SOURCES         = $(wildcard src/*.cc)
DEPS            = $(SOURCES:%.cc=%.dd)
OBJS            = $(SOURCES:%.cc=%.o)
BINS            = $(SOURCES:%.cc=%)

TEST_SOURCES    = $(wildcard test/*.cc)
TEST_DEPS       = $(TEST_SOURCES:%.cc=%.dd)
TEST_OBJS       = $(TEST_SOURCES:%.cc=%.o)
TEST_BINS       = $(TEST_SOURCES:%.cc=%.exe)
TEST_OKS        = $(TEST_SOURCES:%.cc=%.ok)

TEST_CPPFLAGS   = $(CPPFLAGS) -I$(GTEST_INCDIR)
TEST_LIBS       = $(GTEST_LIB)

PYTHON	    	= python3

#-------------------------------------------------------------------------------

.PHONY: all
all:			test 

.PHONY: clean
clean:			test-clean
	rm -f $(OBJS) $(BINS) $(DEPS) $(OKS)

$(DEPS): %.dd: 		%.cc
	@echo "generating $@"; set -e; \
	$(CXX) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

#-------------------------------------------------------------------------------

.PHONY: test-clean
test-clean:
	rm -f $(TEST_DEPS) $(TEST_OBJS) $(TEST_BINS) $(TEST_OKS)

.PHONY: test
test: $(TEST_OKS)

$(TEST_DEPS): \
%.dd: 			%.cc
	@echo "generating $@"; \
	set -e; $(CXX) -MM $(TEST_CPPFLAGS) $< | sed -E 's#([^ ]+:)#test/\1#g' > $@

$(TEST_OBJS): \
%.o: 	    	    	%.cc
	$(CXX) $(TEST_CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TEST_BINS): \
%.exe: 	    	    	%.o $(TEST_LIBS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(TEST_LIBS) $(LDLIBS) -o $@

$(TEST_OKS): \
%.ok:    	    	%.exe
	@rm -f $@
	$< && touch $@

#-------------------------------------------------------------------------------

.PHONY: python
python:
	cd python; $(PYTHON) setup.py build_ext --inplace

.PHONY: python-clean
python-clean:
	rm -rf python/build python/*.so

#-------------------------------------------------------------------------------

include $(DEPS) $(TEST_DEPS)
