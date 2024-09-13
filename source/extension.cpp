/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <stdexcept>

class PreciseTimer
{
public:
	void Start() noexcept;
	float GetInterval() const noexcept;
private:
	std::chrono::high_resolution_clock::time_point startTimePoint;
};

// Global singleton for extension's main interface.
PreciseTime g_PreciseTime;
SMEXT_LINK(&g_PreciseTime);

// Used by global timer.
std::chrono::high_resolution_clock::time_point g_StartTimePoint;

// User created timers.
std::unordered_map<int, PreciseTimer> g_Timers;

// Unique serial ID for created timers.
int GenerateSerial() noexcept
{
	static int serial = 0;
	return ++serial;
}

// native void StartGlobalPreciseTimer()
cell_t Native_StartGlobalPreciseTimer(IPluginContext *pContext, const cell_t *params) noexcept
{
	g_StartTimePoint = std::chrono::high_resolution_clock::now();
	return 1;
}

// native float GetGlobalPreciseTimeInterval()
cell_t Native_GetGlobalPreciseTimeInterval(IPluginContext *pContext, const cell_t *params) noexcept
{
	const auto now = std::chrono::high_resolution_clock::now();
	return sp_ftoc(std::chrono::duration<float, std::chrono::milliseconds::period>(now - g_StartTimePoint).count());
}

// native int CreatePreciseTimer()
cell_t Native_CreatePreciseTimer(IPluginContext *pContext, const cell_t *params)
{
	const int serial = GenerateSerial();
	g_Timers.insert({ serial, PreciseTimer() });
	return serial;
}

// native void DeletePreciseTimer(int serial)
cell_t Native_DeletePreciseTimer(IPluginContext *pContext, const cell_t *params) noexcept
{
	const int serial = params[1];
	g_Timers.erase(serial);
	return 1;
}

// native void StartPreciseTimer(int serial)
cell_t Native_StartPreciseTimer(IPluginContext *pContext, const cell_t *params) noexcept
{
	const int serial = params[1];

	try {
		g_Timers.at(serial).Start();
	}
	catch (std::out_of_range) {
		pContext->ReportError("Bad serial");
		return 0;
	}

	return 1;
}

// native float GetPreciseTimeInterval(int serial)
cell_t Native_GetPreciseTimeInterval(IPluginContext *pContext, const cell_t *params) noexcept
{
	const int serial = params[1];

	try {
		return sp_ftoc(g_Timers.at(serial).GetInterval());
	}
	catch (std::out_of_range) {
		pContext->ReportError("Bad serial");
		return 0;
	}
}

// native void ThreadSleep(int ms)
cell_t Native_ThreadSleep(IPluginContext *pContext, const cell_t *params) noexcept
{
	const int ms = params[1];
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	return 1;
}

// native void PreciseThreadSleep(float ms, int accounted_error = 2)
cell_t Native_PreciseThreadSleep(IPluginContext *pContext, const cell_t *params) noexcept
{
	const float ms = sp_ctof(params[1]);
	const int accounted_error = params[2];
	const auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(ms) - accounted_error));
	while (std::chrono::duration<float, std::chrono::milliseconds::period>(std::chrono::high_resolution_clock::now() - start).count() < ms)
		continue;
	return 1;
}

void PreciseTimer::Start() noexcept
{
	startTimePoint = std::chrono::high_resolution_clock::now();
}

float PreciseTimer::GetInterval() const noexcept
{
	const auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::chrono::milliseconds::period>(now - startTimePoint).count();
}

// Bind natives.
sp_nativeinfo_t g_Natives[] = {
	{ "StartGlobalPreciseTimer", Native_StartGlobalPreciseTimer },
	{ "GetGlobalPreciseTimeInterval", Native_GetGlobalPreciseTimeInterval },
	{ "CreatePreciseTimer", Native_CreatePreciseTimer },
	{ "DeletePreciseTimer", Native_DeletePreciseTimer },
	{ "StartPreciseTimer", Native_StartPreciseTimer },
	{ "GetPreciseTimeInterval", Native_GetPreciseTimeInterval },
	{ "ThreadSleep", Native_ThreadSleep },
	{ "PreciseThreadSleep", Native_PreciseThreadSleep },
	{ NULL, NULL }
};

void PreciseTime::SDK_OnAllLoaded()
{
	sharesys->AddNatives(myself, g_Natives);
}
