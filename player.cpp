#include "player.h"
#include "gameconfig.h"

Player::Player()
    : m_x(532),
      m_y(76),
      m_direction(Down),
      m_action(Idle),
      m_sizeState(Normal),
      m_isMoving(false),
      m_actionLocked(false),
      m_animationFrame(0),
      m_animationCounter(0),
      m_rollCooldown(0),
      m_shrinkFrame(0),
      m_approachToMini(true),
      m_approachPauseFrames(0),
      m_stumpContactFrames(0),
      m_leaveTargetX(0),
      m_leaveTargetY(0),
      m_leaveContactFrames(0),
      m_minish1FirstLoop(true)
{
}

int Player::x() const
{
    return m_x;
}

int Player::y() const
{
    return m_y;
}

Player::Direction Player::direction() const
{
    return m_direction;
}

Player::Action Player::action() const
{
    return m_action;
}

Player::SizeState Player::sizeState() const
{
    return m_sizeState;
}

int Player::animationFrame() const
{
    return m_animationFrame;
}

bool Player::isMoving() const
{
    return m_isMoving;
}

bool Player::isActionLocked() const
{
    return m_actionLocked;
}

bool Player::canRoll() const
{
    return !m_actionLocked && m_rollCooldown <= 0
           && (m_sizeState == Normal || m_sizeState == Mini || m_sizeState == NearStump);
}

bool Player::isMini() const
{
    // Mini 状态，或在 NearStump 等待放大时（之前是 Mini 走过来的）
    return m_sizeState == Mini
        || (m_sizeState == NearStump && !m_approachToMini);
}

int Player::bodyWidth() const
{
    return isMini() ? GameConfig::MINI_BODY_WIDTH : GameConfig::PLAYER_BODY_WIDTH;
}

int Player::bodyHeight() const
{
    return isMini() ? GameConfig::MINI_BODY_HEIGHT : GameConfig::PLAYER_BODY_HEIGHT;
}

void Player::moveTo(int x, int y)
{
    m_x = x;
    m_y = y;
}

void Player::setDirection(Direction dir)
{
    if (m_actionLocked) return;

    m_direction = dir;
}

void Player::setAction(Action action)
{
    if (m_action != action) {
        m_action = action;
        m_animationFrame = 0;
        m_animationCounter = 0;
    }
}

void Player::startMoving()
{
    if (m_actionLocked) return;

    // NearStump 状态下开始移动时，恢复到 Normal/Mini 自由状态
    if (m_sizeState == NearStump) {
        m_sizeState = m_approachToMini ? Normal : Mini;
    }

    m_isMoving = true;
    setAction(Walk);
}

void Player::stopMoving()
{
    if (m_actionLocked) return;

    m_isMoving = false;
    setAction(Idle);
}

void Player::startRoll()
{
    if (m_actionLocked) return;

    m_isMoving = false;
    m_actionLocked = true;
    setAction(Roll);
}

void Player::startShrink()
{
    m_sizeState = Shrinking;
    m_shrinkFrame = 0;
    m_actionLocked = true;
    m_isMoving = false;
    setAction(Idle);
}

void Player::startGrow()
{
    m_sizeState = Growing;
    m_shrinkFrame = 0;
    m_actionLocked = true;
    m_isMoving = false;
    setAction(Idle);
}

void Player::startApproach(bool toMini)
{
    m_sizeState = Approaching;
    m_approachToMini = toMini;
    m_actionLocked = true;
    m_isMoving = false;
    m_stumpContactFrames = 0;
    // 根据目标位置决定朝向
    int dx = GameConfig::SHRINK_TARGET_X - m_x;
    int dy = GameConfig::SHRINK_TARGET_Y - m_y;
    if (qAbs(dy) >= qAbs(dx)) {
        m_direction = dy > 0 ? Down : Up;
    } else {
        m_direction = dx > 0 ? Right : Left;
    }
    setAction(Roll);
}

void Player::startLeaveStump()
{
    m_sizeState = LeavingStump;
    m_actionLocked = true;
    m_isMoving = false;
    m_stumpContactFrames = 0;
    m_leaveContactFrames = 0;

    // 根据当前朝向，计算离开树桩的目标点（沿方向移出树桩区域边界）
    int stumpCX = GameConfig::STUMP_X + GameConfig::STUMP_W / 2;
    int stumpCY = GameConfig::STUMP_Y + GameConfig::STUMP_H / 2;
    int halfW = GameConfig::STUMP_W / 2 + bodyWidth();
    int halfH = GameConfig::STUMP_H / 2 + bodyHeight();

    switch (m_direction) {
    case Up:
        m_leaveTargetX = m_x;
        m_leaveTargetY = GameConfig::STUMP_Y - bodyHeight();
        break;
    case Down:
        m_leaveTargetX = m_x;
        m_leaveTargetY = GameConfig::STUMP_Y + GameConfig::STUMP_H;
        break;
    case Left:
        m_leaveTargetX = GameConfig::STUMP_X - bodyWidth();
        m_leaveTargetY = m_y;
        break;
    case Right:
        m_leaveTargetX = GameConfig::STUMP_X + GameConfig::STUMP_W;
        m_leaveTargetY = m_y;
        break;
    }

    setAction(Roll);
}

bool Player::isApproaching() const
{
    return m_sizeState == Approaching;
}

bool Player::isNearStumpState() const
{
    return m_sizeState == NearStump;
}

int Player::stumpContactFrames() const
{
    return m_stumpContactFrames;
}

void Player::incStumpContactFrames()
{
    m_stumpContactFrames++;
}

void Player::resetStumpContactFrames()
{
    m_stumpContactFrames = 0;
}

int Player::leaveContactFrames() const
{
    return m_leaveContactFrames;
}

void Player::incLeaveContactFrames()
{
    m_leaveContactFrames++;
}

void Player::resetLeaveContactFrames()
{
    m_leaveContactFrames = 0;
}

int Player::rollArcOffset() const
{
    // 只在滚上/滚下树桩时使用抛物线偏移
    if (m_sizeState != Approaching && m_sizeState != LeavingStump)
        return 0;

    int totalFrames = currentActionFrameCount();
    if (totalFrames <= 0) return 0;

    // 用 animationCounter 做更精细的插值
    // 实际进度 = (animationFrame + animationCounter / ROLL_ANIMATION_SPEED) / totalFrames
    float progress = (static_cast<float>(m_animationFrame)
                      + static_cast<float>(m_animationCounter) / GameConfig::ROLL_ANIMATION_SPEED)
                     / totalFrames;

    if (progress > 1.0f) progress = 1.0f;

    // 抛物线: offset = -4a * (progress - 0.5)^2 + a
    // progress=0.5 时最大偏移为 -ROLL_ARC_MAX_OFFSET（向上）
    int offset = -static_cast<int>(4.0f * GameConfig::ROLL_ARC_MAX_OFFSET
                                   * (progress - 0.5f) * (progress - 0.5f)
                                   + 0.5f)
                 + GameConfig::ROLL_ARC_MAX_OFFSET;

    return -offset;  // 负值表示向上偏移
}

bool Player::minish1FirstLoop() const
{
    return m_minish1FirstLoop;
}

int Player::shrinkFrame() const
{
    return m_shrinkFrame;
}

int Player::shrinkTotalFrames() const
{
    return GameConfig::SHRINK_ANIMATION_FRAMES;
}

void Player::updateAnimation()
{
    // 走向树桩中心：滚上树桩，使用翻滚动作表
    if (m_sizeState == Approaching) {
        int targetX = GameConfig::SHRINK_TARGET_X;
        int targetY = GameConfig::SHRINK_TARGET_Y;

        int dx = targetX - m_x;
        int dy = targetY - m_y;

        if (dx == 0 && dy == 0) {
            // 到达目标点，进入 NearStump 等待状态
            m_sizeState = NearStump;
            m_actionLocked = false;
            m_isMoving = false;
            m_minish1FirstLoop = true;
            setAction(Minish1);
            return;
        }

        // 根据剩余动画帧计算步长，使一轮动画恰好走完
        int totalAnimFrames = currentActionFrameCount() * GameConfig::ROLL_ANIMATION_SPEED;
        int remainingAnimFrames = (currentActionFrameCount() - m_animationFrame - 1)
                                      * GameConfig::ROLL_ANIMATION_SPEED
                                  + (GameConfig::ROLL_ANIMATION_SPEED - m_animationCounter);
        if (remainingAnimFrames <= 0) remainingAnimFrames = 1;

        float stepXf = static_cast<float>(dx) / remainingAnimFrames;
        float stepYf = static_cast<float>(dy) / remainingAnimFrames;

        m_x += qRound(stepXf);
        m_y += qRound(stepYf);

        // 防止越过目标点
        if ((targetX - m_x) * dx < 0) m_x = targetX;
        if ((targetY - m_y) * dy < 0) m_y = targetY;

        // 播放翻滚动画帧
        m_animationCounter++;
        if (m_animationCounter >= GameConfig::ROLL_ANIMATION_SPEED) {
            m_animationCounter = 0;
            m_animationFrame++;
            int maxFrame = currentActionFrameCount();
            if (m_animationFrame >= maxFrame) {
                m_animationFrame = 0;
            }
        }

        return;
    }

    // 平滑移出树桩区域：滚下树桩，使用翻滚动作表
    if (m_sizeState == LeavingStump) {
        int dx = m_leaveTargetX - m_x;
        int dy = m_leaveTargetY - m_y;

        if (dx == 0 && dy == 0) {
            // 到达离开目标点，恢复自由状态
            m_sizeState = m_approachToMini ? Normal : Mini;
            m_actionLocked = false;
            m_isMoving = false;
            setAction(Idle);
            return;
        }

        // 根据剩余动画帧计算步长，使一轮动画恰好走完
        int totalAnimFrames = currentActionFrameCount() * GameConfig::ROLL_ANIMATION_SPEED;
        int remainingAnimFrames = (currentActionFrameCount() - m_animationFrame - 1)
                                      * GameConfig::ROLL_ANIMATION_SPEED
                                  + (GameConfig::ROLL_ANIMATION_SPEED - m_animationCounter);
        if (remainingAnimFrames <= 0) remainingAnimFrames = 1;

        float stepXf = static_cast<float>(dx) / remainingAnimFrames;
        float stepYf = static_cast<float>(dy) / remainingAnimFrames;

        m_x += qRound(stepXf);
        m_y += qRound(stepYf);

        // 防止越过目标点
        if ((m_leaveTargetX - m_x) * dx < 0) m_x = m_leaveTargetX;
        if ((m_leaveTargetY - m_y) * dy < 0) m_y = m_leaveTargetY;

        // 播放翻滚动画帧
        m_animationCounter++;
        if (m_animationCounter >= GameConfig::ROLL_ANIMATION_SPEED) {
            m_animationCounter = 0;
            m_animationFrame++;
            int maxFrame = currentActionFrameCount();
            if (m_animationFrame >= maxFrame) {
                m_animationFrame = 0;
            }
        }

        return;
    }

    // 缩小/放大动画处理
    if (m_sizeState == Shrinking || m_sizeState == Growing) {
        m_shrinkFrame++;
        if (m_shrinkFrame >= GameConfig::SHRINK_ANIMATION_FRAMES) {
            if (m_sizeState == Shrinking) {
                m_sizeState = Mini;
            } else {
                m_sizeState = Normal;
            }
            m_actionLocked = false;
            m_shrinkFrame = 0;
        }
        return;
    }

    m_animationCounter++;

    int speed = GameConfig::ANIMATION_SPEED;
    int maxFrameCount = currentActionFrameCount();

    if (m_action == Roll) {
        speed = GameConfig::ROLL_ANIMATION_SPEED;
    } else if (m_action == Minish1) {
        speed = GameConfig::ANIMATION_SPEED;
    }

    if (m_animationCounter < speed) {
        if (m_rollCooldown > 0) m_rollCooldown--;
        return;
    }

    m_animationCounter = 0;
    m_animationFrame++;

    if (m_rollCooldown > 0) m_rollCooldown--;

    if (m_action == Minish1) {
        if (m_minish1FirstLoop) {
            // 第一遍：播放全部10帧
            if (m_animationFrame >= GameConfig::MINISH1_FRAME_COUNT) {
                m_minish1FirstLoop = false;
                m_animationFrame = GameConfig::MINISH1_LOOP_START;
            }
        } else {
            // 之后：循环第3-8帧
            if (m_animationFrame > GameConfig::MINISH1_LOOP_END) {
                m_animationFrame = GameConfig::MINISH1_LOOP_START;
            }
        }
    } else if (m_animationFrame >= maxFrameCount) {
        m_animationFrame = 0;

        if (m_action == Roll) {
            m_actionLocked = false;
            m_rollCooldown = 4;
            setAction(Idle);
        }
    }
}

int Player::actionRow() const
{
    int directionIndex = 0;

    if (m_direction == Down) {
        directionIndex = 0;
    } else if (m_direction == Left) {
        directionIndex = 1;
    } else if (m_direction == Right) {
        directionIndex = 2;
    } else if (m_direction == Up) {
        directionIndex = 3;
    }

    if (m_action == Idle) {
        return 0 + directionIndex;
    }

    if (m_action == Walk) {
        return 4 + directionIndex;
    }

    if (m_action == Minish1) {
        return directionIndex;
    }

    return directionIndex;
}

QRect Player::rect() const
{
    return QRect(m_x,
                 m_y,
                 bodyWidth(),
                 bodyHeight());
}

int Player::currentActionFrameCount() const
{
    if (m_action == Minish1) {
        return GameConfig::MINISH1_FRAME_COUNT;
    }

    if (m_action == Roll) {
        if (m_direction == Left || m_direction == Right) {
            return isMini() ? GameConfig::MINI_ROLL_SIDE_FRAME_COUNT
                            : GameConfig::ROLL_SIDE_FRAME_COUNT;
        }

        return isMini() ? GameConfig::MINI_ROLL_FRAME_COUNT
                        : GameConfig::ROLL_FRAME_COUNT;
    }

    return isMini() ? GameConfig::MINI_FRAME_COUNT
                    : GameConfig::PLAYER_FRAME_COUNT;
}
