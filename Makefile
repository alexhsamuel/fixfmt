GTEST_DIR       = ./googletest/googletest
GTEST_INCDIR    = $(GTEST_DIR)/include
GTEST_LIB       = $(GTEST_DIR)/make/gtest_main.a

CPPFLAGS        = -I./cxx
CXXFLAGS        = -std=c++14 -g
LDLIBS          = -lpthread

SOURCES         = $(wildcard cxx/fixfmt/*.cc) \
	          $(wildcard cxx/fixfmt/double-conversion/*.cc)
DEPS            = $(SOURCES:%.cc=%.dd)
OBJS            = $(SOURCES:%.cc=%.o)
LIB	    	= cxx/libfixfmt.a
BINS            = $(SOURCES:%.cc=%)

TEST_SOURCES    = $(wildcard test/*.cc)
TEST_DEPS       = $(TEST_SOURCES:%.cc=%.dd)
TEST_OBJS       = $(TEST_SOURCES:%.cc=%.o)
TEST_BINS       = $(TEST_SOURCES:%.cc=%.exe)
TEST_OKS        = $(TEST_SOURCES:%.cc=%.ok)

TEST_CPPFLAGS   = $(CPPFLAGS) -I$(GTEST_INCDIR)
TEST_LIBS       = $(GTEST_LIB)

PYTHON	    	= python3
PYTEST	    	= py.test

#-------------------------------------------------------------------------------

.PHONY: all
all:			$(LIB) test

.PHONY: test
test:			test-cxx test-python

.PHONY: clean
clean:			clean-cxx clean-python testclean

.PHONY: testclean
testclean:		testclean-cxx testclean-python

#-------------------------------------------------------------------------------
# C++

$(DEPS): %.dd: 		%.cc
	@echo "generating $@"; set -e; \
	$(CXX) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

.PHONY: clean-cxx
clean-cxx:
	rm -f $(OBJS) $(BINS) $(DEPS) $(OKS)

.PHONY: testclean
testclean-cxx:
	rm -f $(TEST_DEPS) $(TEST_OBJS) $(TEST_BINS) $(TEST_OKS)

.PHONY: test-cxx
test-cxx: $(TEST_OKS)

$(LIB):			$(OBJS)
	ar -r $@ $<

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
# Python

.PHONY: python
python:
	cd python; $(PYTHON) setup.py build_ext --inplace

.PHONY: clean-python
clean-python:
	rm -rf python/build python/fixfmt/*.so

.PHONY: test-python
test-python: 
	$(PYTEST) python

.PHONY: testclean-python
testclean-python:

#-------------------------------------------------------------------------------

include $(DEPS) $(TEST_DEPS)

