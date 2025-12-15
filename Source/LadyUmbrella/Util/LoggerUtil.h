// 
// Logger.h
// 
// Utility that generalizes printing to screen. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#define DEBUG_BUFFER_SIZE 256
#define PREPARE_BUFFER()                 \
	char DebugBuffer[DEBUG_BUFFER_SIZE]; \
	va_list ArgList;                     \
	va_start(ArgList, Message);          \
	vsnprintf(DebugBuffer,               \
		DEBUG_BUFFER_SIZE,               \
		Message,                         \
		ArgList                          \
	)

static void LogWithoutScreenMessage(const FString& Prefix, const FString& Message)
{
	const FString FormattedMessage = Prefix + Message;
	
	UE_LOG(LogTemp, Log, TEXT("%s"), *FormattedMessage);
}

static void ConsoleLog(const FString& Prefix, FColor Color, const FString& Message)
{
	const FString FormattedMessage = Prefix + Message;
	
	if (GEngine && true)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			Color,
			FormattedMessage
		);
	}

	LogWithoutScreenMessage(Prefix, Message);
}

namespace FLogger
{
	static void DebugLog(const char* Message, ...)
	{
		PREPARE_BUFFER();
		ConsoleLog("[DEBUG]: ", FColor::Cyan, DebugBuffer);
	}
	
	static void DebugLog(const FString& Message)
	{
		ConsoleLog("[DEBUG]: ", FColor::Cyan, Message);
	}
	
	static void ErrorLog(const char* Message, ...)
	{
		PREPARE_BUFFER();
		ConsoleLog("[ERROR]: ", FColor::Red, DebugBuffer);
	}
	
	static void ErrorLog(const FString& Message)
	{
		ConsoleLog("[ERROR]: ", FColor::Red, Message);
	}

	static void InfoLog(const char* Message, ...)
	{
		PREPARE_BUFFER();
		ConsoleLog("[INFO]: ", FColor::Green, DebugBuffer);
	}
	
	static void InfoLog(const FString& Message)
	{
		ConsoleLog("[INFO]: ", FColor::Green, Message);
	}

	static void WarningLog(const char* Message, ...)
	{
		PREPARE_BUFFER();
		ConsoleLog("[WARNING]: ", FColor::Yellow, DebugBuffer);
	}
	
	static void WarningLog(const FString& Message)
	{
		ConsoleLog("[WARNING]: ", FColor::Yellow, Message);
	}

	static void NoScreenMessageLog(const char* Message, ...)
	{
		PREPARE_BUFFER();
		LogWithoutScreenMessage("[LOG]: ", DebugBuffer);
	}
	
	static void NoScreenMessageLog(const FString& Message)
	{
		LogWithoutScreenMessage("[LOG]: ", Message);
	}
}
