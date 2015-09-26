﻿#include "Stage.h"
#include "MainScene.h"
#include "TitleScene.h"

USING_NS_CC;

const Vec2 JUMP_IMPULSE = Vec2(0, 275);
const int MAPCHIP_SIZE = 16;
const float MAP_HEIGHT = 14;
const int MAX_LEVEL = 2;

const char* STAGE_FILE = "graphics/stage%d.tmx";

Stage::Stage()
	:_player(nullptr)
	, _tiledMap(nullptr)
	, _blocks(nullptr)
	, _state(GameState::PLAYING)
	, leftPressFlag(false)
	, rightPressFlag(false)
	, upPressFlag(false)
	, _jumpFlag(false)
	, goalFlag(false)

	, _sideMagic(nullptr)
	, _upDownMagic(nullptr)
	, playerX(0)
	, playerY(0)

	, _level(0)
	
	, blockX(0)
	, blockY(0)
	, mapX(0)
	, mapY(0)
	, tileID(0)
	, rectX(0)
	, rectY(0)
{

}

Stage::~Stage()
{
	CC_SAFE_RELEASE_NULL(_sideMagic);
	CC_SAFE_RELEASE_NULL(_upDownMagic);

	CC_SAFE_RELEASE_NULL(_player);
	CC_SAFE_RELEASE_NULL(_tiledMap);
	CC_SAFE_RELEASE_NULL(_blocks);

}

Stage* Stage::createWithLevel(int level)
{
	Stage* ret = new Stage();
	if (ret->initWithLevel(level))
	{
		ret->autorelease();

		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}



void Stage::jumpMethod()
{
	_player->getPhysicsBody()->applyImpulse(JUMP_IMPULSE);
	setJumpFlag(false);
}

//プレイヤーの操作
void Stage::playerMove()
{
	/*
	スペースキーを押すと　向いている方向に横反射
	Aを押すと左を向いて横反射
	Dを押すと右を向いて横反射
	Wを押すと上反射
	Sを押すと下反射
	*/
	//cocos2d::EventListenerKeyboard型のポインタ変数keyboardListenerを宣言し、EventListenerKeyboard::createを代入
	auto keyboardListener = EventListenerKeyboard::create();

	//キーボードが押された時のstopを書く関数？
	keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_R)
		{

			auto mainScene = MainScene::createSceneWithLevel(_level);
			auto mainTransition = TransitionFade::create(1.0f, mainScene);
			Director::getInstance()->replaceScene(mainTransition);
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();

			auto titleScene = TitleScene::createScene();
			auto titleTransition = TransitionFade::create(1.0f, titleScene);
			Director::getInstance()->replaceScene(titleTransition);
		}

		auto flip = FlipX::create(true);
		auto flipback = FlipX::create(false);

		//もし押されたキーが←だったら
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
		{
			leftPressFlag = true;
			_player->rightFlag = false;
			if (rightPressFlag == true)
			{
				rightPressFlag = false;
			}
		}
		//そうではなくもし、キーが→だったら
		if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
		{
			rightPressFlag = true;
			_player->rightFlag = true;

			if (leftPressFlag == true)
			{
				leftPressFlag = false;
			}
		}
		//もし押されたキーが↑だったら
		if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
		{
			upPressFlag = true;
		}

		//そうではなくもし押されたキーがスペースだったら
		if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
		{
			if (getJumpFlag() == true)
			{
				_player->magicFlag = true;
				Magic* sideMagic = _player->sideMirrorEffect();
				this->setSideMagic(sideMagic);
				_sideMagic->setPosition(_player->LRMagicPosition);
				this->addChild(_sideMagic);
			}

			for (Blocks* mirrorBlock : _mirrorAbleBlocks)
			{
				_mirrorAblePositions.push_back(BlockVecConvert(mirrorBlock->getPosition()));


				std::vector<float> playerDiffPositions;
				float diffPosition;
				if (_player->magicFlag == true)
				{
					for (Vec2 mirrorPosition : _mirrorAblePositions)
					{
						//右反射
						if (_player->rightFlag == true)
						{

							if (_standBlockPosition.x < mirrorPosition.x)
							{
								diffPosition = mirrorPosition.x - _standBlockPosition.x;
								playerDiffPositions.push_back(diffPosition);
								float rightMirrorMove = StageVecConvertX(_standBlockPosition.x - diffPosition);
								MoveTo* rightMoveAction = MoveTo::create(0.2, Vec2(rightMirrorMove, mirrorBlock->getPosition().y));
								mirrorBlock->runAction(rightMoveAction);
								//log("Rcount = %f", _standBlockPosition.x - diffPosition * MAPCHIP_SIZE);
							}
						}
						//左反射
						else
						{
							if (_standBlockPosition.x > mirrorPosition.x)
							{
								diffPosition = _standBlockPosition.x - mirrorPosition.x;
								playerDiffPositions.push_back(diffPosition);
								float leftMirrorMove = StageVecConvertX(_standBlockPosition.x + diffPosition);
								MoveTo* leftMoveAction = MoveTo::create(0.2, Vec2(leftMirrorMove, mirrorBlock->getPosition().y));
								mirrorBlock->runAction(leftMoveAction);
								//log("Lcount = %f", diffPosition);
							}
						}
					}
				}
				_mirrorAblePositions.clear();
				playerDiffPositions.clear();
			}
		}

		//上反射
		if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			_player->upFlag = true;
			if (_player->downFlag == true){ _player->downFlag = false; }
			if (getJumpFlag() == true)
			{
				_player->magicFlag = true;
				Magic* upMagic = _player->upDownMirrorEffect();
				this->setUpDownMagic(upMagic);
				_upDownMagic->setPosition(_player->UDMagicPosition);
				this->addChild(_upDownMagic);
			}

			for (Blocks* mirrorBlock : _mirrorAbleBlocks)
			{
				_mirrorAblePositions.push_back(BlockVecConvert(mirrorBlock->getPosition()));


				std::vector<float> playerDiffPositions;
				float diffPosition;
				if (_player->magicFlag == true)
				{
					for (Vec2 mirrorPosition : _mirrorAblePositions)
					{
							if (_standBlockPosition.y > mirrorPosition.y)
							{
								diffPosition = _standBlockPosition.y - mirrorPosition.y;
								//playerDiffPositions.push_back(diffPosition);
								float upMirrorMove = StageVecConvertY(_standBlockPosition.y + diffPosition);
								MoveTo* upMoveAction = MoveTo::create(0.2, Vec2(mirrorBlock->getPosition().x, upMirrorMove));
								mirrorBlock->runAction(upMoveAction);
								//log("Rcount = %f", upMirrorMove);
							}
						}

				}
				_mirrorAblePositions.clear();
				playerDiffPositions.clear();
			}
		}

		//下反射
		if (keyCode == EventKeyboard::KeyCode::KEY_S)
		{
			_player->downFlag = true;
			if (_player->upFlag == true){ _player->upFlag = false; }

			if (getJumpFlag() == true)
			{
				_player->magicFlag = true;
				Magic* upMagic = _player->upDownMirrorEffect();
				this->setUpDownMagic(upMagic);
				_upDownMagic->setPosition(_player->UDMagicPosition);
				this->addChild(_upDownMagic);
			}

			for (Blocks* mirrorBlock : _mirrorAbleBlocks)
			{
				_mirrorAblePositions.push_back(BlockVecConvert(mirrorBlock->getPosition()));


				std::vector<float> playerDiffPositions;
				float diffPosition;
				if (_player->magicFlag == true)
				{
					for (Vec2 mirrorPosition : _mirrorAblePositions)
					{
						if (_standBlockPosition.y < mirrorPosition.y)
						{
							diffPosition = mirrorPosition.y - _standBlockPosition.y;
							playerDiffPositions.push_back(diffPosition);
							float downMirrorMove = StageVecConvertY(_standBlockPosition.y - diffPosition);
							MoveTo* downMoveAction = MoveTo::create(0.2, Vec2(mirrorBlock->getPosition().x, downMirrorMove));
							mirrorBlock->runAction(downMoveAction);
							//log("Rcount = %f", downMirrorMove);
						}
					}

				}
				_mirrorAblePositions.clear();
				playerDiffPositions.clear();
			}
		}

		//左反射
		if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			_player->rightFlag = false;
			_player->runAction(flip);

			if (getJumpFlag() == true)
			{
				_player->magicFlag = true;

				Magic* sideMagic = _player->sideMirrorEffect();
				this->setSideMagic(sideMagic);
				_sideMagic->setPosition(_player->LRMagicPosition);
				this->addChild(_sideMagic);
			}

			for (Blocks* mirrorBlock : _mirrorAbleBlocks)
			{
				_mirrorAblePositions.push_back(BlockVecConvert(mirrorBlock->getPosition()));


				std::vector<float> playerDiffPositions;
				float diffPosition;
				if (_player->magicFlag == true)
				{
					for (Vec2 mirrorPosition : _mirrorAblePositions)
					{
						if (_player->rightFlag == false)
						{
							if (_standBlockPosition.x > mirrorPosition.x)
							{
								diffPosition = _standBlockPosition.x - mirrorPosition.x;
								playerDiffPositions.push_back(diffPosition);
								float leftMirrorMove = StageVecConvertX(_standBlockPosition.x + diffPosition);
								MoveTo* leftMoveAction = MoveTo::create(0.2, Vec2(leftMirrorMove, mirrorBlock->getPosition().y));
								mirrorBlock->runAction(leftMoveAction);
							}
						}

					}
				}
				_mirrorAblePositions.clear();
				playerDiffPositions.clear();
			}
		}

		//右反射
		if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			_player->rightFlag = true;
			_player->runAction(flipback);

			if (getJumpFlag() == true)
			{
				_player->magicFlag = true;

				Magic* sideMagic = _player->sideMirrorEffect();
				this->setSideMagic(sideMagic);
				_sideMagic->setPosition(_player->LRMagicPosition);
				this->addChild(_sideMagic);
			}

			for (Blocks* mirrorBlock : _mirrorAbleBlocks)
			{
				_mirrorAblePositions.push_back(BlockVecConvert(mirrorBlock->getPosition()));


				std::vector<float> playerDiffPositions;
				float diffPosition;
				if (_player->magicFlag == true)
				{
					for (Vec2 mirrorPosition : _mirrorAblePositions)
					{
						if (_player->rightFlag == true)
						{
							if (_standBlockPosition.x < mirrorPosition.x)
							{
								diffPosition = mirrorPosition.x - _standBlockPosition.x;
								playerDiffPositions.push_back(diffPosition);
								float rightMirrorMove = StageVecConvertX(_standBlockPosition.x - diffPosition);
								MoveTo* rightMoveAction = MoveTo::create(0.2, Vec2(rightMirrorMove, mirrorBlock->getPosition().y));
								mirrorBlock->runAction(rightMoveAction);
								//log("Rcount = %f", _standBlockPosition.x - diffPosition * MAPCHIP_SIZE);
							}
						}

					}
				}
				_mirrorAblePositions.clear();
				playerDiffPositions.clear();
			}
		}

		
	};

	//たぶんキーを離した時のstop　詳細わかんない
	keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event) {

		//もしも離されたキーが←、または→だったら
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
		{
			leftPressFlag = false;
		}
		if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
		{

			rightPressFlag = false;
		}
		if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
		{
			upPressFlag = false;
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_SPACE || keyCode == EventKeyboard::KeyCode::KEY_A || keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			_player->magicFlag = false;
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			_player->magicFlag = false;
			_player->upFlag = false;
		}
		if (keyCode == EventKeyboard::KeyCode::KEY_S)
		{
			_player->magicFlag = false;
			_player->downFlag = false;
		}
	};

	//キーボードのstopを書いた後のおまじないみたいなもの。
	//(ぎぎさんのコメントより抜粋→)EventListenerってのにキーを押したときとか、話したときみたいなstopを紐付けておいて
	//最後にEventDispatcherっていうものに、今作ったEventListenerを登録して、「よしなにお願いします」って伝えるstop
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

//ブロックの生成メソッド
Blocks* Stage::BlockGen(int gid)
{
	Blocks* blockGen = Blocks::create();
	blockGen->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	//剛体マテリアル設定
	auto material = PhysicsMaterial();
	//摩擦
	material.friction = 99;
	material.restitution = 0.0;

	//剛体設置
	if (gid == 1 || gid == 4 || gid == 6 || gid == 7 || gid == 8 || gid == 9 || gid == 12 || gid == 13){
	auto category = 1;
	Point box[4]{Point(-8, -8), Point(-8, 8), Point(8, 8), Point(8, -8)};
	auto physicsBody = PhysicsBody::createEdgeChain(box,4, material);
	physicsBody->setDynamic(false);
	physicsBody->setCategoryBitmask(category);
	physicsBody->setContactTestBitmask(static_cast<int>(TileType::PLAYER));
	physicsBody->setCollisionBitmask(static_cast<int>(TileType::PLAYER));
	blockGen->setPhysicsBody(physicsBody);
	_allBlocks.pushBack(blockGen);
	}
	auto tileSize = Size(5, 4);
	const int X_MAX = tileSize.width;

	rectX = ((gid - 1) % X_MAX + 1) - 1;
	rectY = (int)((gid - 1) / X_MAX);

	blockGen->setTextureRect(Rect(rectX * MAPCHIP_SIZE, rectY * MAPCHIP_SIZE, MAPCHIP_SIZE, MAPCHIP_SIZE));


	auto blockRect = blockGen->getBoundingBox();

	return blockGen;
}

//ステージ座標をマップ座標に変換
Vec2 Stage::BlockVecConvert(Vec2 blockAncorVecs)
{
	Vec2 Position = blockAncorVecs;
	float x = floor((Position.x) / MAPCHIP_SIZE);
	float y = MAP_HEIGHT - floor((Position.y) / MAPCHIP_SIZE) - 1;
	return  Vec2(x, y);
}

//マップ座標をステージ座標に変換
Vec2 Stage::StageVecConvert(Vec2 blockAncorVecs)
{
	Vec2 Position = blockAncorVecs;
	float x = floor((Position.x) * MAPCHIP_SIZE) + MAPCHIP_SIZE / 2;
	float y = MAP_HEIGHT * MAPCHIP_SIZE - floor((Position.y + 1)* MAPCHIP_SIZE) + MAPCHIP_SIZE / 2;
	return  Vec2(x, y);
}
float Stage::StageVecConvertX(float blockAncorVecs)
{
	float Position = blockAncorVecs;
	float x = floor((Position) * MAPCHIP_SIZE) + MAPCHIP_SIZE / 2;
	return  x;
}
float Stage::StageVecConvertY(float blockAncorVecs)
{
	float Position = blockAncorVecs;
	float y = MAP_HEIGHT * MAPCHIP_SIZE - floor((Position + 1)* MAPCHIP_SIZE) + MAPCHIP_SIZE / 2;
	return  y;
}

//BGM再生とロード
void Stage::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	auto soundEngine = CocosDenshion::SimpleAudioEngine::getInstance();
	soundEngine->setBackgroundMusicVolume(0.6f);
	soundEngine->preloadBackgroundMusic("sounds/main_bgm.wav");
	soundEngine->preloadBackgroundMusic("sounds/se_clear.wav");

	soundEngine->playBackgroundMusic("sounds/main_bgm.wav", true);

	this->scheduleUpdate();
}

//メイン
void Stage::update(float dt)
{
	//Vec2型の_velocityという変数を整数値に直す？
	_velocity.normalize();

	const float  SPEED = 1.5;
	//自身の位置を、現在地＋ベクトル＊SPEEDの値にする
	_player->setPosition(_player->getPosition() + _velocity * SPEED);

	_playerPosition = _player->getPosition();

	Size winSize = Director::getInstance()->getWinSize();


	if (_playerPosition.x >= winSize.width)
	{
		_player->setPositionX(winSize.width);
		_velocity.x = 0;
	}

	else if (_playerPosition.x <= 0)
	{
		_player->setPositionX(0);
		_velocity.x = 0;
	}

	if (_playerPosition.y >= winSize.height)
	{
		_player->setPositionY(winSize.height);
		_velocity.y = 0;
	}

	else if (_playerPosition.y <= 0)
	{
		_player->setPositionY(0);
		_velocity.y = 0;
	}

	auto flip = FlipX::create(true);
	auto flipback = FlipX::create(false);
	if (_state == GameState::PLAYING)
	{
		if (leftPressFlag == true)
		{

			if (getJumpFlag() == true)
			{
				_player->playAnimation(1);
			}
			_player->runAction(flip);
			_velocity.x = -2;
		}

		if (rightPressFlag == true)
		{

			if (getJumpFlag() == true)
			{
				_player->playAnimation(1);
			}
			_player->runAction(flipback);
			_velocity.x = 2;
		}

		if (leftPressFlag == false && rightPressFlag == false)
		{
			if (getJumpFlag() == true)
			{
				if (_player->magicFlag == true)
				{
					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/se_mirror.wav");

					if (_player->upFlag == true)
					{
						_player->playAnimation(5);
					}
					else if (_player->downFlag == true)
					{
						_player->playAnimation(6);
					}
					else
					{
						_player->playAnimation(4);
					}

				}
				else{ _player->playAnimation(0); }
			}
			_velocity.x = 0;
		}

		if (upPressFlag == true)
		{
			if (getJumpFlag() == true)
			{
				jumpMethod();
			}
		}


		if (_player->stopR == true)
		{
			if (_velocity.x >= 0)
			{
				_velocity.x = 0;
			}
			else
			{
				_player->stopR = false;
			}
		}
		if (_player->stopL == true)
		{
			if (_velocity.x <= 0)
			{
				_velocity.x = 0;
			}
			else
			{
				_player->stopL = false;
			}
		}

		for (Vec2 point : _neighborBlockPositions)
		{
			Rect blockRect = Rect(point.x - MAPCHIP_SIZE / 2, point.y - MAPCHIP_SIZE / 2, MAPCHIP_SIZE, MAPCHIP_SIZE);
			if (_player->getPosition().x - blockRect.getMinX() > 0 && blockRect.getMaxX() - _player->getPosition().x > 0)
			{
				_standBlockPosition = BlockVecConvert(point);

				if (_player->rightFlag == true)
				{

					_player->LRMagicPosition = Vec2(blockRect.getMaxX(), -224);
				}
				else
				{
					_player->LRMagicPosition = Vec2(blockRect.getMinX(), -224);
				}

				if (_player->upFlag == true)
				{

					_player->UDMagicPosition = Vec2(-384, blockRect.getMinY());
				}
				else
				{
					_player->UDMagicPosition = Vec2(-384, blockRect.getMaxY());
				}
			}
		}

		if (Vec2(_standBlockPosition.x, _standBlockPosition.y - 1) == goalVec)
		{
			goalFlag = true;
		}
		else
		{
			goalFlag = false;
		}


		if (goalFlag == true)
		{
			if (_playerPosition.y == _prevPosition.y)
			{
				_state = GameState::RESULT;

				if (_state == GameState::RESULT)
				{
					CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/se_clear.wav", false);
				}
			}
			_prevPosition = _playerPosition;

		}

		playerX = BlockVecConvert(_player->getPosition()).x;
		playerY = BlockVecConvert(_player->getPosition()).y;
		playerMapVec = Vec2(playerX, playerY);
		if (getJumpFlag() == false)
		{
			if (_playerPosition.y > _prevPosition.y)
			{
				_player->playAnimation(2);
				for (Blocks* block : _allBlocks)
				{

					if (playerMapVec.x == BlockVecConvert(block->getPosition()).x)
					{
						if (playerMapVec.y - 1 == BlockVecConvert(block->getPosition()).y || playerMapVec.y == BlockVecConvert(block->getPosition()).y)
						{
							block->getPhysicsBody()->setCategoryBitmask(0);
						}
						else if (block->getPhysicsBody()->getCategoryBitmask() == 0)
						{
							block->getPhysicsBody()->setCategoryBitmask(1);
						}
					}

				}
			}

			else if (_playerPosition.y < _prevPosition.y)
			{
				_player->playAnimation(3);
			}
			_prevPosition = _playerPosition;
		}

		if (_player->magicFlag == false)
		{
			this->removeChild(_sideMagic);
			this->removeChild(_upDownMagic);

		}
	}
	if (_state == GameState::RESULT)
	{
		_velocity.x = 0;

		_player->playAnimation(7);

	}
}
bool Stage::initWithLevel(int level)
{
	if (!Layer::init())
	{
		return false;
	}
	//現ステージ数を格納
	_level = level;

	//剛体の接触チェック
	auto contactListener = EventListenerPhysicsContact::create();

	contactListener->onContactBegin = [this](PhysicsContact&contact){

		//プレイヤーではない方を抽出
		PhysicsShape* ground = contact.getShapeA()->getBody() == _player->getPhysicsBody() ? contact.getShapeB() : contact.getShapeA();
		Node* groundNode = ground->getBody()->getNode();
		nowBlockPosition = groundNode->getPosition();
		//log("%f", worldPos.x);
		_neighborBlockPositions.push_back(nowBlockPosition);

		auto nowBlockVec = BlockVecConvert(nowBlockPosition);

		//カテゴリ抽出
		int category = groundNode->getPhysicsBody()->getCategoryBitmask();

		Rect groundRect = groundNode->getBoundingBox();
		float groundTopY = groundRect.getMaxY();

		float minX = groundRect.origin.x;
		float maxX = groundRect.getMaxX() ;
	
		Rect playerRect = _player->getBoundingBox();
		float playerBottomY = playerRect.origin.y;

		if (category & static_cast<int>(Stage::TileType::BLOCKS))
		{

			if (groundTopY <= playerBottomY)
			{
				setJumpFlag(true);
			}
			else
			{
				if (maxX >= playerRect.getMinX() && nowBlockPosition.x < _player->getPositionX())
				{
					
					_player->stopL = true;
				}
				else if (minX <= playerRect.getMaxX() && nowBlockPosition.x > _player->getPositionX())
				{
					_player->stopR = true;
				}
			}
		}

		return true;
	};
	contactListener->onContactSeperate = [this](PhysicsContact&contact){
		if (_neighborBlockPositions.size() > 0)
		{
			_neighborBlockPositions.erase(_neighborBlockPositions.begin());
		}
		if (_neighborBlockPositions.size() == 0)
		{
			_player->stopL = false;
			_player->stopR = false;
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	//画面サイズ取得
	Size winSize = Director::getInstance()->getWinSize();
	
	//背景表示
	auto background1 = Sprite::create("graphics/sky.png");
	background1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	background1->setPosition(Vec2(0, 0));
	background1->getTexture()->setAliasTexParameters();

	auto background2 = Node::create();
	background2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	auto sky1 = Sprite::create("graphics/cloud.png");
	sky1->setPosition(Vec2(0, 0));
	sky1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	sky1->getTexture()->setAliasTexParameters();

	auto sky2 = Sprite::create("graphics/cloud.png");
	sky2->setPosition(winSize.width, 0);
	sky2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	sky2->getTexture()->setAliasTexParameters();

	background2->addChild(sky1);
	background2->addChild(sky2);
	//雲動かす
	background2->runAction(
		RepeatForever::create(
		Sequence::create(
		MoveTo::create(32, (Vec2(-winSize.width, 0))),
		Place::create(Vec2::ZERO),
		NULL
		)));

	//背景空
	this->addChild(background1);
	//背景雲
	this->addChild(background2);
	
	//乗れる部分 
	auto stageFile = StringUtils::format(STAGE_FILE, level);
	auto map = TMXTiledMap::create(stageFile);

	//レイヤー抽出
	TMXLayer* layer = map->getLayer("ground");

	//マップの大きさ
	auto mapSize = map->getMapSize();
	auto category = 1;

	for (int x = 0; x < mapSize.width; x++)
	{
		for (int y = 0; y < mapSize.height; y++)
		{
			auto coordinate = Vec2(x, y);
			int tileID = layer->getTileGIDAt(coordinate);
			auto tileMap = layer->getTileAt(coordinate);
	
			if (tileID >= 1)
			{
				_blockGen = BlockGen(tileID);
				_blockGen->setPosition(tileMap->getPosition().x + MAPCHIP_SIZE / 2, tileMap->getPosition().y + MAPCHIP_SIZE / 2);
				this->addChild(_blockGen);

				if (tileID == 4)
				{
					_blockGen->setLocalZOrder(5);
					_mirrorAbleBlocks.push_back(_blockGen);
				}
				if (tileID == 10)
				{
					goalVec = BlockVecConvert(_blockGen->getPosition());
				}

				tileMap->removeFromParent();
			}			
		}
	}

	//キャラ配置
	auto luk = Player::create();
	this->setPlayer(luk);
	luk->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	luk->setPosition(Vec2(64, 92));
	luk->getTexture()->setAliasTexParameters();
	luk->setLocalZOrder(10);
	_player->playAnimation(0);
	this->addChild(luk);
	_prevPosition = luk->getPosition();

	playerMove(); 

	Sprite* guide = Sprite::create("graphics/mainguide.png");
	guide->setPosition(Vec2(5, 210));
	guide->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	guide->getTexture()->setAliasTexParameters();
	this->addChild(guide);
	if (_state == GameState::RESULT && level <= MAX_LEVEL)
	{
		log("check");
		level++;
	}
	this->scheduleUpdate();

	return true;
}

