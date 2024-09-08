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

class Precise_timer
{
public:
	void start();
	float get_interval() const;
private:
	std::chrono::high_resolution_clock::time_point start_time_point;
};

// Global singleton for extension's main interface.
Precise_time g_precise_time;
SMEXT_LINK(&g_precise_time);

// Used by global timer.
std::chrono::high_resolution_clock::time_point g_start_time_point;

// User created timers.
std::unordered_map<int, Precise_timer> g_timers;

// Unique serial ID for created timers.
int generate_serial()
{
	static int serial = 0;
	return ++serial;
}

// native void StartGlobalPreciseTimer()
cell_t native_StartGlobalPreciseTimer(IPluginContext *pContext, const cell_t *params)
{
	g_start_time_point = std::chrono::high_resolution_clock::now();
	return 1;
}

// native float GetGlobalPreciseTimeInterval()
cell_t native_GetGlobalPreciseTimeInterval(IPluginContext *pContext, const cell_t *params)
{
	const auto now = std::chrono::high_resolution_clock::now();
	return sp_ftoc(std::chrono::duration<float, std::chrono::milliseconds::period>(now - g_start_time_point).count());
}

// native int CreatePreciseTimer()
cell_t native_CreatePreciseTimer(IPluginContext *pContext, const cell_t *params)
{
	const int serial = generate_serial();
	g_timers.insert({ serial, Precise_timer() });
	return serial;
}

// native void DeletePreciseTimer(int serial)
cell_t native_DeletePreciseTimer(IPluginContext *pContext, const cell_t *params)
{
	const int serial = params[1];
	g_timers.erase(serial);
	return 1;
}

// native void StartPreciseTimer(int serial)
cell_t native_StartPreciseTimer(IPluginContext *pContext, const cell_t *params)
{
	const int serial = params[1];

	try {
		g_timers.at(serial).start();
	}
	catch (std::out_of_range) {
		return pContext->ThrowNativeError("Bad serial");
	}

	return 1;
}

// native float GetPreciseTimeInterval(int serial)
cell_t native_GetPreciseTimeInterval(IPluginContext *pContext, const cell_t *params)
{
	const int serial = params[1];

	try {
		return sp_ftoc(g_timers.at(serial).get_interval());
	}
	catch (std::out_of_range) {
		return pContext->ThrowNativeError("Bad serial");
	}
}

// native void ThreadSleep(int ms)
cell_t native_ThreadSleep(IPluginContext *pContext, const cell_t *params)
{
	const int ms = params[1];
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	return 1;
}

// native void PreciseThreadSleep(float ms, int accounted_error = 2)
cell_t native_PreciseThreadSleep(IPluginContext *pContext, const cell_t *params)
{
	const float ms = sp_ctof(params[1]);
	const int accounted_error = params[2];
	const auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(ms) - accounted_error));
	while (std::chrono::duration<float, std::chrono::milliseconds::period>(std::chrono::high_resolution_clock::now() - start).count() < ms)
		continue;
	return 1;
}

void Precise_timer::start()
{
	start_time_point = std::chrono::high_resolution_clock::now();
}

float Precise_timer::get_interval() const
{
	const auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::chrono::milliseconds::period>(now - start_time_point).count();
}

// Bind natives.
sp_nativeinfo_t g_natives[] = {
	{ "StartGlobalPreciseTimer", native_StartGlobalPreciseTimer },
	{ "GetGlobalPreciseTimeInterval", native_GetGlobalPreciseTimeInterval },
	{ "CreatePreciseTimer", native_CreatePreciseTimer },
	{ "DeletePreciseTimer", native_DeletePreciseTimer },
	{ "StartPreciseTimer", native_StartPreciseTimer },
	{ "GetPreciseTimeInterval", native_GetPreciseTimeInterval },
	{ "ThreadSleep", native_ThreadSleep },
	{ "PreciseThreadSleep", native_PreciseThreadSleep },
	{ NULL, NULL }
};

void Precise_time::SDK_OnAllLoaded()
{
	sharesys->AddNatives(myself, g_natives);
}
