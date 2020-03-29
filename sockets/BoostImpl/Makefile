ifeq ($(origin BOOST_SOCKETS_INC),undefined)
    BOOST_SOCKETS_INC := 1
    
   ifeq ($(origin BASE_DIR),undefined)
      BASE_DIR := $(shell pwd | sed "s,\(.*/sockets\).*,\1,gI")
   endif

   # Configuration includes go here
   include ${BASE_DIR}/config/common.mk

   # Include Makefiles for any libraries we require
   
   
   # Specify variables used to build this Makefile's rules
   THISDIR := $(BOOST_SOCKETS_IMPL_DIR)
   OBJDIR  := $(THISDIR)/obj
   LIBRARY := $(BOOST_SOCKETS_IMPL_LIB)
   
   # Specify include directories here
   $(OBJDIR)_INCLUDE_DIRS  := $(SOCKETS_DIR)
   
   # Specify libraries to link in here
   $(LIBRARY)_LIBS         := $(BOOST_LIBS)
   
   # Make one .o for every .cpp in the source directory
   $(OBJDIR)_SRCDIR        := $(THISDIR)
   $(LIBRARY)_OBJS         := $(patsubst $(THISDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(THISDIR)/*.cpp))
   
   ifdef WINDOWS
      $($(LIBRARY)_OBJS): CXXFLAGS   := $(CXXFLAGS) -D_WIN32_WINNT=0x0601
      $(LIBRARY)_EXTERNAL_LIB_FLAGS := -lws2_32 -lwsock32
   endif

   include $(CONFIG_DIR)/make_shared_objects.mk
   
 endif
