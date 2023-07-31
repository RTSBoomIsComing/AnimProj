#include "pch.h"
#include "TwoBoneIKSolver.h"

bool pa::TwoBoneIKSolver::solve(DirectX::XMVECTOR A, DirectX::XMVECTOR B, DirectX::XMVECTOR C, DirectX::XMVECTOR Target, DirectX::XMVECTOR RotAGlobal, DirectX::XMVECTOR RotBGlobal)
{
    using namespace DirectX;

    XMVECTOR AB = B - A;
    XMVECTOR AC = C - A;
    XMVECTOR BC = C - B;
    XMVECTOR AT = Target - A;

    const float ab = XMVectorGetX(XMVector3Length(AB));
    const float bc = XMVectorGetX(XMVector3Length(BC));
    
    float at = XMVectorGetX(XMVector3Length(AT));
    at = std::min(at, ab + bc - 0.0001f);
    at = std::max(at, 0.0001f);

    AB = XMVector3Normalize(AB);
    AC = XMVector3Normalize(AC);
    BC = XMVector3Normalize(BC);
    AT = XMVector3Normalize(AT);

    const float CAB = std::acosf(XMVectorGetX(XMVector3Dot(AB, AC)));
    const float ABC = std::acosf(XMVectorGetX(XMVector3Dot(BC, -AB)));
    const float TAC = std::acosf(XMVectorGetX(XMVector3Dot(AC, AT)));

    float CABnew = (bc * bc - ab * ab - at * at) / (-2 * ab * at);
    CABnew = std::acosf(CABnew);

    float ABCnew = (at * at - ab * ab - bc * bc) / (-2 * ab * bc);
    ABCnew = std::acosf(ABCnew);

    XMVECTOR AxisABAC = XMVector3Normalize(XMVector3Cross(AB, AC));
    XMVECTOR AxisACAT = XMVector3Normalize(XMVector3Cross(AC, AT));

    bool isAxisABAC = true;
    bool isAxisACAT = true;
    if (XMVectorGetX(XMVectorSum(AxisABAC)) == 0)
    {
        AxisABAC = AxisACAT;
        isAxisABAC = false;
    }

    if (XMVectorGetX(XMVectorSum(AxisACAT)) == 0)
    {
        AxisACAT = AxisABAC;
        isAxisACAT = false;
    }

    if (!isAxisABAC && !isAxisACAT)
        return false;


    const XMVECTOR Axis0 = XMVector3InverseRotate(AxisABAC, RotAGlobal);
    const XMVECTOR Axis1 = XMVector3InverseRotate(AxisABAC, RotBGlobal);

    const XMVECTOR Q0 = XMQuaternionRotationAxis(Axis0, CAB - CABnew);
    const XMVECTOR Q1 = XMQuaternionRotationAxis(Axis1, ABC - ABCnew);

    const XMVECTOR Axis2 = XMVector3InverseRotate(AxisACAT, RotAGlobal);
    const XMVECTOR Q2 = XMQuaternionRotationAxis(Axis2, TAC);

    XMStoreFloat4(&Qa, XMQuaternionMultiply(Q0, Q2));
    XMStoreFloat4(&Qb, Q1);


    return true;
}
