### module.mk

LOCAL_PATH	        :=$(call MyDir)

LOCAL_MODULE		:=main
LOCAL_MODULE_OWNER	:=user_main

LOCAL_SRC_FILES		:=main.c

#LOCAL_CORSS_COMPILE	:=

#LOCAL_AR		:=$(LOCAL_CROSS_COMPILE)ar
#LOCAL_CC		:=$(LOCAL_CROSS_COMPILE)gcc
#LOCAL_CPP		:=$(LOCAL_CROSS_COMPILE)gcc
#LOCAL_CXX		:=$(LOCAL_CROSS_COMPILE)g++
#LOCAL_LD		:=$(LOCAL_CC)

LOCAL_INCLUDE_DIRS	:=$(TEST_TOP_DIR)/include
LOCAL_LIBRARY_DIRS	:=

LOCAL_EXPORT_HEADER_TO	 :=
LOCAL_EXPORT_HEADER_DIRS :=

LOCAL_STATIC_LIBRARIES	:=liball
LOCAL_SHARED_LIBRARIES	:=libfunc_d

LOCAL_LDLIBS		:=

LOCAL_ARFLAGS		:=
LOCAL_CFLAGS		:=
LOCAL_CPPFLAGS		:=
LOCAL_CXXFLAGS		:=
LOCAL_LDFLAGS		:=

include $(BUILD_HOST_EXECUTABLE)
