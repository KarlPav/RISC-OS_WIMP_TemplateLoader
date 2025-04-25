# Setup RISCOS GCC environment
TARGET_ENV = . ~/gccsdk/env/ro-path

# Compiler and linker
CC := gcc
LD := gcc

# Setup some standard compiler flags.
CCFLAGS += -std=gnu99 -mlibscl -mhard-float -mthrowback -Wall -O2 -fno-strict-aliasing -mpoke-function-name #-static
#CXXFLAGS += -Wall -O2 #-mfloat-abi=hard #-static
LDFLAGS +=

# Set target
TARGET := !RunImage

# Relative paths to our source, header and object file directories.
SRCDIR := $(CURDIR)/c
HDRDIR := $(CURDIR)/h
OBJDIR := $(CURDIR)/o

# The C++ Headers and libraries live here.
INCLUDES := -I$(GCCSDK_INSTALL_ENV)/include -I$(HDRDIR)
LINKS := -L$(GCCSDK_INSTALL_ENV)/lib -lOSLibH32 -lSFLib32

# Ensure the directory exists
$(shell mkdir -p $(SRCDIR))
$(shell mkdir -p $(HDRDIR))
$(shell mkdir -p $(OBJDIR))

# List of source files and corresponding object files and the target
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Debugging: Print the source and object files
$(info Source files: $(SRCS))
$(info Object files: $(OBJS))

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Compile the C++ sources into ELF object files.
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(TARGET_ENV) && $(CC) -c $(CCFLAGS) $(INCLUDES) $< -o $@

# Link the ELF object files into an RISCOS ELF executable.
$(TARGET): $(OBJS)
	@$(TARGET_ENV) && $(CC) $(CCFLAGS) $(LINKS) $(LDFLAGS) -o $(TARGET) $(OBJS)

# Clean the build directory
clean:
	@$(TARGET_ENV) && rm -f $(OBJDIR)/*.o $(TARGET)
