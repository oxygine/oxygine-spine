LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := oxygine-spine_static
LOCAL_MODULE_FILENAME := liboxygine-spine
LOCAL_CPP_EXTENSION := .cpp

SPINE_SRC := $(LOCAL_PATH)/dependencies/spine/

LOCAL_SRC_FILES := \
				$(subst $(LOCAL_PATH)/,, \
				$(wildcard $(SPINE_SRC)/src/spine/*.cpp) \
				)

LOCAL_SRC_FILES += src/oxygine-spine/spine.cpp


LOCAL_C_INCLUDES := $(SPINE_SRC)/include \
					$(LOCAL_PATH)/../oxygine-framework/oxygine/src/

LOCAL_EXPORT_C_INCLUDES += $(SPINE_SRC)/include \
						   $(LOCAL_PATH)/src 
					
include $(BUILD_STATIC_LIBRARY)
