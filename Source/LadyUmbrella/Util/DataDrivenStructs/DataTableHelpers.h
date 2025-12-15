// 
// DataTableHelpers.h
// 
// Tool that retrieves a Struct with the information of the row given 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#pragma once

namespace FDataTableHelpers
{

	template<typename T>
	static T* ValidateTableAndGetData(FDataTableRowHandle& DataTableRowHandler, const FName& RowName, const FString& Context = "")
	{
		if (!IsValid(DataTableRowHandler.DataTable) || DataTableRowHandler.IsNull())
		{
			return nullptr;
		}
		return DataTableRowHandler.DataTable->FindRow<T>(RowName, Context);
	}
}
