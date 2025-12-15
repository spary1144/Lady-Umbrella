// Fill out your copyright notice in the Description page of Project Settings.

#include "SecondOrderFunctionLibrary.h"

FSecondOrderConstants USecondOrderFunctionLibrary::CreateSecondOrderConstants(
	const float _Frequency,
	const float _Dampening,
	const float _InitialResponse)
{
	FSecondOrderConstants result(_Frequency, _Dampening, _InitialResponse);

	return result;
}
