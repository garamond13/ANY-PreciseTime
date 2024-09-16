#if defined _precise_time_included
 #endinput
#endif
#define _precise_time_included

/**
 * Starts or restarts the global PreciseTimer. Note that this timer may be
 * shared across all plugins.
 */
native void StartGlobalPreciseTimer();

/**
 * Get time interval since most recent start of the global PreciseTimer.
 *
 * @return Time in miliseconds.
 */
native float GetGlobalPreciseTimeInterval();

/**
 * Creates PreciseTimer instance. Note that it should
 * be deleted with DeletePreciseTimer.
 *
 * @return Unique valid PreciseTimer serial of the created timer.
 */
native int CreatePreciseTimer();

/**
 * Deletes PreciseTimer instance. Note that the serial will be invalid
 * after deletion.
 */
native void DeletePreciseTimer(int serial);

/**
 * Starts or restarts PreciseTimer instance.
 *
 * @param serial	Valid PreciseTimer serial.
 */
native void StartPreciseTimer(int serial);

/**
 *  Get time interval since most recent start of the PreciseTimer instance.
 *
 * @param serial	Valid PreciseTimer serial.
 * 
 * @return Time in miliseconds.
 */
native float GetPreciseTimeInterval(int serial);

/**
 *  Check is PreciseTimer serial valid.
 *
 * @param serial	PreciseTimer serial.
 * 
 * @return True if valid, false if invalid.
 */
native bool IsValidPreciseTimer(int serial);

/**
 * Sleeps this thread.
 *
 * @param ms    Sleep for this many milliseconds.
 */
native void ThreadSleep(int ms);

/**
 * Sleeps this thread. More accurate version of ThreadSleep, but may consume more CPU.
 *
 * @param ms    Sleep for this many milliseconds.
 * @param accounted_error	Accounts for longer sleep this many miliseconds.
 */
native void PreciseThreadSleep(float ms, int accounted_error = 2);

/**
 * Do not edit below this line!
 */
public Extension __ext_precise_time = 
{
	name = "PreciseTime",
	file = "precise.time.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};