// Fill out your copyright notice in the Description page of Project Settings.


#include "SecondOrderStructs.h"

FSecondOrderConstants::FSecondOrderConstants(const float _Frequency, const float _Dampening, const float _InitialResponse)
{
	w = PI * 2 * _Frequency;
	z = _Dampening;
	d = w * sqrt(abs(_Dampening * _Dampening - 1));
	k1 = _Dampening / (PI * _Frequency);
	k2 = 1 / (w * w);
	k3 = _InitialResponse * _Dampening / w;
}

FSecondOrderConstants::FSecondOrderConstants(const FSecondOrderParams& _Parameters)
{
	w = PI * 2 * _Parameters.Frequency;
	z = _Parameters.Dampening;
	d = w * sqrt(abs(_Parameters.Dampening * _Parameters.Dampening - 1));
	k1 = _Parameters.Dampening / (PI * _Parameters.Frequency);
	k2 = 1 / (w * w);
	k3 = _Parameters.InitialResponse * _Parameters.Dampening / w;
}

FSecondOrderParams::FSecondOrderParams(float _Frequency, float _Dampening, float _InitialResponse)
{
	if (_Frequency <= 0)
	{
		_Frequency = 1;
	}
	if (_Dampening < 0)
	{
		_Dampening = 1;
	}

	Frequency = _Frequency;
	Dampening = _Dampening;
	InitialResponse = _InitialResponse;
}
