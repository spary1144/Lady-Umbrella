#pragma once
#include "CoreMinimal.h"
#include "DebugRenderSceneProxy.h"

class FCustomDebugRenderSceneProxy : public FDebugRenderSceneProxy
{
	
public:
	
	FCustomDebugRenderSceneProxy(const UPrimitiveComponent* InComponent)
	: FDebugRenderSceneProxy(InComponent) {}
	
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
};

