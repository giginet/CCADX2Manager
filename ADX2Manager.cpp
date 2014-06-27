//
//  ADX2Manager.cpp
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#include "ADX2Manager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include <AudioToolbox/AudioSession.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

namespace ADX2 {
    
    ADX2Manager* ADX2Manager::_instance = nullptr;
    
    ADX2Manager* ADX2Manager::initialize(CriAtomExPlayerConfig playerConfig, CriAtomExStandardVoicePoolConfig voicePoolConfig)
    {
        _instance = new ADX2Manager(playerConfig, voicePoolConfig);
        
        return _instance;
    }
    
    ADX2Manager::ADX2Manager()
    {
        /* ボイスプールの設定。まずはデフォルト設定にして、その上で必要な値へ書き換えていく */
        CriAtomExStandardVoicePoolConfig vp_config;

        vp_config.num_voices = 8;
        vp_config.player_config.streaming_flag		= CRI_TRUE;
        vp_config.player_config.max_sampling_rate	= 48000 * 2;
        
        /* Player作成にも設定は必要 */
        CriAtomExPlayerConfig pf_config;
        criAtomExPlayer_SetDefaultConfig(&pf_config);
        pf_config.max_path_strings	= 1;
        pf_config.max_path			= 256;

        ADX2Manager::ADX2Manager(pf_config, vp_config);
    }
    
    ADX2Manager::ADX2Manager(CriAtomExPlayerConfig playerConfig,
                             CriAtomExStandardVoicePoolConfig voicePoolConfig)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        auto interruptionListener = [](void *userData, UInt32 interruptionState) {
            
        };
        AudioSessionInitialize(NULL, NULL, interruptionListener, NULL);
        UInt32 category = kAudioSessionCategory_AmbientSound;
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        AudioSessionSetActive(true);
#endif
        auto errorCallback = [](const char *errid, uint32_t p1, uint32_t p2, uint32_t *parray) {
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
        
        /* ANDROIDの場合で必要な一手間。assetsフォルダへのアクセスを可能にする */
        /* まずはJniHelperでActivityのContextを取得 */
        cocos2d::JniMethodInfo methodInfo;
        cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
                                                "org/cocos2dx/lib/Cocos2dxActivity",
                                                "getContext",
                                                "()Landroid/content/Context;");
        auto android_context_object = (jobject)methodInfo.env->CallStaticObjectMethod( methodInfo.classID, methodInfo.methodID );
        /* 有効化。assetsフォルダはCocosプロジェクトのResource相当なので、ほぼ必須と言って良い手順 */
        criFs_EnableAssetsAccess_ANDROID(cocos2d::JniHelper::getJavaVM(), android_context_object);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        criAtomEx_Initialize_PC(NULL, NULL, 0);
#endif
        
        _dbasID = criAtomDbas_Create(NULL, NULL, 0);
        
        /* ボイスプールの設定。まずはデフォルト設定にして、その上で必要な値へ書き換えていく */
        criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&voicePoolConfig);
        
        /* 上で作った設定オブジェクトを渡して、ボイスプールを作成 */
        _voicePool = criAtomExVoicePool_AllocateStandardVoicePool(&voicePoolConfig, NULL, 0);
        
        /* Player作成にも設定は必要 */
        criAtomExPlayer_SetDefaultConfig(&playerConfig);
        _player = criAtomExPlayer_Create(&playerConfig, NULL, 0);
    }
    
    ADX2Manager* ADX2Manager::getInstance()
    {
        if (_instance == nullptr) {
            assert(_instance == nullptr);
        }
        return _instance;
    }
    
    void ADX2Manager::update()
    {
    	criAtomEx_ExecuteMain();
    }
    
    void ADX2Manager::finalize()
    {
        criAtomExPlayer_Stop(_player);
        criAtomExVoicePool_Free(_voicePool);
        //criAtomExAcb_Release(g_acb);
        criAtomEx_UnregisterAcf();
        criAtomDbas_Destroy(_dbasID);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        criAtomEx_Finalize_IOS();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        criAtomEx_Finalize_ANDROID();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        criAtomEx_Finalize_PC();
#endif
    }
    
    ADX2Manager::~ADX2Manager()
    {
    }

    void ADX2Manager::stopAll()
    {
        criAtomExPlayer_Stop(_player);
    }
    
    int ADX2Manager::getVoiceNum()
    {
        int32_t currentVnum;
        criAtomExVoicePool_GetNumUsedVoices(_voicePool, &currentVnum, NULL);
        return currentVnum;
    }
    

}
