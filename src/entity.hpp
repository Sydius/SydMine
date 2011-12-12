#pragma once

#include "types.hpp"

class Entity
{
    public:
        Entity()
            : m_x (0)
            , m_y (72)
            , m_z (0)
            , m_yaw (0.0f)
            , m_pitch (0.0f)
        {}

        AbsLoc getX(void) const { return m_x; }
        AbsLoc getY(void) const { return m_y; }
        AbsLoc getZ(void) const { return m_z; }
        Rot getYaw(void) const { return m_yaw; }
        Rot getPitch(void) const { return m_pitch; }

        void setX(AbsLoc x) { m_x = x; }
        void setY(AbsLoc y) { m_y = y; }
        void setZ(AbsLoc z) { m_z = z; }
        void setYaw(Rot r) { m_yaw = r; }
        void setPitch(Rot r) { m_pitch = r; }

        virtual ~Entity() {}
    private:
        AbsLoc m_x;
        AbsLoc m_y;
        AbsLoc m_z;

        Rot m_yaw;
        Rot m_pitch;
};
