LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SRC_FILES := main.cpp \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/src/core/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/core/filesystem/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/core/math/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/enemies/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/enemies/bosses*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/gui/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/input/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/level/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/objects/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/owerworld/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/user/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/video/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/audio/*.cpp))

LOCAL_STATIC_LIBRARIES := SDL2_static

include $(BUILD_SHARED_LIBRARY)
