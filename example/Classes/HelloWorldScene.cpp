/****************************************************************************
 *
 * CRI ADX2 SDK
 *
 * Copyright (c) 2009-2013 CRI Middleware Co., Ltd.
 *
 * Title   : CRI ADX2 sample program
 * Module  : Simple Playback using Cocos2Dx
 *         : Cocos2Dxでの単純再生サンプル
 * File    : HelloWorldScene.cpp
 *
 ****************************************************************************/

/*
 このサンプルは、Cocos2Dxによって、サウンドをキューID指定再生します。
 画面上のUIボタンは以下のような機能を持っています。
 　(1) 左側の再生ボタン x8	:	それぞれ対応するIDのキューを再生
　(2) 右上の停止ボタン x2	:	PlaybackId単位の停止，Player単位の停止
　(3) 右下のラベル x2	:	再生状態表示、再生時間表示。 情報は毎フレーム更新
 */

#include "HelloWorldScene.h"

USING_NS_CC;

/* ADX2 関連 */
namespace {
	/* ボイスプールへの設定 */
	const int	max_voice			= 8;		/* デフォルト設定でも8	*/
	const int	max_sampling_rate	= 48000*2;	/* ピッチ変更も加味		*/

	const float	pitch_change_value	= 200.0f;

	CriAtomExAcbHn			g_acb;			/* 波形データバンクのハンドル	*/
	CriAtomExVoicePoolHn	g_voice_pool;	/* ボイスプールのハンドル	 	*/
	CriAtomExPlayerHn		g_player;		/* 音声プレーヤのハンドル		*/
	CriAtomDbasId			g_dbas_id;		/* D-BASのID				*/

	/* 多重初期化を防ぐためのカウンタ  */
	CriUint32				g_initialize_count = 0;

	//---------------------------------------------
	/*---- ADX2関連 ----*/
	//---------------------------------------------
	/*---- 変数定義 ----*/
	/* ADX2 Atomライブラリ初期化カウント(多重初期化の回避) */
	/* キュー一覧 */
	CriAtomExCueId g_cue_list[] = {
		CRI_BASIC_MUSIC1,
		CRI_BASIC_MUSIC2,
		CRI_BASIC_VOICE,
		CRI_BASIC_KALIMBA_RANDOM,
		CRI_BASIC_KALIMBA,
		CRI_BASIC_GUNSHOT,
		CRI_BASIC_BOMB_LIMIT,
		CRI_BASIC_BOMB,
		CRI_BASIC_VOICE_EFFECT
	};
	const CriUint32 g_num_cue_items = sizeof(g_cue_list)/sizeof(CriAtomExCueId);

	/* プレイヤーの状態 */
	const std::string g_playback_status_description[] = {
		"--",
		"Prepare",
		"Playing",
		"Removed",
	};

	/* 関数宣言 */
	void adx2_initialize(void);
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	void adx2_interruption_listener(void *userData, UInt32 interruptionState);
	#endif
	void adx2_finalize(void);
	void adx2_update(void);
	CriSint64 adx2_play_cue_by_id(CriUint32 cue_id);
	void adx2_stop(CriSint64 playback_id);
	void adx2_stop_all(void);
	void adx2_pause_all(CriBool sw);
	void adx2_get_resource_path(CriChar8 *full_path, const CriChar8 *loc_path);
	void *adx2_alloc(void *obj, CriUint32 size);
	void adx2_free(void *obj, void *ptr);
	void adx2_error_callback(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray);
	const char* adx2_get_cue_name(CriAtomExCueId cue_id);
	std::string adx2_get_voice_num(void);
	std::string adx2_get_time(CriAtomExCueId cue_id);

	void adx2_initialize(void)
	{
		/* 初期化カウントアップ */
		g_initialize_count++;
		if (g_initialize_count > 1) {
			/* 多重初期化は許さず */
			return;
		}

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		AudioSessionInitialize(NULL, NULL, adx2_interruption_listener, NULL);
		UInt32 category = kAudioSessionCategory_AmbientSound;
		AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
		AudioSessionSetActive(true);
	#endif
		criErr_SetCallback(&adx2_error_callback);

		criAtomEx_SetUserAllocator(&adx2_alloc, &adx2_free, NULL);

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		criAtomEx_Initialize_PC(NULL, NULL, 0);
	#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
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
	#endif

		g_dbas_id = criAtomDbas_Create(NULL, NULL, 0);

		CriAtomExStandardVoicePoolConfig vp_config;

		/* ボイスプールの設定。まずはデフォルト設定にして、その上で必要な値へ書き換えていく */
		criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vp_config);
		vp_config.num_voices = max_voice;
		vp_config.player_config.streaming_flag		= CRI_TRUE;
		vp_config.player_config.max_sampling_rate	= max_sampling_rate;

		/* 上で作った設定オブジェクトを渡して、ボイスプールを作成 */
		g_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vp_config, NULL, 0);

		CriChar8 acf_path[256], acb_path[256], awb_path[256];
		adx2_get_resource_path(acf_path, "ADX2_samples.acf");
		adx2_get_resource_path(acb_path, "Basic.acb");
		adx2_get_resource_path(awb_path, "Basic.awb");
		criAtomEx_RegisterAcfFile(NULL, acf_path, NULL, 0);

		g_acb = criAtomExAcb_LoadAcbFile(NULL, acb_path, NULL, awb_path, NULL, 0);

		/* Player作成にも設定は必要 */
		CriAtomExPlayerConfig pf_config;
		criAtomExPlayer_SetDefaultConfig(&pf_config);
		pf_config.max_path_strings	= 1;
		pf_config.max_path			= 256;
		g_player = criAtomExPlayer_Create(&pf_config, NULL, 0);
	}

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	void adx2_interruption_listener(void *userData, UInt32 interruptionState)
	{
		switch (interruptionState) {
			case kAudioSessionBeginInterruption:
				criAtomEx_StopSound_IOS();
				break;
			case kAudioSessionEndInterruption:
				AudioSessionSetActive(true);
				criAtomEx_StartSound_IOS();
				break;
		}
	}
	#endif

	void adx2_finalize(void)
	{
		/* 早すぎる終了処理をしないように。Atomを使ってるHelloWorldがまだあるかもしれない */
		g_initialize_count--;
		if (g_initialize_count > 0) {
			return;
		}

		criAtomExPlayer_Stop(g_player);
		criAtomExVoicePool_Free(g_voice_pool);
		criAtomExAcb_Release(g_acb);
		criAtomEx_UnregisterAcf();
		criAtomDbas_Destroy(g_dbas_id);

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		criAtomEx_Finalize_IOS();
	#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		criAtomEx_Finalize_ANDROID();
	#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		criAtomEx_Finalize_PC();
	#endif
	}

	/* HelloWorldレイヤのupdateで呼ぶ */
	void adx2_update(void)
	{
		criAtomEx_ExecuteMain();
	}

	/* ID単位で音を鳴らす処理。鳴らした音のプレイバックIDが返る */
	CriSint64  adx2_play_cue_by_id(CriAtomExCueId cue_id)
	{
		criAtomExPlayer_SetCueId(g_player, g_acb, cue_id);

		if (cue_id == CRI_BASIC_KALIMBA) {
			/* Kalimbaキューのみピッチを変える */
			criAtomExPlayer_SetPitch(g_player, pitch_change_value);
		}

		CriSint64 playback_id =  criAtomExPlayer_Start(g_player);

		if (cue_id == CRI_BASIC_KALIMBA) {
			/* ピッチを元に戻す。この時点ではkalimbaの音には影響しない */
			criAtomExPlayer_SetPitch(g_player, 0.0f);
		}

		return playback_id;
	}
	
	/* プレイバックID単位で音を止める */
	void adx2_stop(CriSint64 playback_id)
	{
		criAtomExPlayback_Stop(playback_id);
	}
	
	/* プレイヤーごと音を止める */
	void adx2_stop_all(void)
	{
		criAtomExPlayer_Stop(g_player);
	}
	
	void adx2_get_resource_path(CriChar8 *full_path, const CriChar8 *loc_path)
	{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		sprintf(full_path, "%s", CCFileUtils::sharedFileUtils()->fullPathForFilename(loc_path).c_str());
	#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		/* assetsへのアクセスパスは、必ずassets以下からの相対パスでファイルを指定する
		 * パスの先頭に'/'文字が入ってはダメ */
		sprintf(full_path, "%s", loc_path);
	#endif
	}
	
	void *adx2_alloc(void *obj, CriUint32 size)
	{
		return malloc(size);
	}
	
	void adx2_free(void *obj, void *ptr)
	{
		free(ptr);
	}
	
	void adx2_error_callback(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
	{
		const CriChar8 *errmsg;
		errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
		CCLOG("%s\n", errmsg);
	}

	const char* adx2_get_cue_name(CriAtomExCueId cue_id)
	{
		return criAtomExAcb_GetCueNameById(g_acb, cue_id);
	}

	/* 現在使用中のボイス数を(文字列表現で)取得する */
	std::string adx2_get_voice_num(void)
	{
		CriSint32 current_vnum;
		criAtomExVoicePool_GetNumUsedVoices(g_voice_pool, &current_vnum, NULL);

		/* 整数 --> 文字列 に変換 */
		std::stringstream time_ss;
		time_ss << "[VoiceNum: " << current_vnum << "/" << max_voice << "]";
		return time_ss.str();
	}

	/* 現在プレイバック中の音声の再生時間を(文字列表現で)取得する */
	std::string adx2_get_time(CriAtomExCueId cue_id)
	{
		/* 整数 --> 文字列 に変換 */
		std::stringstream time_ss;
		time_ss << "[Time(msec): " << criAtomExPlayback_GetTime(cue_id) << "]";
		return time_ss.str();
	}
}

//---------------------------------------------
//---- cocos2d-x ----
//---------------------------------------------

/* 画面に配置するUIボタンに割り当てるコールバック。音を止める */
void HelloWorld::menuStop(Ref* pSender)
{
	adx2_stop(currentPlaybackId);
}

/* 画面に配置するUIボタンに割り当てるコールバック。音をプレイヤーごと止める */
void HelloWorld::menuAllStop(Ref* pSender)
{
	adx2_stop_all();
}

bool HelloWorld::init()
{
	CCLOG("%s", "HelloWorld::init");
	if ( !Layer::init() )
    {
        return false;
    }
	cocos2d::Size size = cocos2d::Director::sharedDirector()->getWinSize();

	/* GUIの設定と生成。そして配置。ボタンとラベル */
	{
		/* MenuItem (ボタン) をためこむベクタ。あとで一気に一個のMenuとしてadd */
		cocos2d::Vector<cocos2d::MenuItem*> buttonItems;

		/* ボタン生成 & ベクタへのプッシュバックの処理 */
		auto pushButton = [this, &size, &buttonItems](float x, float y,
				const char* ui_normal_image, const char* ui_push_image, cocos2d::ccMenuCallback c){
			auto *item = cocos2d::MenuItemImage::create(ui_normal_image, ui_push_image, std::move(c));
			item->setPosition(cocos2d::Point(size.width  * x, size.height * y));
			buttonItems.pushBack(item);
		};

		/* ラベル生成 & UIとして追加の処理 */
		auto addLabel = [this, &size](cocos2d::LabelTTF *pLabel, float x, float y){
			pLabel->setPosition(cocos2d::Point(size.width  * x, size.height * y));
			addChild(pLabel);
		};

		/* 鳴らしたいキューの数だけボタンを作る */
		for (CriAtomExCueId id = 0; id < g_num_cue_items; id++) {
			pushButton(0.10f, 0.95f - static_cast<float>(id)*0.10f, "PlayNormal.png", "PlayPush.png",
				[this, id](Ref*)->void{currentPlaybackId = adx2_play_cue_by_id(id);});

			/* キュー名のラベルを併記 */
			addLabel(cocos2d::LabelTTF::create(adx2_get_cue_name(id), "fonts/Marker Felt.ttf", 30),
					0.30f, 0.95f - static_cast<float>(id)*0.10f);
		}

		/* 音声停止ボタン (プレイバックID単位) */
		{
			pushButton(0.60f, 0.95f, "StopNormal.png", "StopPush.png",
					CC_CALLBACK_1(HelloWorld::menuStop, this));
			addLabel(cocos2d::LabelTTF::create("Stop", "fonts/Marker Felt.ttf", 30),
					0.75f, 0.95f);
		}

		/* 音声停止ボタン (プレイヤー単位) */
		{
			pushButton(0.60f, 0.85f, "StopNormal.png", "StopPush.png",
					CC_CALLBACK_1(HelloWorld::menuAllStop, this));
			addLabel(cocos2d::LabelTTF::create("StopAll", "fonts/Marker Felt.ttf", 30),
					0.75f, 0.85f);
		}

		/* ここまでに作った全てのMenuItem(ボタン)をGUIとして追加 */
		{
			cocos2d::Menu* menu = cocos2d::Menu::createWithArray(buttonItems);
			menu->setPosition(cocos2d::Point::ZERO);
			this->addChild(menu);
		}

		/* 現在のボイス数を示すラベルを追加 */
		{
			vnumLabel = cocos2d::LabelTTF::create(
					adx2_get_voice_num(),
					"fonts/Marker Felt.ttf", 30);
			addLabel(vnumLabel, 0.75f, 0.15f);
		}

		/* 現在のプレイヤー状態を示すラベルを追加 */
		{
			stateLabel = cocos2d::LabelTTF::create(
					g_playback_status_description[ criAtomExPlayback_GetStatus(currentPlaybackId) ],
					"fonts/Marker Felt.ttf", 30);
			addLabel(stateLabel, 0.75f, 0.10f);
		}

		/* 現在の再生時間を示すラベルを追加 */
		{
			timeLabel = cocos2d::LabelTTF::create(
					adx2_get_time(currentPlaybackId),
					"fonts/Marker Felt.ttf", 30);
			addLabel(timeLabel, 0.75f, 0.05f);
		}
	}

	setTouchEnabled( true );
	scheduleUpdate();
	
    return true;
}

void HelloWorld::update(float dt)
{
	/* ADX2 Atomライブラリのアップデート処理 */
	adx2_update();

	/* ラベル情報の更新 */
	vnumLabel->setString(adx2_get_voice_num());
	stateLabel->setString(g_playback_status_description[ criAtomExPlayback_GetStatus(currentPlaybackId) ]);
	timeLabel->setString(adx2_get_time(currentPlaybackId));
}


cocos2d::Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = cocos2d::Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelloWorld::HelloWorld()
{
	/* ADX2 Atomライブラリの初期化。内部的に多重初期化は回避している */
	adx2_initialize();
}

HelloWorld::~HelloWorld()
{
	/* ADX2 Atomライブラリの終了処理。thisが最後に破棄されるHelloWorldインスタンスなら実行 */
	adx2_finalize();
}

