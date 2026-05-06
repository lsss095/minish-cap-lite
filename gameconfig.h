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

// Minish1 树桩等待动画帧数
const int MINISH1_FRAME_COUNT = 10;
const int MINISH1_LOOP_START = 3;  // 第二遍开始从第3帧循环
const int MINISH1_LOOP_END = 9;    // 循环到第9帧（0-indexed，即第10帧）

// 完整动作表一共 8 行
const int PLAYER_ACTION_ROW_COUNT = 8;

// 数字越小，动画越快
const int ANIMATION_SPEED = 2;
const int ROLL_ANIMATION_SPEED = 2;

const int MOVE_STEP = 2;
const int ROLL_STEP = 3;

// ===== 缩小形态 =====
// 缩小后碰撞体积
const int MINI_BODY_WIDTH = 4;
const int MINI_BODY_HEIGHT = 4;

// 缩小后阴影显示大小
const int MINI_SHADOW_DRAW_WIDTH = 8;
const int MINI_SHADOW_DRAW_HEIGHT = 4;

// 缩小后林克显示大小
const int MINI_DRAW_WIDTH = 16;
const int MINI_DRAW_HEIGHT = 16;

// 缩小后动作表帧大小
const int MINI_FRAME_WIDTH = 16;
const int MINI_FRAME_HEIGHT = 16;

// 缩小后动作帧数
const int MINI_FRAME_COUNT = 10;
const int MINI_ROLL_FRAME_COUNT = 8;
const int MINI_ROLL_SIDE_FRAME_COUNT = 7;

// 缩小动画持续帧数
const int SHRINK_ANIMATION_FRAMES = 20;

// 树桩区域（可缩小的位置）QRect(569, 308, 32, 32)
const int STUMP_X = 569;
const int STUMP_Y = 308;
const int STUMP_W = 32;
const int STUMP_H = 32;

// 缩小/放大时角色走向的目标点（树桩中心）
const int SHRINK_TARGET_X = 581;
const int SHRINK_TARGET_Y = 316;

// 走向目标点的速度（与行走速度一致）
const int APPROACH_SPEED = 1;

// 翻滚抛物线弧线最大偏移像素（向上为负，所以这里存绝对值）
const int ROLL_ARC_MAX_OFFSET = 20;
}

#endif