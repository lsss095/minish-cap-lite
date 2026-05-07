// 游戏全局配置常量

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

namespace GameConfig
{
// 画面与缩放
const int LOGIC_WIDTH = 240;      // 逻辑画面宽度
const int LOGIC_HEIGHT = 160;     // 逻辑画面高度
const int SCALE = 4;              // 画面放大倍数

// 正常形态角色参数
const int PLAYER_BODY_WIDTH = 8;  // 碰撞体宽度
const int PLAYER_BODY_HEIGHT = 8; // 碰撞体高度

const int SHADOW_DRAW_WIDTH = 16; // 阴影显示宽度
const int SHADOW_DRAW_HEIGHT = 8; // 阴影显示高度

const int PLAYER_DRAW_WIDTH = 32;  // 角色贴图显示宽度
const int PLAYER_DRAW_HEIGHT = 32; // 角色贴图显示高度

const int PLAYER_FRAME_WIDTH = 32;  // 动作表帧宽度
const int PLAYER_FRAME_HEIGHT = 32; // 动作表帧高度

const int PLAYER_FRAME_COUNT = 10; // 站立/行走帧数

const int ROLL_FRAME_COUNT = 8;        // 翻滚帧数（上下方向）
const int ROLL_SIDE_FRAME_COUNT = 7;   // 翻滚帧数（左右方向）

const int MINISH1_FRAME_COUNT = 10;  // Minish1等待动画总帧数
const int MINISH1_LOOP_START = 3;    // 循环起始帧
const int MINISH1_LOOP_END = 9;      // 循环结束帧

const int PLAYER_ACTION_ROW_COUNT = 8; // 动作表行数（站立4行+行走4行）

// 动画速度
const int ANIMATION_SPEED = 2;       // 普通动作动画速度
const int ROLL_ANIMATION_SPEED = 2;  // 翻滚动画速度

// 移动步长
const int MOVE_STEP = 2;        // 正常行走步长
const int MINI_MOVE_STEP = 1;   // 缩小行走步长
const int ROLL_STEP = 3;        // 正常翻滚步长
const int MINI_ROLL_STEP = 2;   // 缩小翻滚步长

// 缩小形态角色参数
const int MINI_BODY_WIDTH = 3;  // 碰撞体宽度
const int MINI_BODY_HEIGHT = 3; // 碰撞体高度

const int MINI_SHADOW_DRAW_WIDTH = 5;  // 阴影显示宽度
const int MINI_SHADOW_DRAW_HEIGHT = 3; // 阴影显示高度

const int MINI_DRAW_WIDTH = 7;  // 角色贴图显示宽度
const int MINI_DRAW_HEIGHT = 7; // 角色贴图显示高度

const int MINI_FRAME_WIDTH = 7;  // 动作表帧宽度
const int MINI_FRAME_HEIGHT = 7; // 动作表帧高度

const int MINI_FRAME_COUNT = 7;          // 上下方向帧数（1站立+6行走）
const int MINI_SIDE_FRAME_COUNT = 6;     // 左右方向帧数（1站立+5行走）
const int MINI_ROLL_FRAME_COUNT = 5;     // 翻滚帧数（上下方向）
const int MINI_ROLL_SIDE_FRAME_COUNT = 5; // 翻滚帧数（左右方向）

// 缩小/放大流程参数
const int SHRINK_ANIMATION_FRAMES = 20; // 缩小/放大过渡动画帧数

const int SHRINK_FLASH_FRAMES = 16; // 白闪渐亮帧数

const int SHRINK_RESULT_X = 565; // 缩小后传送目标X
const int SHRINK_RESULT_Y = 312; // 缩小后传送目标Y

const int STUMP_X = 569; // 树桩区域X
const int STUMP_Y = 308; // 树桩区域Y
const int STUMP_W = 32;  // 树桩区域宽度
const int STUMP_H = 32;  // 树桩区域高度

const int SHRINK_TARGET_X = 581; // 走向目标点X（树桩中心）
const int SHRINK_TARGET_Y = 316; // 走向目标点Y

const int APPROACH_SPEED = 1; // 走向目标点的速度

const int ROLL_ARC_MAX_OFFSET = 20; // 翻滚抛物线最大偏移

} // namespace GameConfig

#endif
