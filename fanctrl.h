/* A fan control application for Lenovo laptops.
 * 
 * References:
 * https://github.com/bitrate16/FanControl/blob/main/FanControl/FanControl.cpp
 * https://github.com/Soberia/Lenovo-IdeaPad-Z500-Fan-Controller?tab=readme-ov-file#-about
 * https://www.allstone.lt/ideafan/
 */
#ifndef LENOVO_FAN_CONTROL_H
#define LENOVO_FAN_CONTROL_H

/**
 * Possible fan spinning mode.
 *
 * NORMAL: Fan spins at normal speed.
 * FAST: Fan spins at the highest speed.
 */
enum FanMode { NORMAL, FAST };

/**
 * Control the operating mode of the fan.
 *
 * @param mode: Set the fan spinning mode:
 *      NORMAL  Spin at normal speed.
 *      FAST    Spin at the highest speed.
 * @return Operating result:
 *      -1  Failed to access \\.\EnergyDrv device.
 *      1   Succeeded.
 */
int fan_control(enum FanMode mode);

/**
 * Get the current operating mode of the fan.
 *
 * @return The current status of the fan:
 *      -1      Failed to access \\.\EnergyDrv device.
 *      NORMAL  Spin at normal speed.
 *      FAST    Spin at the highest speed.
 */
enum FanMode read_state();

/**
 * Try to keep the fan in fast mode for a specific period of time or forever. 
 * Block until done.
 * 
 * For more information, refer to:
 * https://github.com/Soberia/Lenovo-IdeaPad-Z500-Fan-Controller?tab=readme-ov-file#-about
 *
 * @param duration: Keep the fan spinning at the highest speed for the specified time, 
 *      in milliseconds. If nagetive value is given, then keep the fan spinning indefinitely.
 */
void keep_fast(int duration);

#endif
