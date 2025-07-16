/* A fan control application for Lenovo laptops.
 * 
 * References:
 * https://github.com/bitrate16/FanControl/blob/main/FanControl/FanControl.cpp
 * https://github.com/Soberia/Lenovo-IdeaPad-Z500-Fan-Controller?tab=readme-ov-file#-about
 * https://www.allstone.lt/ideafan/
 */
#ifndef LENOVO_FAN_CONTROL_H
#define LENOVO_FAN_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Set this variable to 0 to terminate keep_fan_running()
 */
volatile extern int is_keep_fan_running;

/**
 * Possible fan spinning mode.
 *
 * NORMAL: Fan spins at normal speed.
 * FAST: Fan spins at maximum speed.
 */
enum FanMode { NORMAL, FAST };

/**
 * Control the operating mode of the fan.
 *
 * @param mode: Set the fan spinning mode:
 *      NORMAL  Spin at normal speed.
 *      FAST    Spin at maximum speed.
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
 *      FAST    Spin at maximum speed.
 */
enum FanMode read_state();

/**
 * Try to keep the fan in fast mode. 
 * Block until done. Or set "is_keep_fan_running" variable to 0 to terminate.
 * 
 * For more information, refer to:
 * https://github.com/Soberia/Lenovo-IdeaPad-Z500-Fan-Controller?tab=readme-ov-file#-about
 */
void keep_fan_running();

#ifdef __cplusplus
}
#endif

#endif
