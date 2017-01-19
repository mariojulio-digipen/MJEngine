#pragma once

#include "ClothComponent.h"
#include "FollowMeComponent.h"
#include "MoveAlongPathComponent.h"
#include "SplinePathGeneratorComponent.h"
#include "RayPlaneIntersectEvaluatorComponent.h"
#include "UIUpdaterComponent.h"
#include "LocalLightMoverComponent.h"
#include "KeyFrameAnimationComponent.h"
#include "LineComponent.h"
#include "Move3DComponent.h"
#include "LightPointComponent.h"
#include "SpawnerComponent.h"
#include "TransformComponent.h"
#include "DummyRotationComponent.h"
#include "UILightController.h"
#include "Move2DComponent.h"
#include "BodyComponent.h"

template <class T>
void assembleComponent(GameObject* go)
{
	T* component = new T;
	go->AddComponent(component);
}

void attachComponents(std::string componentName, GameObject* go)
{
	if (componentName == "DummyRotationComponent")
		assembleComponent<DummyRotationComponent>(go);
	if (componentName == "PainterComponent")
		assembleComponent<PainterComponent>(go);
	if (componentName == "UILightController")
		assembleComponent<UILightController>(go);
	if (componentName == "Move2DComponent")
		assembleComponent<Move2DComponent>(go);
	if (componentName == "BodyComponent")
		assembleComponent<BodyComponent>(go);
	if (componentName == "SpawnerComponent")
		assembleComponent<SpawnerComponent>(go);
	if (componentName == "LightPointComponent")
		assembleComponent<LightPointComponent>(go);
	if (componentName == "Move3DComponent")
		assembleComponent<Move3DComponent>(go);
	if (componentName == "LineComponent")
		assembleComponent<LineComponent>(go);
	if (componentName == "KeyFrameAnimationComponent")
		assembleComponent<KeyFrameAnimationComponent>(go);
	if (componentName == "LocalLightMoverComponent")
		assembleComponent<LocalLightMoverComponent>(go);
	if (componentName == "UIUpdaterComponent")
		assembleComponent<UIUpdaterComponent>(go);
	if (componentName == "RayPlaneIntersectEvaluatorComponent")
		assembleComponent<RayPlaneIntersectEvaluatorComponent>(go);
	if (componentName == "SplinePathGeneratorComponent")
		assembleComponent<SplinePathGeneratorComponent>(go);
	if (componentName == "MoveAlongPathComponent")
		assembleComponent<MoveAlongPathComponent>(go);
	if (componentName == "FollowMeComponent")
		assembleComponent<FollowMeComponent>(go);
	if (componentName == "ClothComponent")
		assembleComponent<ClothComponent>(go);
}
