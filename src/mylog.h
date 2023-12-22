/**
 * @brief define LOGVERBOSE macro.
*/

#define LOGVERBOSE(...) if (Serial.dtr()) Log.verbose(__VA_ARGS__)
