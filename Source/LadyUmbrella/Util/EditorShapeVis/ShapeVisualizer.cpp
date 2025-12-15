#include "ShapeVisualizer.h"
#include "CustomDebugRenderProxy.h"
#include "ShapeInformation.h"

UShapeVisualizer::UShapeVisualizer(const FObjectInitializer& ObjectInitializer) : UPrimitiveComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCastShadow(false);
#if WITH_EDITORONLY_DATA
	SetIsVisualizationComponent(!bShowInGame);
#endif
	SetHiddenInGame(!bShowInGame);
	bIsEditorOnly = !bShowInGame;
	bVisibleInReflectionCaptures = false;
	bVisibleInRayTracing = false;
	bVisibleInRealTimeSkyCaptures = false;
	AlwaysLoadOnClient = false;
}

#if WITH_EDITOR
void UShapeVisualizer::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	/*
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetNameCPP() == "bShowInGame")
	{
		SetIsVisualizationComponent(!bShowInGame);
		MarkRenderStateDirty();
	}
	*/
}
#endif
FPrimitiveSceneProxy* UShapeVisualizer::CreateSceneProxy()
{
	FCustomDebugRenderSceneProxy* RetShape = new FCustomDebugRenderSceneProxy(this);
	
	if (Shape.ShapeToShow == EShapeToVis::LINE)
	{
		FTransform CompTransform = GetComponentTransform();
		RetShape->Lines.Add(FCustomDebugRenderSceneProxy::FDebugLine(
			CompTransform.TransformPosition(Shape.Origin),
			CompTransform.TransformPosition(Shape.End),
			Shape.Color,
			5.f));
	}

	if (Shape.ShapeToShow == EShapeToVis::BOX)
	{
		FVector HalfSize = Shape.Size * 0.5f;
		FBox BoundBox(-HalfSize, HalfSize);
		FTransform CombinedTransform = FTransform(Shape.Rotation, Shape.Origin) * GetComponentTransform();
		RetShape->Boxes.Add(FCustomDebugRenderSceneProxy::FDebugBox(
			BoundBox,
			Shape.Color,
			CombinedTransform));
	}

	if (Shape.ShapeToShow == EShapeToVis::SPHERE)
	{
		FTransform CompTransform = GetComponentTransform();
		RetShape->Spheres.Add(FCustomDebugRenderSceneProxy::FSphere(
			CompTransform.GetMaximumAxisScale() * Shape.Radius,
			CompTransform.TransformPosition(Shape.Origin),
			Shape.Color));
	}
	
	return RetShape;
}

FBoxSphereBounds UShapeVisualizer::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds BoundsBS = Super::CalcBounds(FTransform::Identity); // Local Space
	
	if (Shape.ShapeToShow == EShapeToVis::LINE)
	{
		FVector BoxExtent = Shape.Origin.GetAbs().ComponentMax(Shape.End.GetAbs());
		BoundsBS = BoundsBS + FBoxSphereBounds(FVector::ZeroVector, BoxExtent, BoxExtent.GetMax());
	}

	if (Shape.ShapeToShow == EShapeToVis::BOX)
	{
		FVector HalfSize = Shape.Size * 0.5f;
		FBox BoundBox(-HalfSize, HalfSize);
		FTransform BoxTransform = FTransform(Shape.Rotation, Shape.Origin);
		BoundBox = BoundBox.TransformBy(BoxTransform);
		BoundsBS = BoundsBS + FBoxSphereBounds(BoundBox);
	}

	if (Shape.ShapeToShow == EShapeToVis::SPHERE)
	{
		BoundsBS = BoundsBS + FBoxSphereBounds(Shape.Origin, FVector(Shape.Radius), Shape.Radius);
	}
	
	return BoundsBS.TransformBy(LocalToWorld);
}
