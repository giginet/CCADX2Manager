//
//  Cue.h
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#ifndef __CcAdx2Basic__Cue__
#define __CcAdx2Basic__Cue__

#include <iostream>
#include "cri_adx2le.h"

namespace ADX2 {
    
    class Cue {
     private:
        /// 波形データバンクのハンドル
        CriAtomExAcbHn _acb;
     public:
        Cue(const char* acf, const char* acb);
        Cue(const char* acf, const char* acb, const char* awb);
        virtual ~Cue();
        
        int64_t playCueByID(uint32_t cueID);
        void stop(int64_t playbackID);
        const char* getCueName(CriAtomExCueId cueID);
        int64_t getTime(uint32_t cueID);
        CriAtomExPlaybackStatus getStatus(CriAtomExPlaybackId id);
    };
    
}


#endif /* defined(__CcAdx2Basic__Cue__) */
