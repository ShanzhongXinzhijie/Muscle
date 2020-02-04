#pragma once

static constexpr float METER = 16.45f / 1.65f;//1メートル
static constexpr float ONE_G = 10.0f;//1G

static constexpr float FRAME_PER_SECOND = 1.0f / 60.0f;//1fの秒数(60fps)
static constexpr int FRAME_RATE = 60;//動作フレームレート

static constexpr int PLAYER_NUM = 2;//プレイヤー数

static constexpr float MAX_HEIGHT_METER = 400.0f;//限界高度(m)
static constexpr float LIMIT_HEIGHT_METER = 250.0f;//最高高度(m)

static constexpr float MIN_Y = -3000.0f;//最低高さ

//解像度
static constexpr int SCREEN_SIZE_2D = 640;
static constexpr int SCREEN_SIZE_3D = 640*2;//1280
static constexpr int SCREEN_SPRITE_BAR = 0;//64
static constexpr int WINDOW_SIZE = 640;