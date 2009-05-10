#
# OpenURL os agnostic makefile
#

# OpenURL
TARGET = OpenURL
DEST   = C:

# Where to look for
VPATH=$(ODIR)

# Objs
OBJS += OpenURL.o SmartReadArgs.o

FOBJS =$(addprefix $(ODIR)/,$(OBJS))

# Options
CDEFS   +=
CWARN   += -Wall -Wextra -Werror -Wno-missing-braces -Wno-parentheses -Wno-switch -Wstrict-prototypes -Werror-implicit-function-declaration -Wpointer-arith
CFLAGS  += $(CDEFS) $(CWARN) -g -O2 -fomit-frame-pointer -mcpu=750 -mstring -mmultiple \
          -fstrict-aliasing -I../include/
LDFLAGS +=

# Dependencies options
DEPFILE=makefile.dep
DEPOPTS=
DEPINCS=-I../include/

# Main target
.PHONY: all
ifeq ($(wildcard $(DEPFILE)),$(DEPFILE))
all: $(TARGET)
else
all: depend
	@make -f $(MAKEFILENAME) all
endif

# Build
$(TARGET): $(ODIR) $(OBJS)
	@printf '    LD $@\n'
	@$(CC) -o $@.db $(FOBJS) $(LDFLAGS)
	@$(STRIP) --remove-section=.comment $@.db -o $@
	@$(CHMOD) $@

# Compile
$(OBJS):
	@printf '    CC $<\n'
	@$(CC) -c $(CFLAGS) $(CWARN) $(CDEFS) $< -o $(ODIR)/$@

# Make dir
$(ODIR):
	@$(MKDIR) $(ODIR)

# Dump
.PHONY: dump
dump:
	@printf '    => Dumping... $(TARGET)\n'
	@$(DUMP) --disassemble-all --reloc $(TARGET).db >$(TARGET).s
	@printf '    <= Done\n'

# Clean
.PHONY: clean
clean:
	@printf '    => Cleaning all...\n'
	-@$(RM) $(ODIR)/*.o
	-@$(RM) $(TARGET)
	-@$(RM) $(TARGET).db
	-@$(RM) $(TARGET).s
	-@$(RM) crunch
	@printf '    <= Done\n'

# Install
.PHONY: install
install:
	@copy $(TARGET) $(DEST)
	@delete $(TARGET)
	@avail flush

# Help
.PHONY: help
help:
	@printf 'Usage: make [target]\n'
	@printf ' target:\n'
	@printf '   help     display this information\n'
	@printf '   all      make $(TARGET)\n'
	@printf '   depend   create the dependencies file\n'
	@printf '   clean    clean stuff\n'
	@printf '   dump     create a dump file\n'
	@printf '\n'

# Make dependencies
.PHONY: depend
depend:
	@printf '    => Generating dependencies (takes some seconds please wait)...\n'
	@sh mkdep.sh -cc "$(CC) -M" -f $(DEPFILE) $(DEPOPTS) $(DEPINCS) $(patsubst %.o,%.c, $(OBJS))
	@printf '    <= Done\n'

# Include dependencies file
-include $(DEPFILE)

