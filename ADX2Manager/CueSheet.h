#ifndef __CCADX2Manager__CueSheet__
#define __CCADX2Manager__CueSheet__

#include <iostream>
#include <map>
#include "cri_adx2le.h"
#include "cocos2d.h"

namespace ADX2 {
    
    class CueSheet :public cocos2d::Ref {
    private:
        /// 波形データバンクのハンドル
        CriAtomExAcbHn _acb;
        
        std::unordered_map<std::string, CriAtomExPlaybackId> _playbackIDs;
        
    CC_CONSTRUCTOR_ACCESS:
        
        CueSheet();
        
        /**
         *  ファイルからCueSheetを初期化します
         *  @param acfのファイル名
         *  @param acbのファイル名
         *  @param awbのファイル名
         */
        bool initWithFile(const char* acf, const char* acb, const char* awb);
        
        virtual ~CueSheet();
        
        std::string getFullPath(const char* filename);
        
    public:
        
        /**
         *  CueSheet内の指定IDの音声データを鳴らします
         *  @param cueID 再生する音声データのcueID
         *  @param keyword 再生にキーワードを与え、あとで参照できるようにします
         *  @return 再生されたサウンドのPlaybackID
         */
        CriAtomExPlaybackId playCueByID(CriAtomExCueId cueID);
        CriAtomExPlaybackId playCueByID(CriAtomExCueId cueID, const char* keyword);
        
        static CueSheet* create(const char* acf, const char* acb, const char* awb);
        static CueSheet* create(const char* acf, const char* acb);
        
        /**
         *  指定したPlaybackIDのサウンドを停止します
         *  @param playbackID, keyword 停止するサウンドのPlaybackID、またはキーワード
         */
        void stop(CriAtomExPlaybackId playbackID);
        void stop(const char *keyword);
        
        /**
         *  CueSheet内の指定IDの音声データの名称を取り出します
         *  @param cueID 音声データのcueID
         *  @return 音声データ名
         */
        const char* getCueName(CriAtomExCueId cueID);
        
        /**
         *  指定されたサウンドの再生位置を取り出します
         *  指定IDが再生されていないときは-1が返ります
         *  @param playbackID, keyword 再生位置を取得するサウンドのplaybackID、またはキーワード
         *  @return 再生位置
         *
         */
        int64_t getTime(CriAtomExPlaybackId playbackID);
        int64_t getTime(const char* keyword);
        
        /**
         * 指定されたサウンドの状態を取り出します
         * @param playbackID, keyword 状態を取得するサウンドのplaybackID、またはキーワード
         * @return サウンドの状態
         */
        CriAtomExPlaybackStatus getStatus(CriAtomExPlaybackId playbackID);
        CriAtomExPlaybackStatus getStatus(const char* keyword);
        
        /**
         * 指定されたキーワードからPlaybackIDを取り出します。見つからない場合は-1を返します
         * @param keyword Playback IDを取得するサウンドのキーワード
         * @return Playback ID、もしくは-1
         */
        CriAtomExPlaybackId getPlaybackID(const char* keyword);
    };
    
}


#endif /* defined(__CCADX2Manager__CueSheet__) */
