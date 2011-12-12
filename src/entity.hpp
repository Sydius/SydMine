#pragma once

#include "types.hpp"

class Entity
{
    public:
        Entity()
            : m_eid(0)
            , m_x(0)
            , m_lastX(0)
            , m_y(72)
            , m_lastY(72)
            , m_z(0)
            , m_lastZ(72)
            , m_yaw(0.0f)
            , m_lastYaw(0.0f)
            , m_pitch(0.0f)
            , m_lastPitch(0.0f)
            , m_ticks(0)
        {}

        void setEID(EID eid) { m_eid = eid; }
        EID getEID(void) const { return m_eid; }

        AbsLoc getX(void) const { return m_x; }
        AbsLoc getY(void) const { return m_y; }
        AbsLoc getZ(void) const { return m_z; }
        AbsLoc getLastX(void) const { return m_lastX; }
        AbsLoc getLastY(void) const { return m_lastY; }
        AbsLoc getLastZ(void) const { return m_lastZ; }
        Rot getYaw(void) const { return m_yaw; }
        Rot getLastYaw(void) const { return m_lastYaw; }
        Rot getPitch(void) const { return m_pitch; }
        Rot getLastPitch(void) const { return m_lastPitch; }

        void setX(AbsLoc x) { m_x = x; }
        void setY(AbsLoc y) { m_y = y; }
        void setZ(AbsLoc z) { m_z = z; }
        void setYaw(Rot r) { m_yaw = r; }
        void setPitch(Rot r) { m_pitch = r; }
        
        void tickPosition(void)
        {
            m_lastX = m_x;
            m_lastY = m_y;
            m_lastZ = m_z;
            m_lastYaw = m_yaw;
            m_lastPitch = m_pitch;
            m_ticks++;
        }

        bool forceTeleport(void) const { return !(m_ticks % 5); }

        virtual ~Entity() {}
    private:
        EID m_eid;

        AbsLoc m_x;
        AbsLoc m_lastX;
        AbsLoc m_y;
        AbsLoc m_lastY;
        AbsLoc m_z;
        AbsLoc m_lastZ;

        Rot m_yaw;
        Rot m_lastYaw;
        Rot m_pitch;
        Rot m_lastPitch;
        
        int m_ticks;
};
