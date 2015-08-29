#include "CueSheet.h"
#include "Manager.h"

namespace ADX2 {
    
    CueSheet::CueSheet()
    : _acb(nullptr)
    {
    }
    
    CueSheet* CueSheet::create(const char *acf, const char *acb)
    {
        return CueSheet::create(acf, acb, nullptr);
    }
    
    CueSheet* CueSheet::create(const char *acf, const char *acb, const char *awb)
    {
        auto cue = new CueSheet();
        if (cue && cue->initWithFile(acf, acb, awb)) {
            cue->autorelease();
            return cue;
        }
        CC_SAFE_DELETE(cue);
        return nullptr;
    }
    
    std::string CueSheet::getFullPath(const char *filename)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        return std::string(filename);
#endif
        return cocos2d::FileUtils::getInstance()->fullPathForFilename(filename);
    }
    
    bool CueSheet::initWithFile(const char* acf, const char* acb, const char* awb)
    {
        
        auto acfPath = getFullPath(acf);
        auto acbPath = getFullPath(acb);
        
        criAtomEx_RegisterAcfFile(NULL, acfPath.c_str(), NULL, 0);
        if (awb) {
            auto awbPath = getFullPath(awb);
            _acb = criAtomExAcb_LoadAcbFile(NULL, acbPath.c_str(), NULL, awbPath.c_str(), NULL, 0);
        } else {
            _acb = criAtomExAcb_LoadAcbFile(NULL, acbPath.c_str(), NULL, NULL, NULL, 0);
        }
        
        if (_acb == nullptr) {
            return false;
        }
        
        return true;
    }
    
    CueSheet::~CueSheet()
    {
        criAtomExAcb_Release(_acb);
    }
    
    CriAtomExPlaybackId CueSheet::playCueByID(CriAtomExCueId cueID)
    {
        auto player = Manager::getInstance()->getDefaultPlayer();
        criAtomExPlayer_SetCueId(player, _acb, cueID);
        
        CriAtomExPlaybackId playbackID = criAtomExPlayer_Start(player);
        
        return playbackID;
    }
    
    CriAtomExPlaybackId CueSheet::playCueByID(CriAtomExCueId cueID, const char *keyword)
    {
        CriAtomExPlaybackId playbackID = this->playCueByID(cueID);
        _playbackIDs.insert(std::make_pair(keyword, playbackID));
        return playbackID;
    }
    
    void CueSheet::stop(CriAtomExPlaybackId playbackID)
    {
        criAtomExPlayback_Stop(playbackID);
    }
    
    void CueSheet::stop(const char *keyword)
    {
        CriAtomExPlaybackId playbackId = _playbackIDs.at(keyword);
        this->stop(playbackId);
    }
    
    const char* CueSheet::getCueName(CriAtomExCueId cueID)
    {
        return criAtomExAcb_GetCueNameById(_acb, cueID);
    }
    
    int64_t CueSheet::getTime(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetTime(playbackID);
    }
    
    int64_t CueSheet::getTime(const char *keyword)
    {
        CriAtomExPlaybackId playbackId = _playbackIDs.at(keyword);
        return this->getTime(playbackId);
    }
    
    CriAtomExPlaybackStatus CueSheet::getStatus(CriAtomExPlaybackId playbackID)
    {
        return criAtomExPlayback_GetStatus(playbackID);
    }
    
    CriAtomExPlaybackStatus CueSheet::getStatus(const char *keyword)
    {
        CriAtomExPlaybackId playbackId = _playbackIDs.at(keyword);
        return this->getStatus(playbackId);
    }
    
}