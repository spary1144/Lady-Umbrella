// Fill out your copyright notice in the Description page of Project Settings.


#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "../../Characters/GenericCharacter.h"
#include "AICoverEntryPoint.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UCoverSpline::UCoverSpline()
{
	// Initializing variables
	bIsLooped = false;
	bIsDisabled = false;
	bIsInverted = false;
	bIsSinglePointCover = false;
	NumberOfIaEntryPoints = 0;

	bNeedsOnConstructReset = false;
	bNeedsOnConstructUpdate = true;

	// Rest of constructor
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetHiddenInGame(true);
	SetVisibility(true);
	SetMobility(EComponentMobility::Type::Stationary);
	
	InstancedStaticMesh = CreateEditorOnlyDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_EntryPointWidgets"));
	if (InstancedStaticMesh)
	{
		InstancedStaticMesh->SetupAttachment(this);
		InstancedStaticMesh->SetMobility(EComponentMobility::Stationary);
		InstancedStaticMesh->bDisableCollision = true;
		InstancedStaticMesh->SetHiddenInGame(true);
		InstancedStaticMesh->SetVisibility(false);

		static ConstructorHelpers::FObjectFinder<UStaticMesh> ArrowStaticMeshRef (TEXT("/Game/Meshes/StaticMeshes/EditorOnly/CoverSystem/SM_CoverEntryWidget.SM_CoverEntryWidget"));
		if (ArrowStaticMeshRef.Object != nullptr)
		{
			InstancedStaticMesh->SetStaticMesh(ArrowStaticMeshRef.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterialInstance> ArrowMat0Ref (TEXT("/Game/Materials/Instancias/Editor/Covers/MI_CoverWidget3.MI_CoverWidget3"));
		if (ArrowMat0Ref.Object != nullptr)
		{
			InstancedStaticMesh->SetMaterial(0, ArrowMat0Ref.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterialInstance> ArrowMat1Ref (TEXT("/Game/Materials/Instancias/Editor/Covers/MI_CoverWidget1.MI_CoverWidget1"));
		if (ArrowMat1Ref.Object != nullptr)
		{
			InstancedStaticMesh->SetMaterial(1, ArrowMat1Ref.Object);
		}
	}
}

#pragma region EDITOR ONLY FUNCTIONS

void UCoverSpline::OrderResetSplinePoints()
{
#if UE_EDITOR
	if (Cast<ACoverParent>(GetOwner()) == nullptr)
	{
		FLogger::ErrorLog("Cannot cast owner of CoverSpline to ACoverParent. CoverSplines can only be attached to a ACoverParent actor.");
	}
#endif

	bNeedsOnConstructReset = true;
	bNeedsOnConstructUpdate = true;

#if WITH_EDITOR
	if (auto* CoverParent = TryGetParent())
	{
		CoverParent->RerunConstructionScripts();
	}
#endif
	
}

void UCoverSpline::OrderUpdateSplinePoints()
{
#if UE_EDITOR
	if (Cast<ACoverParent>(GetOwner()) == nullptr)
	{
		FLogger::ErrorLog("Cannot cast owner of CoverSpline to ACoverParent. CoverSplines can only be attached to a ACoverParent actor.");
	}
#endif

	bNeedsOnConstructUpdate = true;
	
#if WITH_EDITOR
	if (auto* CoverParent = TryGetParent())
	{
		CoverParent->RerunConstructionScripts();
	}
#endif
}

void UCoverSpline::ShowPlayerEntryPoints()
{
#if UE_EDITOR
	if (IsValid(InstancedStaticMesh))
	{
		InstancedStaticMesh->SetVisibility(true);
	}
#endif
}

void UCoverSpline::HidePlayerEntryPoints()
{
#if UE_EDITOR
	if (IsValid(InstancedStaticMesh))
	{
		InstancedStaticMesh->SetVisibility(false);
	}
#endif
}

void UCoverSpline::ResetCoverSpline()
{
	SetClosedLoop(bIsLooped);

	if (bIsLooped && bIsSinglePointCover)
	{
		bIsLooped = false;
		// FLogger::WarningLog("A cover cannot be Looped and SinglePointCover at the same time. Making the cover not Looped.");
	}

#if UE_EDITOR
	UpdateSplineTrackPoints();
	ResetNumberOfIAEntryPoints();
	DestroyAllAttachedIaEntryPoints();
	EnsureSpawnOfIaEntryPoints();
	ResetSectionsOfIaEntryPoints();
#endif

	bNeedsOnConstructReset = false;
	bNeedsOnConstructUpdate	= true;
}

void UCoverSpline::UpdateCoverSpline()
{
	if (!IsValid(Cast<ACoverParent>(GetOwner())))
	{
#if UE_EDITOR
		FLogger::ErrorLog("Cannot cast owner of CoverSpline to ACoverParent. CoverSplines can only be attached to a ACoverParent actor.");
#endif

		return;
	}

	if (IsClosedLoop() != bIsLooped)
	{
		SetClosedLoop(bIsLooped);
	}

	if (bIsLooped && bIsSinglePointCover)
	{
		bIsLooped = false;
		// FLogger::WarningLog("A cover cannot be Looped and SinglePointCover at the same time. Making the cover not Looped.");
	}

#if UE_EDITOR
	UpdateSplineTrackPoints();
	if (EnsureSpawnOfIaEntryPoints()) // If we had to change the number of EntryPoints, we reset their sections.
	{
		ResetSectionsOfIaEntryPoints();
	}
	CreatePlayerEntryPointsWidgets();
	PlaceAIEntryPoints();
	SortIAEntryPoints();
	UpdateReachableCovers();
#endif

	bNeedsOnConstructUpdate	= false;
}

void UCoverSpline::ResetIaEntryPointsOwningSplines()
{
	for (int Index = IaEntryPoints.Num() - 1; Index >= 0; Index--)
	{
		if (IsValid(IaEntryPoints[Index]))
		{
			IaEntryPoints[Index]->SetOwningSpline(this);
		}
		else
		{
			IaEntryPoints.RemoveAt(Index);
			bNeedsOnConstructUpdate = true;
		}
	}
}

static bool SortingCoverEntryPointsBySectionAlgo(const AAICoverEntryPoint& LeftPoint, const AAICoverEntryPoint& RightPoint)
{ return IsValid(&LeftPoint) ? ( IsValid(&RightPoint) ? (LeftPoint.GetSection() < RightPoint.GetSection()) : false) : true; }

void UCoverSpline::SortIAEntryPoints()
{
	IaEntryPoints.Sort(&SortingCoverEntryPointsBySectionAlgo);
}

#pragma endregion 

#pragma region PUBLIC FUNCTIONS

void UCoverSpline::CalculatePlayerEntryPoints(TArray<FVector>& BufferArrayOfEntryPoints, float& SectionLengthBuffer)
{
	// In case the buffers were not empty.
	BufferArrayOfEntryPoints.Empty();

	if (IsDisabled())
	{
		SectionLengthBuffer = 0;
		return;
	}

	unsigned int numberOfPlayerEntryPoints = CalculateNumberOfPlayerEntryPoints();

	if (bIsSinglePointCover)
	{
		BufferArrayOfEntryPoints.Emplace(GetLocationAtTime(0, ESplineCoordinateSpace::World, true));
		SectionLengthBuffer = 1;
		return;
	}
	
	double LengthOfSections = bIsLooped ? 1.0f / numberOfPlayerEntryPoints : 1.0f / (numberOfPlayerEntryPoints - 1);

	BufferArrayOfEntryPoints.Reserve(numberOfPlayerEntryPoints);
	
	for (unsigned int Index = 0; Index < numberOfPlayerEntryPoints; Index++)
	{
		BufferArrayOfEntryPoints.Emplace(GetLocationAtTime(Index * LengthOfSections, ESplineCoordinateSpace::World, true));
	}

	SectionLengthBuffer = LengthOfSections;
}

bool UCoverSpline::TestTallCoverAtTime(const float Time) const
{
	UWorld* World = GetWorld();
	if (World == nullptr || IsDisabled())
	{
		return false;
	}

	TArray<FHitResult> HitResults;

	const FVector TraceLineBegin = FindLocationAtTime(Time) + IS_TALL_CHECK_HEIGHT;
	const FVector TraceForwardVector = FindForwardRotationAtTime(Time).Vector() * IS_TALL_LENGTH_OF_TRACE_LINE;
	FCollisionQueryParams QueryParams = IS_TALL_COLLISION_QUERY_PARAMS;

	for (const AGenericCharacter* Character : CharactersUsingTheSpline)
	{
		QueryParams.AddIgnoredActor(Character);
	}
	
	if (World->LineTraceMultiByChannel(
	HitResults,
	TraceLineBegin,
	TraceLineBegin + TraceForwardVector,
	ECC_Visibility,
	QueryParams))
	{
		for (const FHitResult Hit : HitResults)
		{
			if (Hit.GetActor() == TryGetParent() || !Hit.GetActor()->ActorHasTag(ENVIRONMENT_NO_COVER_TAG))
			{
				return true;
			}
		}
	}

	return false;
}

bool UCoverSpline::TestTallCoverAtTimeWithHorizontalDisplacement(const float Time,
	const float HorizontalDisplacement) const
{
	UWorld* World = GetWorld();
	if (!IsValid(World) || IsDisabled())
	{
		return false;
	}

	TArray<FHitResult> HitResults;

	const FVector TraceForwardVector = FindForwardRotationAtTime(Time).Vector() * IS_TALL_LENGTH_OF_TRACE_LINE;
	const FVector TraceRightVector = FindRightRotationAtTime(Time).Vector();
	const FVector TraceLineBegin = FindLocationAtTime(Time) + IS_TALL_CHECK_HEIGHT + TraceRightVector * HorizontalDisplacement;
	FCollisionQueryParams QueryParams = IS_TALL_COLLISION_QUERY_PARAMS;

	for (const AGenericCharacter* Character : CharactersUsingTheSpline)
	{
		QueryParams.AddIgnoredActor(Character);
	}
	
	if (World->LineTraceMultiByChannel(
	HitResults,
	TraceLineBegin,
	TraceLineBegin + TraceForwardVector,
	ECC_Visibility,
	QueryParams))
	{
		for (const FHitResult Hit : HitResults)
		{
			if (Hit.GetActor() == TryGetParent() || !Hit.GetActor()->ActorHasTag(ENVIRONMENT_NO_COVER_TAG))
			{
				return true;
			}
		}
	}

	return false;
}

void UCoverSpline::DestroyCover()
{
	// Notifying actors that were using the spline.
	for (int i = CharactersUsingTheSpline.Num() - 1; i >= 0; i--)
	{
		if (IsValid(CharactersUsingTheSpline[i]))
		{
			CharactersUsingTheSpline[i]->GetCoverMovementComponent()->NotifyExitCover();
		}
	}

	DestroyAllAttachedIaEntryPoints();

	// Self-Destroying.
	DestroyComponent();
}

ACoverParent* UCoverSpline::TryGetParent() const
{
	return Cast<ACoverParent>(GetOwner());
}

int UCoverSpline::GetZone() const
{
	ACoverParent* CoverParent = TryGetParent();
	
	if (!IsDisabled() && IsValid(CoverParent))
	{
		return CoverParent->GetZone();
	}

	return -1;
}

#pragma endregion

#pragma region PRIVATE FUNCTIONS

void UCoverSpline::UpdateSplineTrackPoints()
{
	const unsigned int NumberOfSplinePoints = GetNumberOfSplinePoints();
	
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	for (unsigned int Point = 0; Point < NumberOfSplinePoints; Point++)
	{
		// Getting the location of the point															Adding a vertical offset to detect increased terrain height.
		const FVector OriginLocation = GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World) + FVector(0, 0, SPLINE_TRACK_LINE_TRACE_HEIGHT_OFFSET);

		// Doing a LineTrace to find the floor.
		FHitResult HitResult;
		if (World->LineTraceSingleByProfile
			(
				HitResult,
				OriginLocation,
				OriginLocation + FVector(0, 0, -50),
				FName(TEXT("BlockAll"))
			))
		{
			SetLocationAtSplinePoint(Point, HitResult.Location, ESplineCoordinateSpace::World);
		}
		// If we don't find the floor, we do nothing.
	}
}

AAICoverEntryPoint* UCoverSpline::GetMoveBetweenCoversPoint(const bool bGetPoint0, const bool bGetContinuousPoint) const
{
	if (bGetPoint0)
	{
		return bGetContinuousPoint ? GetReachablePointContinuous_0() : GetReachablePointForward_0();
	}

	// Getting Point 1.
	return bGetContinuousPoint ? GetReachablePointContinuous_1() : GetReachablePointForward_1();
}

void UCoverSpline::ResetNumberOfIAEntryPoints()
{
	if (IsDisabled())
	{
		NumberOfIaEntryPoints = 0;
		return;
	}
	
	if (bIsSinglePointCover)
	{
		NumberOfIaEntryPoints = 1;
		return;
	}

	if (TestTallCoverAtTime(0.0f) && TestTallCoverAtTime(1.0f))
	{
		NumberOfIaEntryPoints = 2;
		return;
	}
	
	NumberOfIaEntryPoints = GetSplineLength() / 50.0f;
	
	if (NumberOfIaEntryPoints < 1)
	{
		NumberOfIaEntryPoints = 1;
	}
}

void UCoverSpline::ResetSectionsOfIaEntryPoints()
{
	const double SectionLength = bIsSinglePointCover || IsDisabled() ? 0
	: bIsLooped ? (1.0f / NumberOfIaEntryPoints)
		: (1.0f / (NumberOfIaEntryPoints - 1));

	if (IsDisabled())
	{
		return;
	}

	for (int Index = IaEntryPoints.Num() - 1; Index >= 0; Index--)
	{
		if (IsValid(IaEntryPoints[Index]))
		{
			IaEntryPoints[Index]->SetSection(static_cast<double>(Index) * SectionLength);
		}
		else
		{
			IaEntryPoints.RemoveAt(Index);
		}
	}
}

unsigned int UCoverSpline::CalculateNumberOfPlayerEntryPoints() const
{
	if (IsDisabled())
	{
		return 0;
	}
	
	if (bIsSinglePointCover)
	{
		return 1;
	}
	
	const unsigned int NumberOfEntryPoints = GetSplineLength() / 22.0f;
	
	if (NumberOfEntryPoints < 1)
	{
		return 1;
	}

	return NumberOfEntryPoints;
}

void UCoverSpline::DestroyAllAttachedIaEntryPoints()
{
	for (int Index = IaEntryPoints.Num() - 1; Index >= 0; Index--)
	{
		if (IsValid(IaEntryPoints[Index]))
		{
			IaEntryPoints[Index]->Destroy();
		}
		
		IaEntryPoints.RemoveAt(Index);
	}
}

void UCoverSpline::PlaceAIEntryPoints()
{
	if (IsDisabled())
	{
		return;
	}
	
	// Placing the points.
	for (AAICoverEntryPoint* EntryPoint : IaEntryPoints)
	{
		if (!IsValid(EntryPoint))
		{
			continue;
		}
		
		EntryPoint->SetActorLocation(GetLocationAtTime(
			EntryPoint->GetSection(),
			ESplineCoordinateSpace::World,
			true)
			+ AI_ENTRYPOINT_ELEVATION);
		EntryPoint->SetActorRotation(FindForwardRotationAtTime(EntryPoint->GetSection()));
	}
}

void UCoverSpline::CreatePlayerEntryPointsWidgets()
{
	if (!IsValid(InstancedStaticMesh))
	{
		return;
	}

	InstancedStaticMesh->ClearInstances();

	if (IsDisabled()) // If we are disabled, we clean our widgets and not do anything else.
	{
		return;
	}

	TArray<FVector> EntryLocations;
	float SectionLength = 0;
	CalculatePlayerEntryPoints(EntryLocations, SectionLength);
	
	TArray<FTransform> InstancedTransforms;
	InstancedTransforms.Reserve(EntryLocations.Num());

	for (int Index = 0; Index < EntryLocations.Num(); Index++)
	{
		const FRotator Rotation = FindForwardRotationAtTime(SectionLength * Index);
		
		// Adding the Transform to our Array.
		InstancedTransforms.Emplace(
			FTransform(
				Rotation,
				EntryLocations[Index] + FVector(0,0,5),
				FVector(0.06f, 0.06f, 0.06f)));
	}

	InstancedStaticMesh->AddInstances(InstancedTransforms, false, true, false);
}

void UCoverSpline::OnRegister()
{
	Super::OnRegister();
	
	if (IsValid(InstancedStaticMesh))
	{
		InstancedStaticMesh->SetupAttachment(this->GetAttachmentRoot());
	}
}

bool UCoverSpline::IsPointReachableContinuous(bool FromPoint0, const AAICoverEntryPoint* Point) const
{
	if (!IsValid(Point) || IsDisabled())
	{
		return false;
	}

	const int8 OriginSection = (FromPoint0 || IsSinglePointCover()) ? 0 : 1;
	const FVector OriginPoint = FindLocationAtTime(OriginSection);
	const FVector VectorToPoint = Point->GetActorLocation() - AI_ENTRYPOINT_ELEVATION - OriginPoint;

	// If the other point is too far away.
	if (VectorToPoint.SquaredLength() > FMath::Square(MAX_DISTANCE_TO_CONTINUOUS_REACHABLE_POINT))
	{
		return false;
	}

	const FVector OtherForwardVector = Point->GetActorForwardVector();

	// If the other point is not aligned with ours.
	if (FVector::DotProduct(FindForwardRotationAtTime(OriginSection).Vector(), OtherForwardVector) < 0.85f)
	{
		return false;
	}

	const FVector ContinuousVector = (FromPoint0 ? FindLeftRotationAtTime(OriginSection) : FindRightRotationAtTime(OriginSection)).Vector();
	
	// If the other point is not in the direction our Spline goes into.
	if (FVector::DotProduct(ContinuousVector, VectorToPoint.GetSafeNormal()) < 0.98f)
	{
		return false;
	}

	return true;
}

bool UCoverSpline::IsPointReachableForward(bool FromPoint0, AAICoverEntryPoint* Point) const
{
	if (!IsValid(Point) || IsDisabled())
	{
		return false;
	}

	const UCoverSpline* CoverParent = Point->EnsureOwningSplineIsSet();

	// We do not want points that do not belong to the same cover.
	if (CoverParent == nullptr || CoverParent->TryGetParent() != TryGetParent())
	{
		return false;
	}

	const int8 OriginSection = (FromPoint0 || IsSinglePointCover()) ? 0 : 1;
	const FVector OriginPoint = FindLocationAtTime(OriginSection);
	const FVector VectorToPoint = Point->GetActorLocation() - AI_ENTRYPOINT_ELEVATION - OriginPoint;

	// If the other point is too far away.
	if (VectorToPoint.SquaredLength() > FMath::Square(MAX_DISTANCE_TO_FORWARD_REACHABLE_POINT))
	{
		return false;
	}

	const FVector OtherForwardVector = Point->GetActorForwardVector();

	// If the other point is not perpendicular with ours.
	if (abs(FVector::DotProduct(FindForwardRotationAtTime(OriginSection).Vector(), OtherForwardVector)) > MAX_FORWARD_REACHABLE_POINT_DOT_PRODUCT_ANGLE_DIFFERENCE)
	{
		return false;
	}

	const FVector ContinuousVector = (FromPoint0 ? FindLeftRotationAtTime(OriginSection) : FindRightRotationAtTime(OriginSection)).Vector();
	
	// If the other point is not in the direction our Spline goes into.
	if (FVector::DotProduct(ContinuousVector, OtherForwardVector) > (-1 + MAX_FORWARD_REACHABLE_POINT_DOT_PRODUCT_ANGLE_DIFFERENCE))
	{
		return false;
	}

	return true;
}

void UCoverSpline::UpdateReachableCovers(const UCoverSpline* CoverThatOrderedTheUpdate)
{
	const AActor* Parent = TryGetParent();
	const UWorld* World = GetWorld();
	if (!IsValid(Parent) || IaEntryPoints.Num() == 0 || IsDisabled() || !IsValid(IaEntryPoints[0])
		|| !IsValid(IaEntryPoints[IaEntryPoints.Num() - 1]) || World == nullptr)
	{
		ReachablePointForward_0 = nullptr;
		ReachablePointForward_1 = nullptr;
		ReachablePointContinuous_0 = nullptr;
		ReachablePointContinuous_1 = nullptr;
		return;
	}
	

	TArray<AAICoverEntryPoint*> PointsWhoseSplinesNeedUpdating;
	
	// Testing if our Forward Points broke.
	if (IsValid(ReachablePointForward_0))
	{
		if (!IsPointReachableForward(true, ReachablePointForward_0))
		{
			DrawDebugConnectionLine(World, true, ReachablePointForward_0, FColor::Red);

			PointsWhoseSplinesNeedUpdating.Add(ReachablePointForward_0);
			ReachablePointForward_0 = nullptr;
		}
		else
		{
			DrawDebugConnectionLine(World,true, ReachablePointForward_0, FColor::Yellow);
		}
	}
	if (IsValid(ReachablePointForward_1))
	{
		if (!IsPointReachableForward(false, ReachablePointForward_1))
		{
			DrawDebugConnectionLine(World,false, ReachablePointForward_1, FColor::Red);
			
			PointsWhoseSplinesNeedUpdating.Add(ReachablePointForward_1);
			ReachablePointForward_1 = nullptr;
		}
		else
		{
			DrawDebugConnectionLine(World,false, ReachablePointForward_1, FColor::Yellow);
		}
	}

	// Testing if our continuous points broke.
	if (IsValid(ReachablePointContinuous_0))
	{
		if (!IsPointReachableContinuous(true, ReachablePointContinuous_0))
		{
			DrawDebugConnectionLine(World,true, ReachablePointContinuous_0, FColor::Red);

			PointsWhoseSplinesNeedUpdating.Add(ReachablePointContinuous_0);
			ReachablePointContinuous_0 = nullptr;
		}
		else
		{
			DrawDebugConnectionLine(World,true, ReachablePointContinuous_0, FColor::Yellow);
		}
	}
	if (IsValid(ReachablePointContinuous_1))
	{
		if (!IsPointReachableContinuous(false, ReachablePointContinuous_1))
		{
			DrawDebugConnectionLine(World,false, ReachablePointContinuous_1, FColor::Red);

			PointsWhoseSplinesNeedUpdating.Add(ReachablePointContinuous_1);
			ReachablePointContinuous_1 = nullptr;
		}
		else
		{
			DrawDebugConnectionLine(World,false, ReachablePointContinuous_1, FColor::Yellow);
		}
	}

	
	if (IsValid(ReachablePointForward_0) && IsValid(ReachablePointForward_1) && IsValid(ReachablePointContinuous_0) && IsValid(ReachablePointContinuous_1))
	{
		return;  // All of our stored connections are still valid, we won't be adding any new.
	}
	
	// I know this ain't great but boy did I try other things (overlaps don't seem to work in editor, at least for me).
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACoverParent::StaticClass(), FoundActors);

	TArray<AAICoverEntryPoint*> ConnectableEntryPoints;
	
	for (const AActor* Actor : FoundActors)
	{
		const ACoverParent* CoverParent = Cast<ACoverParent>(Actor);

		if (IsValid(CoverParent) && (CoverParent->GetActorLocation() - Parent->GetActorLocation()).SizeSquared2D() < FMath::Square(MAX_COVER_PARENT_DISTANCE))
		{
			CoverParent->FindConnectableEntryPoints(ConnectableEntryPoints);
		}
	}

	for (AAICoverEntryPoint* EntryPoint : ConnectableEntryPoints)
	{
		// We do not connect with ourselves or invalid points.
		if (IaEntryPoints.Contains(EntryPoint) || !IsValid(EntryPoint))
		{
			continue;
		}
		
		// Checking which of our connections needs checking.
		if (!IsValid(ReachablePointForward_0) && IsPointReachableForward(true, EntryPoint))
		{
			ReachablePointForward_0 = EntryPoint;
			
			DrawDebugConnectionLine(World,true, ReachablePointForward_0, FColor::Green);
			PointsWhoseSplinesNeedUpdating.Add(ReachablePointForward_0);
		}
		else if (!IsValid(ReachablePointForward_1) && IsPointReachableForward(false, EntryPoint))
		{
			ReachablePointForward_1 = EntryPoint;

			DrawDebugConnectionLine(World,false, ReachablePointForward_1, FColor::Green);
			PointsWhoseSplinesNeedUpdating.Add(ReachablePointForward_1);
		}
		else if (!IsValid(ReachablePointContinuous_0) && IsPointReachableContinuous(true, EntryPoint))
		{
			ReachablePointContinuous_0 = EntryPoint;

			DrawDebugConnectionLine(World,true, ReachablePointContinuous_0, FColor::Green);
			PointsWhoseSplinesNeedUpdating.Add(ReachablePointContinuous_0);
		}
		else if (!IsValid(ReachablePointContinuous_1) && IsPointReachableContinuous(false, EntryPoint))
		{
			ReachablePointContinuous_1 = EntryPoint;
					
			DrawDebugConnectionLine(World,false, ReachablePointContinuous_1, FColor::Green);
			PointsWhoseSplinesNeedUpdating.Add(ReachablePointContinuous_1);
		}
	}

	// Notifying the covers that were affected by these changes that they need to check their own connections again.
	for (AAICoverEntryPoint* EntryPoint : PointsWhoseSplinesNeedUpdating)
	{
		UCoverSpline* CoverSpline = EntryPoint->EnsureOwningSplineIsSet();
		if (CoverSpline != nullptr || CoverThatOrderedTheUpdate != CoverSpline)
		{
			CoverSpline->UpdateReachableCovers(this);
		}
	}
}

bool UCoverSpline::EnsureSpawnOfIaEntryPoints()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}

	if (IsDisabled())
	{
		DestroyAllAttachedIaEntryPoints();
		return false;
	}

	// This is useful when we are returning from disabling a Spline.
	if (static_cast<int>(NumberOfIaEntryPoints) == 0)
	{
		ResetNumberOfIAEntryPoints();
	}

	bool bHasMadeChanges = false;
	int AttachedActorsCounter = 0;

	// Checking if any of our Entry Points is no longer valid.
	for (int Index = IaEntryPoints.Num() - 1; Index >= 0; Index--)
	{
		if (!IsValid(IaEntryPoints[Index]))
		{
			IaEntryPoints.RemoveAt(Index);
			bHasMadeChanges = true;
			continue;
		}
		
		AttachedActorsCounter++;
	}
	
	// If we need MORE Entry Points.
	if (AttachedActorsCounter < static_cast<int>(NumberOfIaEntryPoints))
	{
		bHasMadeChanges = true;
		UWorld* World = GetWorld();
		
		for (int EntryPointIndex = AttachedActorsCounter; EntryPointIndex < static_cast<int>(NumberOfIaEntryPoints); EntryPointIndex++)
		{
			const FString Label = GetName() + FString(TEXT("_EntryPoint_")) + FString::FromInt(EntryPointIndex);
			
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = Owner;

			// It doesn't matter where they spawn as that will be overriden later.
			const FVector SpawnLocation = FVector::Zero();
			const FRotator SpawnRotation = FRotator::ZeroRotator;

			AAICoverEntryPoint* CoverEntryPoint = Cast<AAICoverEntryPoint>(World->SpawnActor(
				AAICoverEntryPoint::StaticClass(),
				&SpawnLocation,
				&SpawnRotation,
				SpawnParameters
			));

			if (IsValid(CoverEntryPoint))
			{
				CoverEntryPoint->SetActorScale3D(FVector(0.15f, 0.15f, 0.15f));
				IaEntryPoints.Add(CoverEntryPoint);
				CoverEntryPoint->SetOwningSpline(this);
				CoverEntryPoint->AttachToActor(Owner, FAttachmentTransformRules::KeepWorldTransform);
				CoverEntryPoint->SetSection(0);
			}
		}
	}
	
	// If we need LESS Entry Points.
	else if(AttachedActorsCounter > static_cast<int>(NumberOfIaEntryPoints))
	{
		bHasMadeChanges = true;
		
		for (int Index = IaEntryPoints.Num() - 1;
			Index >= static_cast<int>(NumberOfIaEntryPoints);
			Index--)
		{
			IaEntryPoints[Index]->Destroy();
			IaEntryPoints.RemoveAt(Index);
			AttachedActorsCounter--;
		}
	}

	return bHasMadeChanges;
}

void UCoverSpline::DrawDebugConnectionLine(const UWorld* World,	const bool FromPoint0, const AAICoverEntryPoint* TargetEntryPoint, const FColor Color) const
{
#if UE_EDITOR
	if (!IsValid(TargetEntryPoint) || World == nullptr)
	{
		return;
	}
	
	if (IaEntryPoints.Num() == 0)
	{
		return;
	}
	const FVector OriginPoint = IaEntryPoints[FromPoint0 || IsSinglePointCover() ? 0 : IaEntryPoints.Num() -  1]->GetActorLocation() - AI_ENTRYPOINT_ELEVATION;
	float LineLifetime = -1;
	
	if (Color == FColor::Green || Color == FColor::Red)
	{
		LineLifetime = 2.0f;
	}
	else if (Color == FColor::Yellow)
	{
		LineLifetime = 0.5f;
	}

	if (LineLifetime >= 0)
	{
		DrawDebugLine(World, OriginPoint, TargetEntryPoint->GetActorLocation() - AI_ENTRYPOINT_ELEVATION, Color, false, LineLifetime);
	}
#endif
}

#pragma endregion
