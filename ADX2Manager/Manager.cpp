//
//  Manager.cpp
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#include "Manager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include <AudioToolbox/AudioSession.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

namespace ADX2 {
    
    Manager* Manager::_instance = nullptr;
    
    Manager* Manager::initialize(CriAtomExPlayerConfig playerConfig, CriAtomExStandardVoicePoolConfig voicePoolConfig)
    {
        _instance = new Manager(playerConfig, voicePoolConfig);
        
        return _instance;
    }
    
    Manager* Manager::initialize()
    {
        assert(_instance == nullptr);
        CriAtomExStandardVoicePoolConfig voicePoolConfig;
        CriAtomExPlayerConfig playerConfig;
        return Manager::initialize(playerConfig, voicePoolConfig);
    }
    
    Manager::Manager(CriAtomExPlayerConfig playerConfig,
                             CriAtomExStandardVoicePoolConfig voicePoolConfig)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        auto interruptionListener = [](void *userData, UInt32 interruptionState) {
            switch (interruptionState) {
                case kAudioSessionBeginInterruption:
                    criAtomEx_StopSound_IOS();
                    break;
                case kAudioSessionEndInterruption:
                    AudioSessionSetActive(true);
                    criAtomEx_StartSound_IOS();
                    break;
            }
        };
        AudioSessionInitialize(NULL, NULL, interruptionListener, NULL);
        UInt32 category = kAudioSessionCategory_AmbientSound;
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        AudioSessionSetActive(true);
#endif
        auto errorCallback = [](const char *errid, uint32_t p1, uint32_t p2, uint32_t *parray) {
            const CriChar8 *errmsg;
            errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
            cocos2d::log("%s", errmsg);
        };
        criErr_SetCallback(errorCallback);
        
        auto userAlloc = [](void *obj, uint32_t size) {
            return malloc(size);
        };
        
        auto userFree = [] (void *obj, void *ptr) {
            free(ptr);
        };
        
        criAtomEx_SetUserAllocator(userAlloc, userFree, NULL);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        criAtomEx_Initialize_IOS(NULL, NULL, 0);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        criAtomEx_Initialize_ANDROID(NULL, NULL, 0);
        cocos2d::JniMethodInfo methodInfo;
        cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
                                                "org/cocos2dx/lib/Cocos2dxActivity",
                                                "getContext",
                                                "()Landroid/content/Context;");
        auto android_context_object = (jobject)methodInfo.env->CallStaticObjectMethod( methodInfo.classID, methodInfo.methodID );
        criFs_EnableAssetsAccess_ANDROID(cocos2d::JniHelper::getJavaVM(), android_context_object);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        criAtomEx_Initialize_PC(NULL, NULL, 0);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        criAtomEx_Initialize_MACOSX(NULL, NULL, 0);
#endif
        
        _dbasID = criAtomDbas_Create(NULL, NULL, 0);
        
        _voicePool = criAtomExVoicePool_AllocateStandardVoicePool(&voicePoolConfig, NULL, 0);
        
        criAtomExPlayer_SetDefaultConfig(&playerConfig);
        _player = criAtomExPlayer_Create(&playerConfig, NULL, 0);
    }
    
    Manager* Manager::getInstance()
    {
        assert(_instance);
        return _instance;
    }
    
    void Manager::update()
    {
    	criAtomEx_ExecuteMain();
    }
    
    void Manager::finalize()
    {
        delete _instance;
        _instance = nullptr;
    }
    
    Manager::~Manager()
    {
        criAtomExPlayer_Stop(_player);
        criAtomExVoicePool_Free(_voicePool);
        criAtomEx_UnregisterAcf();
        criAtomDbas_Destroy(_dbasID);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        criAtomEx_Finalize_IOS();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        criAtomEx_Finalize_ANDROID();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        criAtomEx_Finalize_PC();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        criAtomEx_Finalize_MACOSX;
#endif
    }

    void Manager::stopAll()
    {
        criAtomExPlayer_Stop(_player);
    }
    
    int Manager::getVoiceNum()
    {
        int32_t currentVnum;
        criAtomExVoicePool_GetNumUsedVoices(_voicePool, &currentVnum, NULL);
        return currentVnum;
    }
    

}
