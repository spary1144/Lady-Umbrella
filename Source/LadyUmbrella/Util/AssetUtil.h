// 
// AssetUtil.h
// 
// Tool that helps finding assets and checking if they exist. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

namespace AssetUtils
{
	template <typename T>
	TSubclassOf<T> FindClass(const FString& Path)
	{
		ConstructorHelpers::FClassFinder<T> AssetBinary(*Path);

		if (!AssetBinary.Succeeded())
		{
			UE_LOG(LogTemp, Display, TEXT("Could not find requested class (%ls)"), *Path);
			return nullptr;
		}

		return AssetBinary.Class;
	}
	
	template <typename T>
	T* FindObject(const FString& Path)
	{
		ConstructorHelpers::FObjectFinder<T> AssetBinary(*Path);

		if (!AssetBinary.Succeeded())
		{
			UE_LOG(LogTemp, Display, TEXT("Could not find requested object (%ls)"), *Path);
			return nullptr;
		}

		return AssetBinary.Object;
	}
}