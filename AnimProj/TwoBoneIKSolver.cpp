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

    float CABadjusted = (bc * bc - ab * ab - at * at) / (-2 * ab * at);
    CABadjusted = std::acosf(CABadjusted);

    float ABCadjusted = (at * at - ab * ab - bc * bc) / (-2 * ab * bc);
    ABCadjusted = std::acosf(ABCadjusted);

    XMVECTOR AxisCAB = XMVector3Normalize(XMVector3Cross(AB, AC));
    XMVECTOR AxisTAC = XMVector3Normalize(XMVector3Cross(AC, AT));

    bool isAxisCAB = true;
    bool isAxisTAC = true;
    if (XMVectorGetX(XMVectorSum(AxisCAB)) == 0)
    {
        AxisCAB = AxisTAC;
        isAxisCAB = false;
    }

    if (XMVectorGetX(XMVectorSum(AxisTAC)) == 0)
    {
        AxisTAC = AxisCAB;
        isAxisTAC = false;
    }

    if (!isAxisCAB && !isAxisTAC)
        return false;


    const XMVECTOR Axis0 = XMVector3InverseRotate(AxisCAB * -1.0f, RotAGlobal);
    const XMVECTOR Axis1 = XMVector3InverseRotate(AxisCAB, RotBGlobal);

    const XMVECTOR Q0 = XMQuaternionRotationAxis(Axis0, CABadjusted - CAB);
    const XMVECTOR Q1 = XMQuaternionRotationAxis(Axis1, ABCadjusted - ABC);

    const XMVECTOR Axis2 = XMVector3InverseRotate(AxisTAC, RotAGlobal);
    const XMVECTOR Q2 = XMQuaternionRotationAxis(Axis2, TAC);

    XMStoreFloat4(&Qa, XMQuaternionMultiply(Q0, Q2));
    XMStoreFloat4(&Qb, Q1);


    return true;
}
