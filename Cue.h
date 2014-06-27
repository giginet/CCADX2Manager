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
        
        /**
         *  ファイルからCueSheetを初期化します
         *  @param acfの絶対パス
         *  @param acbの絶対パス
         */
        Cue(const char* acf, const char* acb);

        
        /**
         *  ファイルからCueSheetを初期化します
         *  @param acfの絶対パス
         *  @param acbの絶対パス
         *  @param awbの絶対パス
         */
        Cue(const char* acf, const char* acb, const char* awb);
        virtual ~Cue();
        
        /**
         *  CueSheet内の指定IDの音声データを鳴らします
         *  @param cueID 再生する音声データのcueID
         *  @return 再生されたサウンドのPlaybackID
         */
        CriAtomExPlaybackId playCueByID(CriAtomExCueId cueID);
        
        /**
         *  指定したPlaybackIDのサウンドを停止します
         *  @param playbackID 停止するサウンドのPlaybackID
         */
        void stop(CriAtomExPlaybackId playbackID);
        
        /**
         *  CueSheet内の指定IDの音声データの名称を取り出します
         *  @param cueID 音声データのcueID
         *  @return 音声データ名
         */
        const char* getCueName(CriAtomExCueId cueID);
        
        /**
         *  指定されたサウンドの再生位置を取り出します
         *  指定IDが再生されていないときは-1が返ります
         *  @param playbackID 再生位置を取得するサウンドのplaybackID
         *  @return 再生位置
         *
         */
        int64_t getTime(CriAtomExPlaybackId playbackID);
        
        /**
         * 指定されたサウンドの状態を取り出します
         * @param playbackID 状態を取得するサウンドのplaybackID
         * @return サウンドの状態
         */
        CriAtomExPlaybackStatus getStatus(CriAtomExPlaybackId id);
    };
    
}


#endif /* defined(__CcAdx2Basic__Cue__) */
