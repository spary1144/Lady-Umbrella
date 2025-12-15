// Fill out your copyright notice in the Description page of Project Settings.


#include "SecondOrderStructsCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "LadyUmbrella/Math/SecondOrderSystems/SecondOrderStructs.h"
#include "UObject/StructOnScope.h"

TSharedRef<IPropertyTypeCustomization> FSecondOrderParamsCustomization::MakeInstance()
{
	return MakeShared<FSecondOrderParamsCustomization>();
}

void FSecondOrderParamsCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - The header customization is called"), ANSI_TO_TCHAR(__FUNCTION__));
	
	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent();
}

void FSecondOrderParamsCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> FrequencyPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSecondOrderParams, Frequency));
	TSharedPtr<IPropertyHandle> DampeningPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSecondOrderParams, Dampening));
	TSharedPtr<IPropertyHandle> InitialResponsePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSecondOrderParams, InitialResponse));
	
	ChildBuilder
	.AddCustomRow(FText::FromString(TEXT("Second Order Parameters")))
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.Padding(0, 5, 5, 5)
		.MaxHeight(30)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(3)
			.MaxWidth(90)
			[
				FrequencyPropertyHandle->CreatePropertyNameWidget()
			]
			+SHorizontalBox::Slot()
			.Padding(3)
			[
				FrequencyPropertyHandle->CreatePropertyValueWidget()
			]
		]
		+SVerticalBox::Slot()
		.Padding(0, 5, 5, 5)
		.MaxHeight(30)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(3)
			.MaxWidth(90)
			[
				DampeningPropertyHandle->CreatePropertyNameWidget()
			]
			+SHorizontalBox::Slot()
			.Padding(3)
			[
				DampeningPropertyHandle->CreatePropertyValueWidget()
			]
		]
		+SVerticalBox::Slot()
		.Padding(0, 5, 5, 5)
		.MaxHeight(30)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(3)
			.MaxWidth(90)
			[
				InitialResponsePropertyHandle->CreatePropertyNameWidget()
			]
			+SHorizontalBox::Slot()
			.Padding(3)
			[
				InitialResponsePropertyHandle->CreatePropertyValueWidget()
			]
		]
		+SVerticalBox::Slot()
		.MinHeight(250)
		.Padding(0, 5, 5, 5)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			[
				SNew(SImage)
				.ColorAndOpacity(FLinearColor::Black)
			]
		]
		+SVerticalBox::Slot()
		.MinHeight(5)
	];
}

void FSecondOrderParamsCustomization::OnStructParamsChanged(TSharedPtr<IPropertyHandle> ParamsStructHandle)
{
	
}
