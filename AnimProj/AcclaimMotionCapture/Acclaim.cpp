#include "pch.h"
#include "Acclaim.h"

pa::Acclaim::DOF pa::Acclaim::convertToDOF(std::string const& s)
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

pa::Acclaim::AxisOrder pa::Acclaim::convertToAxisOrder(std::string const& s)
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

DirectX::XMVECTOR pa::Acclaim::getQuaternionFromAxis(DirectX::XMVECTOR axis, AxisOrder order)
{
    using namespace DirectX;

    const XMMATRIX rotationX = XMMatrixRotationX(XMVectorGetX(axis));
    const XMMATRIX rotationY = XMMatrixRotationY(XMVectorGetY(axis));
    const XMMATRIX rotationZ = XMMatrixRotationZ(XMVectorGetZ(axis));

    XMMATRIX M = {};

    switch (order)
    {
    case AxisOrder::XYZ:
        M = rotationX * rotationY * rotationZ;
        break;

    case AxisOrder::XZY:
        M = rotationX * rotationZ * rotationY;
        break;

    case AxisOrder::YZX:
        M = rotationY * rotationZ * rotationX;
        break;

    case AxisOrder::YXZ:
        M = rotationY * rotationX * rotationZ;
        break;

    case AxisOrder::ZXY:
        M = rotationZ * rotationX * rotationY;
        break;

    case AxisOrder::ZYX:
        M = rotationZ * rotationY * rotationX;
        break;
    default:
        DebugBreak();
    }

    XMVECTOR Q = XMQuaternionRotationMatrix(M);
    Q = XMQuaternionNormalizeEst(Q);

    return Q;
}
