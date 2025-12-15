#pragma once

UENUM()
enum class EChapterNames : uint8
{
	Chapter0 UMETA(DisplayName = "Chapter 0: Root Cinematic"),
	Chapter1 UMETA(DisplayName = "Chapter 1: Streets"),
	Chapter2 UMETA(DisplayName = "Chapter 2: Museum"),
	Chapter3 UMETA(DisplayName = "Chapter 3: Rooftops"),
	Chapter4 UMETA(DisplayName = "Chapter 4: Construction Rooftops"),
	Chapter5 UMETA(DisplayName = "Chapter 5: Building")
};

static FString ToString(const EChapterNames& ChapterName)
{
	switch (ChapterName)
	{
		case EChapterNames::Chapter1:
			return FString("L_Streets_Persistent");
		case EChapterNames::Chapter2:
			return FString("L_Museum_Persistent");
		case EChapterNames::Chapter3:
			return FString("L_Persistent_RoofTops");
		case EChapterNames::Chapter4:
			return FString("L_ConstructionRooftops_PersistentLevel");
		case EChapterNames::Chapter5:
			return FString("L_Building_Persistent");
	default:
		return FString("INVALID_CHAPTER");
	}
}

static FName ToName(const EChapterNames& ChapterName)
{
	switch (ChapterName)
	{
	case EChapterNames::Chapter0:
		return FName("L_Chapter1_CinematicBlockout");
	case EChapterNames::Chapter1:
		return FName("L_Streets_Persistent");
	case EChapterNames::Chapter2:
		return FName("L_Museum_Persistent");
	case EChapterNames::Chapter3:
		return FName("L_Persistent_RoofTops");
	case EChapterNames::Chapter4:
		return FName("L_ConstructionRooftops_PersistentLevel");
	case EChapterNames::Chapter5:
		return FName("L_Building_Persistent");
	default:
		return FName("INVALID_CHAPTER");
	}
}