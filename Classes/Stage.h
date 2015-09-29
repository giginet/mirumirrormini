#ifndef __STAGE_H__
#define __STAGE_H__

#include "vector"
#include "math\CCMath.h"
#include "cocos2d.h"
#include "AudioEngine.h"

#include "Player.h"
#include "Blocks.h"


class Stage : public cocos2d::Layer
{

protected:

	Stage();
	virtual ~Stage() override;
	//bool init() override;
	bool initWithLevel(int level);

	bool leftPressFlag;
	bool rightPressFlag;

	bool magicUse;
	bool moveFlag;

	bool upPressFlag;

	bool goalFlag;
	bool gameoverFlag;

	void playerMove();
	void jumpMethod();
	float playerX;
	float playerY;
	
	//BlockGen
	int blockX;
	int blockY;
	int mapX;
	int mapY;
	int rectX;
	int rectY;
	int tileID;

	//BGMのID
	int mainBgmID;
	//次のステージ
	int nextLevel;


	//魔法停止時間
	int timeCount;


	enum class GameState
	{
		PLAYING,

		RESULT,

		GAMEOVER
	};


	cocos2d::Vector<Blocks*> _allBlocks;

	//現在たっているブロック用のガイド
	cocos2d::Vec2 testVec;

	//ゴールの場所
	cocos2d::Vec2 goalVec;

	//プレイヤーの移動
	cocos2d::Vec2 _velocity;
	//プレイヤーの1フレーム前の座標
	cocos2d::Vec2 _prevPosition;

	//プレイヤーの現在の座標
	cocos2d::Vec2 _playerPosition;

	//現在たっているブロック用のガイド
	cocos2d::Vec2 playerMapVec;


	//現在接触しているブロックの座標
	cocos2d::Vec2 nowBlockPosition;
	//現在たっているブロックのマップ座標
	cocos2d::Vec2 _standBlockPosition;

	//現在接触しているブロックの座標を格納する配列
	std::vector<cocos2d::Vec2> _neighborBlockPositions;

	//反射ブロックを格納する配列
	std::vector<Blocks*> _mirrorAbleBlocks;


	//反射ブロックのマップ座標を格納する配列
	std::vector<cocos2d::Vec2> _mirrorAblePositions;

	//引数に渡されたStage上の座標をマップ座標に変換するメソッド
	cocos2d::Vec2 BlockVecConvert(cocos2d::Vec2 blockAncorVecs);
	//引数に渡されたマップ座標をStage上の座標に変換するメソッド
	cocos2d::Vec2 StageVecConvert(cocos2d::Vec2 blockAncorVecs);
	
	//引数に渡されたXのマップ座標をStage上のX座標に変換するメソッド
	float StageVecConvertX(float blockAncorVecs);
	//引数に渡されたYのマップ座標をStage上のY座標に変換するメソッド
	float StageVecConvertY(float blockAncorVecs);

	//現在たっているブロックのガイドで表示しているスプライト
	cocos2d::Sprite* testBlock;

	//クリア後の表示
	cocos2d::Sprite* clearNext;
	cocos2d::Sprite* clearTitle;


	//Blocksクラスの変数_blockGen　ブロックの生成メソッドを代入
	Blocks* _blockGen;


	//　void型の長い名前の関数の宣言
	void onEnterTransitionDidFinish() override;

public:

	enum TileType
	{
		BLOCKS = 1 << 0,
		GOAL = 1 << 1,
		PLAYER = 2 << 0
	};

	void onResult();

	void update(float dt) override;

	//横反射のエフェクト用のメソッド
	Magic* sideMirrorEffect();
	//gidによってタイルを切り出し表示するメソッド
	Blocks* BlockGen(int gid);

	CC_SYNTHESIZE(bool, _jumpFlag, JumpFlag);

	CC_SYNTHESIZE_READONLY(int, _level, Level);

	CC_SYNTHESIZE_RETAIN(Magic*, _sideMagic, SideMagic);
	CC_SYNTHESIZE_RETAIN(Magic*, _upDownMagic, UpDownMagic);

	CC_SYNTHESIZE_RETAIN(cocos2d::TMXTiledMap*, _tiledMap, TiledMap);
	CC_SYNTHESIZE_RETAIN(Player*, _player, Player);
	CC_SYNTHESIZE_RETAIN(Blocks*, _blocks, Blocks);
	CC_SYNTHESIZE(GameState, _state, State);
	CC_SYNTHESIZE(int, _mainBgmID, MainBgmID);
	static Stage* createWithLevel(int level);

};

#endif //__STAGE_H__