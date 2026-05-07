// 角色类：管理林克的状态、动作、缩小/放大流程

#ifndef PLAYER_H
#define PLAYER_H

#include <QRect>

class Player
{
public:
    enum Direction
    {
        Down,
        Left,
        Right,
        Up,
    };

    enum Action
    {
        Idle,
        Walk,
        Roll,
        Minish1  // 树桩上缩小等待动画
    };

    enum SizeState
    {
        Normal,
        Approaching,           // 走向树桩中心
        NearStump,             // 到达树桩中心，等待按L
        LeavingStump,          // 平滑移出树桩区域
        FlashWhite,            // 全屏白闪
        Shrinking,             // 缩小动画中
        Mini,                  // 已缩小
        Growing,               // 放大动画中
        FlashWhiteApproaching  // 白闪+走向树桩中心
    };

    Player();

    int x() const;
    int y() const;

    Direction direction() const;
    Action action() const;
    SizeState sizeState() const;

    int animationFrame() const;
    int actionRow() const;
    int currentActionFrameCount() const;

    bool isMoving() const;
    bool isActionLocked() const;
    bool canRoll() const;
    bool isMini() const;   // 是否为缩小体积
    bool isShrunk() const; // 是否处于缩小状态

    int bodyWidth() const;
    int bodyHeight() const;

    void moveTo(int x, int y);
    void setDirection(Direction dir);
    void setAction(Action action);

    void startRoll();

    void startMoving();
    void stopMoving();
    void updateAnimation();

    // 缩小/放大
    void startShrink();
    void startShrinkFlash();  // 白闪覆盖翻滚走向树桩中心（缩小）
    void startGrow();
    void startGrowFlash();   // 白闪覆盖翻滚走向树桩中心（放大）
    void startApproach(bool toMini); // 走向树桩中心
    void startLeaveStump();           // 平滑移出树桩区域
    bool isApproaching() const;
    bool isNearStumpState() const;
    int stumpContactFrames() const;  // 连续碰撞树桩帧数
    void incStumpContactFrames();
    void resetStumpContactFrames();
    int leaveContactFrames() const;  // NearStump下按方向键帧数
    void incLeaveContactFrames();
    void resetLeaveContactFrames();
    int rollArcOffset() const;       // 翻滚抛物线偏移
    bool minish1FirstLoop() const;   // Minish1是否第一遍
    int flashFrame() const;
    int flashTotalFrames() const;
    int shrinkFrame() const;
    int shrinkTotalFrames() const;
    int lKeyFrames() const;   // L键持续帧数
    void incLKeyFrames();
    void resetLKeyFrames();

    QRect rect() const;

private:
    int m_x;
    int m_y;

    Direction m_direction;
    Action m_action;
    SizeState m_sizeState;

    bool m_isMoving;
    bool m_actionLocked;

    int m_animationFrame;
    int m_animationCounter;
    int m_rollCooldown;

    int m_shrinkFrame;         // 缩小/放大动画当前帧
    bool m_approachToMini;     // 走向树桩后缩小还是放大
    int m_approachPauseFrames;
    int m_stumpContactFrames;  // 连续碰撞树桩帧数
    int m_leaveTargetX;        // 离开树桩目标X
    int m_leaveTargetY;        // 离开树桩目标Y
    int m_leaveContactFrames;  // NearStump按方向键帧数
    bool m_minish1FirstLoop;   // Minish1第一遍
    int m_flashFrame;          // 白闪当前帧
    int m_lKeyFrames;          // L键持续帧数
    bool m_isShrunk;           // 是否缩小状态
};

#endif
