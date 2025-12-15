#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Basics/State/EAlignment.h"
#include "FCreditSlide.generated.h"




USTRUCT(BlueprintType)
struct FCreditSlide : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 SlideNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="If under 3 max columns, columns will be removed in order from left to right."))
	EAlignment ColumnField;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="If under 3 max columns, columns will be removed in order from left to right."))
	EAlignment ColumnName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 SlideMaxFieldCols;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 SlideMaxNameCols;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Field;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true))
	FText Names;
};
