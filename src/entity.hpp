#pragma once

#include "types.hpp"

class Entity
{
    public:
        Entity()
            : m_x (0)
            , m_y (72)
            , m_z (0)
        {}

        AbsLoc getX(void) const { return m_x; }
        AbsLoc getY(void) const { return m_y; }
        AbsLoc getZ(void) const { return m_z; }

        void setX(AbsLoc x) { m_x = x; }
        void setY(AbsLoc y) { m_y = y; }
        void setZ(AbsLoc z) { m_z = z; }

        virtual ~Entity() {}
    private:
        AbsLoc m_x;
        AbsLoc m_y;
        AbsLoc m_z;
};
