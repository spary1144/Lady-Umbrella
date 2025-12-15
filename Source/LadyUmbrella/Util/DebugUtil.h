// 
// DebugUtil.h
// 
// Collection of debug functions intended to help development of the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

// TODO: Should these be disabled when shipping a production build? 
#define Assert(condition) if (!condition) { *(int*) 0 = 0; }
#define StaticAssert(condition) typedef char [ condition ? 1 : -1 ]
