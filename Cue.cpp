//
//  Cue.cpp
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#include "Cue.h"
#include "ADX2Manager.h"

namespace ADX2 {
    
    Cue::Cue(const char* acf, const char* acb) : Cue::Cue(acf, acb, nullptr) { }
    
    Cue::Cue(const char* acf, const char* acb, const char* awb)
    {
        criAtomEx_RegisterAcfFile(NULL, acf, NULL, 0);
        
        _acb = criAtomExAcb_LoadAcbFile(NULL, acb, NULL, awb, NULL, 0);
    }
    
    Cue::~Cue()
    {
        criAtomExAcb_Release(_acb);
    }
    
    CriAtomExPlaybackId Cue::playCueByID(CriAtomExCueId cueID)
    {
        auto player = ADX2Manager::getInstance()->_player;
        criAtomExPlayer_SetCueId(player, _acb, cueID);
        
        int64_t playbackID = criAtomExPlayer_Start(player);
        
        return playbackID;
    }
    
    void Cue::stop(CriAtomExPlaybackId playbackID)
    {
        criAtomExPlayback_Stop(playbackID);
    }
    
    const char* Cue::getCueName(CriAtomExCueId cueID)
    {
        return criAtomExAcb_GetCueNameById(_acb, cueID);
    }
    
    int64_t Cue::getTime(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetTime(playbackID);
    }
    
    CriAtomExPlaybackStatus Cue::getStatus(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetStatus(playbackID);
    }
    
}