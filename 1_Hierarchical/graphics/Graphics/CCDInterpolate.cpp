//// CCDInterpolate.CPP
//// Implementation autogenerated by MJEngine automation tools
//// WARNING: Do not modify autogenerated comments, automation 
//// tools depend on them.
//
//// Edition author: mzaldivar
//
//#include "CCDInterpolate.h"
//#include "GameObject.h"
//#include "GLObject.h"
//#include "FollowMeComponent.h"
//#include "Transform.h"
//#include <sstream>
//
//CCDInterpolate::CCDInterpolate()
//{
//}
//
//
//CCDInterpolate::~CCDInterpolate()
//{
//}
//
//
//bool CCDInterpolate::Init()
//{
//	// TODO - Implement initializations here
//	// This function runs once when object manager
//	// load objects, before serialization
//	ccd_solver = GetOwner()->FindComponent<FollowMeComponent>();
//	i = 0;
//	t = 0.0;
//
//	//ownerTrans = GetOwner()->FindComponent<TransformComponent>();
//	return true;
//}
//
//
//void CCDInterpolate::Update()
//{
//	// TODO - Implement all the updates here
//	// This function is executed once everyframe
//	//ccd_solver->FramesVQSs[0].
//	t += 0.1;
//	i++;
//	Quaternion from_q0 = ccd_solver->FramesRotators[i].thisFrame;
//	Quaternion to_q1 = ccd_solver->FramesRotators[i].nextFrame;
//	Quaternion newquat = Quaternion::Slerp(from_q0, to_q1, t);
//
//
//	
//	GetOwner()->GetGLObject()->Transform->VQS_ModelToWorld = vqs_interpolated;
//	
//	if (i >= ccd_solver->FramesRotators.size())
//	{
//
//	}
//}
//
//
//void CCDInterpolate::HandleEvent(const Event* const evt)
//{
//	// TODO - Implement reactions to different events
//}
//