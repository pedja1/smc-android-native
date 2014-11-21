include $(call all-subdir-makefiles)

#boost
LOCAL_CFLAGS += -I$(LOCAL_PATH)/boost/include/boost-1_55
LOCAL_LDLIBS += -L$(LOCAL_PATH)/boost/lib/ -lboost_system -lboost_thread -lboost_filesystem

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

#libintl

LOCAL_CFLAGS += -I$(LOCAL_PATH)/libintl-lite/include
LOCAL_LDLIBS += -L$(LOCAL_PATH)/libintl-lite/lib/

