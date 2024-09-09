# ANY PreciseTime
 
A SourceMod extension. It privides precise timers and thread sleep.

## Usage example

```
#include <sourcemod>
#include <precise_time>

int g_precise_timer;

public void OnPluginStart()
{
	g_precise_timer = CreatePreciseTimer();
}

public void OnGameFrame()
{
	float interval = GetPreciseTimeInterval(g_precise_timer);
	StartPreciseTimer(g_precise_timer);
	PrintToChatAll("Frame time in ms: %f", interval);
}

public void OnPluginEnd()
{
	DeletePreciseTimer(g_precise_timer);
}
```

## Compilation

It's similar to building SourceMod itself, so you can setup the build environment as it's done here https://wiki.alliedmods.net/Building_SourceMod 
You could now put the `source` folder (feel free to rename it) into `alliedmodders\sourcemod\public`. Inside source folder create `build` folder.
Open a terminal inside the `build` folder and run `python ../configure.py --enable-optimize --sdks comma_separated_list_of_sdks_you_wish_to_compile_for`.
Next run `ambuild` to build binaries, built binaries should be in `build\package` folder.

Alternatively you could fork this repository and rely on `.github/workflows` to compile it for you.