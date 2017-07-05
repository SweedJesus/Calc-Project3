# @author Nils Olsson

CXX=g++
CXXSTANDARD=c++11

# Generic flags
CXXWARN=-Wall -Wextra -Wpedantic
#CXXWARN=-Wno-unused-variable
CXXFLAGS=-std=$(CXXSTANDARD) $(CXXWARN)
LDFLAGS=-I/usr/local/include -lreadline

# Comment these out if boost not provided a precompiled libs
#BOOST_PO= -lboost_program_options
#BOOST_UT= -lboost_unit_test_framework-mt

define link=
@echo -e "\e[31m- Linking\e[0m $@"
$(CXX) $(LDFLAGS) -o $@ $^
endef

define compile=
@echo -e "\e[33m- Compiling\e[0m $@"
$(CXX) $(CXXFLAGS) -c $< -o $@
endef

define making=
@echo -e "\e[34m- Making $@\e[0m"
endef

define done=
@echo -e "\e[32m- Done!\e[0m"
endef

define do=
$(call making)
$(call link)
$(call done)
endef

all: Calc

#BigInt_test: BigInt.cpp BigInt_test.cpp
BigInt_test: BigInt_test.cpp
	$(call making)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(BOOST_UT) -o $@ $^
	$(call done)

Calc: BigInt.cpp main.cpp
	$(call making)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(BOOST_UT) -o $@ $^
	$(call done)

.cpp.o:
	$(call compile)

.PHONY: clean remove

clean:
	@echo -e "\e[33m-- Clean\e[0m"
	find . -type f -name "*.o" -delete

remove:
	@echo -e "\e[33m-- Remove\e[0m"
	find . -type f -executable -delete
