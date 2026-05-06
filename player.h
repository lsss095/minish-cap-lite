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
        Approaching,   // 正在走向树桩中心
        NearStump,     // 已到达树桩中心，等待按L缩小/放大
        LeavingStump,  // 正在平滑移出树桩区域
        Shrinking,     // 缩小动画中
        Mini,          // 已缩小
        Growing        // 放大动画中
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
    bool isMini() const;   // 当前是否为缩小体积（包括 NearStump 等待放大时）

    // 当前碰撞体大小
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
    void startGrow();
    void startApproach(bool toMini); // 开始走向树桩中心，toMini=true则到达后等待缩小，false则等待放大
    void startLeaveStump();           // 开始平滑移出树桩区域
    bool isApproaching() const;
    bool isNearStumpState() const;  // 是否在树桩中心等待按L
    int stumpContactFrames() const; // 连续碰撞树桩区域的帧数
    void incStumpContactFrames();   // 碰撞树桩帧数+1
    void resetStumpContactFrames(); // 离开树桩，重置帧数
    int leaveContactFrames() const;  // NearStump下按方向键持续帧数
    void incLeaveContactFrames();
    void resetLeaveContactFrames();
    int rollArcOffset() const;       // 翻滚抛物线偏移（负值=向上）
    bool minish1FirstLoop() const;   // Minish1动画是否还在第一遍
    int shrinkFrame() const;
    int shrinkTotalFrames() const;

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

    int m_shrinkFrame;       // 缩小/放大动画当前帧
    bool m_approachToMini;   // 走向树桩后是缩小还是放大
    int m_approachPauseFrames; // 罚站剩余帧数（已废弃）
    int m_stumpContactFrames;  // 连续碰撞树桩区域的帧数
    int m_leaveTargetX;        // 离开树桩的目标X
    int m_leaveTargetY;        // 离开树桩的目标Y
    int m_leaveContactFrames;  // NearStump状态下按方向键的持续帧数
    bool m_minish1FirstLoop;   // Minish1动画是否第一遍（第一遍播全部帧，之后循环3-8帧）
};

#endif
