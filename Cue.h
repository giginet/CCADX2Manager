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
     public:
        Cue(const char* acf, const char* acb);
        Cue(const char* acf, const char* awb);
        virtual ~Cue();
        
        int64_t playCueByID(uint32_t cueID);
        void stop(int64_t playbackID);
        void stopAll();
        void pauseAll(bool sw);
        const char* getCueName();
        int getVoiceNum();
        int64_t getTime(uint32_t cueID);
        
    }
    
}


#endif /* defined(__CcAdx2Basic__Cue__) */
