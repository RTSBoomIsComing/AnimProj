#include "pch.h"
#include "Acclaim.h"

pa::Acclaim::DOF pa::Acclaim::convertStringToDOF(std::string const& s)
{
    if ("TX" == s || "tx" == s)
        return DOF::TX;

    if ("TY" == s || "ty" == s)
        return DOF::TY;

    if ("TZ" == s || "tz" == s)
        return DOF::TZ;

    if ("RX" == s || "rx" == s)
        return DOF::RX;

    if ("RY" == s || "ry" == s)
        return DOF::RY;
    
    if ("RZ" == s || "rz" == s)
        return DOF::RZ;

    if ("L" == s || "l" == s)
        return DOF::L;

    return DOF::None;
}

pa::Acclaim::AxisOrder pa::Acclaim::convertStringToAxisOrder(std::string const& s)
{
    if ("XYZ" == s)
        return AxisOrder::XYZ;

    if ("XZY" == s)
        return AxisOrder::XZY;

    if ("YXZ" == s)
        return AxisOrder::YXZ;

    if ("YZX" == s)
        return AxisOrder::YZX;

    if ("ZXY" == s)
        return AxisOrder::ZXY;

    if ("ZYX" == s)
        return AxisOrder::ZYX;

    DebugBreak();
    return Acclaim::AxisOrder();
}

DirectX::XMMATRIX pa::Acclaim::getRotationFromAxis(DirectX::XMFLOAT3 axis, AxisOrder order, float unit)
{
    using namespace DirectX;
    // Use row major matrix
    const XMMATRIX rotationX = XMMatrixRotationX(axis.x);
    const XMMATRIX rotationY = XMMatrixRotationY(axis.y);
    const XMMATRIX rotationZ = XMMatrixRotationZ(axis.z);

    switch (order)
    {
    case AxisOrder::XYZ:
        return rotationX * rotationY * rotationZ;

    case AxisOrder::XZY:
        return rotationX * rotationZ * rotationY;

    case AxisOrder::YZX:
        return rotationY * rotationZ * rotationX;

    case AxisOrder::YXZ:
        return rotationY * rotationX * rotationZ;

    case AxisOrder::ZXY:
        return rotationZ * rotationX * rotationY;

    case AxisOrder::ZYX:
        return rotationZ * rotationY * rotationX;
    }

    DebugBreak();
    return DirectX::XMMATRIX();
}
