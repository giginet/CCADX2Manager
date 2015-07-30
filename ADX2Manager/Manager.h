//
//  Manager.h
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#ifndef __CcAdx2Basic__Manager__
#define __CcAdx2Basic__Manager__

#include <iostream>
#include "cocos2d.h"
#include "cri_adx2le.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "cri_le_atom_ios.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "cri_le_atom_android.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_PC)
#include "cri_le_atom_pc.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MACOSX)
#include "cri_le_atom_macosx.h"
#endif

namespace ADX2 {
    
    class Manager {
    private:
        static Manager* _instance;
        /// ボイスプールのハンドル
        CriAtomExVoicePoolHn _voicePool;
        /// 音声プレーヤのハンドル
        CriAtomExPlayerHn _player;
        /// D-BASのID
        CriAtomDbasId _dbasID;
        
        Manager(CriAtomExPlayerConfig playerConfig,
                CriAtomExStandardVoicePoolConfig voicePoolConfig);
    public:
        virtual ~Manager();
        
        static Manager* initialize();
        static Manager* initialize(CriAtomExPlayerConfig playerConfig, CriAtomExStandardVoicePoolConfig voicePoolConfig);
        static Manager* getInstance();
        CriAtomExPlayerHn getDefaultPlayer() {
            return _player;
        }
        void update();
        void stopAll();
        int getVoiceNum();
        static void finalize();
    };
    
}

#endif /* defined(__CcAdx2Basic__Manager__) */
