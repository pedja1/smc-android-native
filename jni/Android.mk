LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := SDL2_static
LOCAL_SRC_FILES := SDL2/lib/armeabi-v7a/libSDL2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := SDL2_image
LOCAL_SRC_FILES := SDL2_image/lib/armeabi-v7a/libSDL2_mage.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := SDL2_mixer
LOCAL_SRC_FILES := SDL2_mixer/lib/armeabi-v7a/libSDL2_mixer.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := SDL2_ttf
LOCAL_SRC_FILES := SDL2_ttf/lib/armeabi-v7a/libSDL2_ttf.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libiconv
LOCAL_SRC_FILES := cegui/lib/armeabi-v7a/libiconv.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_atomic
LOCAL_SRC_FILES := boost/lib/armeabi-v7a/libboost_atomic-gcc-mt-1_55.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_filesystem
LOCAL_SRC_FILES := boost/lib/armeabi-v7a/libboost_filesystem-gcc-mt-1_55.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_system
LOCAL_SRC_FILES := boost/lib/armeabi-v7a/libboost_system-gcc-mt-1_55.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := boost_thread
LOCAL_SRC_FILES := boost/lib/armeabi-v7a/libboost_thread-gcc-mt-1_55.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libintl
LOCAL_SRC_FILES := libintl/lib/armeabi-v7a/libintl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libpng
LOCAL_SRC_FILES := libpng/lib/armeabi-v7a/libpng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/cegui/include \
		$(LOCAL_PATH)/SDL2/include \
		$(LOCAL_PATH)/SDL2_image/include \
		$(LOCAL_PATH)/SDL2_mixer/include \
		$(LOCAL_PATH)/SDL2_ttf/include \
		$(LOCAL_PATH)/boost/include \
		$(LOCAL_PATH)/libintl/include \
		$(LOCAL_PATH)/libpng/include

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

LOCAL_CFLAGS    := -DDEBUG 
LOCAL_CFLAGS    += -fexceptions -frtti -D___ANDROID___ -DANDROID \
  -DRENDERER_GLES2 \
  -DGLES2_BACKEND_NATIVE_GLES2 


LOCAL_LDFLAGS	:= -Wl
LOCAL_LDLIBS    := -L$(CEGUI_ROOT)/dependencies/lib/$(TARGET_ARCH_ABI) 
LOCAL_LDLIBS    += -L$(CEGUI_ROOT)/android/systemlibs/$(TARGET_ARCH_ABI) 
LOCAL_LDLIBS    += -L$(CEGUI_ROOT)/android/lib 
LOCAL_LDLIBS    += -L$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.6/libs/$(TARGET_ARCH_ABI)
LOCAL_LDLIBS    += -llog -landroid 
LOCAL_LDLIBS    += -lCEGUIOpenGLESRenderer-9999_Static  -lCEGUIBase-9999_Static -lCEGUICommonDialogs-9999_Static -lCEGUICoreWindowRendererSet_Static -lCEGUITinyXMLParser_Static -lCEGUIFreeImageImageCodec_Static 
LOCAL_LDLIBS    += -ltinyxml -lfreetype -lFreeImage -lgnustl_static
LOCAL_LDLIBS    := -lGLESv2 -lz -ldl -lEGL -lGLESv1_CM

LOCAL_STATIC_LIBRARIES := SDL2_static \
	SDL2_image \
	SDL2_mixer \
	SDL2_ttf \
	boost_thread \
	boost_system \
	boost_atomic \
	boost_filesystem \
	libpng \
	libintl
LOCAL_STATIC_LIBRARIES := android_native_app_glue 
LOCAL_SHARED_LIBRARIES := libiconv

#boost
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

include $(BUILD_SHARED_LIBRARY)

$(call import-module, android/native_app_glue)
