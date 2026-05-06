#ifndef GAMECONFIG_H
#define GAMECONFIG_H

namespace GameConfig
{
const int LOGIC_WIDTH = 240;
const int LOGIC_HEIGHT = 160;
const int SCALE = 4;

// 影子 / 脚底 / 碰撞区域
const int PLAYER_BODY_WIDTH = 8;
const int PLAYER_BODY_HEIGHT = 8;

// 阴影图片显示大小
const int SHADOW_DRAW_WIDTH = 16;
const int SHADOW_DRAW_HEIGHT = 8;

// 林克显示大小
const int PLAYER_DRAW_WIDTH = 32;
const int PLAYER_DRAW_HEIGHT = 32;

// 动作表每一帧大小
const int PLAYER_FRAME_WIDTH = 32;
const int PLAYER_FRAME_HEIGHT = 32;

// 每个动作有 10 帧
const int PLAYER_FRAME_COUNT = 10;

// 滚动动作分别为 8 帧和 7 帧
const int ROLL_FRAME_COUNT = 8;
const int ROLL_SIDE_FRAME_COUNT = 7;

// 完整动作表一共 8 行
const int PLAYER_ACTION_ROW_COUNT = 8;

// 数字越小，动画越快
const int ANIMATION_SPEED = 2;
const int ROLL_ANIMATION_SPEED = 2;

const int MOVE_STEP = 1;
const int ROLL_STEP = 3;
}

#endif