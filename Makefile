# set compiler and linker here
CC		:= gcc
LD		:= gcc
STRIPCMD	:= strip

# set main target name here (name of executeable/shared object
TARGET		:= gasm

# set name of test executeable executing ALL tests
TESTTARGET	:= test.testexe

# set cmd of leak-test executable
LEAK_CMD	:= gasm rv32i_reducedSet.json example.i

# dir config
BUILDDIR	:= build
SRCDIR		:= src
DEPDIR		:= .deps
LIBDIR		:= lib
TESTDIR		:= test

# set this to anything other than "" to enable on-demand search of srcs
# (i.e. for code-generating targets) WILL DECREASE PERFORMANCE
ONDEMAND	:=

# tool config
DEBUGFLAGS	:= -g3
INCLUDES	:=
LINKS		:= json-c c
CFLAGS		:= $(DEBUGFLAGS) -flto -Wall -Wextra -Werror -g -O0 -fPIC -fvisibility=hidden -Wno-error=discarded-qualifiers
LDFLAGS		:=
MEMTESTOPTS	:= --tool=memcheck --leak-check=yes
STRIPOPTS	:= --strip-unneeded
ASANOPTS	:= -fsanitize=address
GPROFOPTS	:= -pg

# tool cmds
RM		:= rm -rf
CP		:= cp
VALGRIND	:= valgrind

# ADD TARGETS TO DEFAULT TARGETS
# TARGETS ADDED TO CLEAN WILL BE MARKED AS PHONY
BUILD_TARGETADD	:=
CLEAN_TARGETADD	:=

# PROJECT SPECIFIC CONFIG

################################################################################
###################### END OF CONFIGURATION SECTION ############################
################################################################################

# FUNCTIONS
# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

################################################################################
##################### START OF USERSPECIFIED TARGETS ###########################
################ set USRDIRS for directory creation rules ######################
################################################################################

.PHONY: install run
install: all
	@echo install is not supported yet

run: all
	./${TARGET}

################################################################################
####################### END OF USERSPECIFIC TARGETS ############################
################################################################################
######################### START OF SYSTEM TARGETS ##############################
################################################################################

.PHONY: all clean clear dep-clean test test-clean test-leak echo $(CLEAN_TARGETADD) $(TARGET)_BUILD
.DEFAULT_GOAL := all

# add src-dir to includes
INCLUDES := $(INCLUDES) $(SRCDIR)

# generate correct cmd-options
INCLUDES := $(addprefix -I,$(INCLUDES))
LINKS := $(addprefix -l,$(LINKS))

# creates depfiles to be included
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
DEPFLAGSASAN = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.asan.d
DEPFLAGSGPROF = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.gprof.d
DEPFLAGSSO = -E -M -MT $(@:$(DEPDIR)/%.d=$(BUILDDIR)/%.o) -MG -MP -MF $@

# commands for build/link/strip
BUILD = $(CC) $(CFLAGS) $(INCLUDES) -c -o
LINK := $(LD) $(CFLAGS) $(LDFLAGS) $(LINKS) -o
STRIP := $(STRIPCMD) $(STRIPOPTS) -o

# command needed when autogenerating code for dep-generation
MKDEP = $(CC) $(DEPFLAGSSO) $(INCLUDES)

# testcommands
MEMTEST := $(VALGRIND) $(MEMTESTOPTS)

# cwd
MAKEFILE := $(abspath $(MAKEFILE_LIST))
PROJECTROOT := $(dir $(MAKEFILE))

ifeq (strip $(ONDEMAND),)
	# list of sources
	SRCS := $(call rwildcard,$(SRCDIR),*.c)

	# generate list of objects and deps
	OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
	DEPS := $(patsubst $(SRCDIR)/%.c,$(DEPDIR)/%.d,$(SRCS))

	OBJS_ASAN := $(patsubst %.o,%.asan.o,$(OBJS))
	DEPS_ASAN := $(patsubst %.d,%.asan.d,$(DEPS))

	OBJS_GPROF := $(patsubst %.o,%.gprof.o,$(OBJS))
	DEPS_GPROF := $(patsubst %.d,%.gprof.d,$(DEPS))

	DEPDIRS := $(sort $(dir $(DEPS)))
	OBJDIRS := $(sort $(dir $(OBJS)))
else
	# list of sources
	SRCS = $(call rwildcard,$(SRCDIR),*.c)

	# generate list of objects and deps
	OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
	DEPS = $(patsubst $(SRCDIR)/%.c,$(DEPDIR)/%.d,$(SRCS))

	OBJS_ASAN = $(patsubst %.o,%.asan.o,$(OBJS))
	DEPS_ASAN = $(patsubst %.d,%.asan.d,$(DEPS))

	OBJS_GPROF = $(patsubst %.o,%.gprof.o,$(OBJS))
	DEPS_GPROF = $(patsubst %.d,%.gprof.d,$(DEPS))

	DEPDIRS = $(sort $(dir $(DEPS)))
	OBJDIRS = $(sort $(dir $(OBJS)))
endif

# main (and default) build target
all: $(TARGET)_BUILD $(TARGET).stripped $(TARGET).asan $(TARGET).gprof
	@echo "################################################"
	@echo reached target all

clean clear: test-clean dep-clean $(CLEAN_TARGETADD)
	$(RM) $(TARGET) $(TARGET).stripped $(BUILDDIR)

echo:
	@echo "SRCS = {" $(SRCS) "}"
	@echo "DEPS = {" $(DEPS) "}"
	@echo "GENDIRS = {" $(GENDIRS) "}"
	@echo
	@echo "DEPDIR = " $(DEPDIR)

dep-clean:
	$(RM) $(DEPDIR)
$(TARGET)_BUILD: $(TARGET) $(BUILD_TARGETADD)
$(TARGET): $(OBJS)
	@echo
	@echo linking $@
	$(LINK) $@ $^

$(TARGET).stripped $(TARGET).asan $(TARGET).gprof:

ifneq ($(strip $(STRIPOPTS)),)
$(TARGET).stripped: $(TARGET)
	@echo
	@echo stripping $< into $@
	$(STRIP) $@ $<
endif

ifneq ($(strip $(ASANOPTS)),)
$(OBJS_ASAN): $(BUILDDIR)/%.asan.o: $(SRCDIR)/%.c | $(OBJDIRS)
$(OBJS_ASAN): $(BUILDDIR)/%.asan.o: $(SRCDIR)/%.c $(DEPDIR)/%.asan.d $(MAKEFILE) | $(DEPDIRS)
	$(BUILD) $@ $(DEPFLAGSASAN) $(ASANOPTS) $<

$(TARGET).asan: $(OBJS_ASAN)
	@echo
	@echo linking $@
	$(LINK) $@ $(ASANOPTS) $^
endif

ifneq ($(strip $(GPROFOPTS)),)
$(OBJS_GPROF): $(BUILDDIR)/%.gprof.o: $(SRCDIR)/%.c | $(OBJDIRS)
$(OBJS_GPROF): $(BUILDDIR)/%.gprof.o: $(SRCDIR)/%.c $(DEPDIR)/%.gprof.d $(MAKEFILE) | $(DEPDIRS)
	$(BUILD) $@ $(DEPFLAGSGPROF) $(GPROFOPTS) $<

$(TARGET).gprof: $(OBJS_GPROF)
	@echo
	@echo linking $@
	$(LINK) $@ $(GPROFOPTS) $^
endif

$(OBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIRS)
$(OBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d $(MAKEFILE) | $(DEPDIRS)
	$(BUILD) $@ $(DEPFLAGS) $<

GENDIRS = $(DEPDIRS) $(USRDIRS) $(OBJDIRS)

$(GENDIRS):
	@mkdir -p $@

# testing
test-clean:
	$(RM) $(TESTTARGET)

test: all $(TESTTARGET)
	@echo running all tests
	@./$(TESTTARGET)

$(TESTTARGET): $(call rwildcard,$(TESTDIR),*.c) $(OBJS)
	$(CC) $(INCLUDES) $(DEBUGFLAGS) $(LINKS) -o $@ $^

test-leak: $(TESTTARGET)
	$(MEMTEST) ./$(LEAK_CMD) 2>&1 | less

#deps
$(DEPS) $(DEPS_ASAN) $(DEPS_GPROF):
include $(DEPS) $(DEPS_ASAN) $(DEPS_GPROF)
