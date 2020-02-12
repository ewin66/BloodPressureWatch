/*
 * gearfit2_dongle.h
 *
 *  Created on: Feb 18, 2019
 *      Author: James Weimer and Ben Chrepta
 */

#ifndef __gearfit2_dongle_H__
#define __gearfit2_dongle_H__

#include <device/power.h>
#include <dlog.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <sensor.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "gearfit2_dongle"

#define ERR(fmt, args...) dlog_print(DLOG_ERROR, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define WARN(fmt, args...) dlog_print(DLOG_WARN, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define INFO(fmt, args...)  dlog_print(DLOG_INFO, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)
#define DBG(fmt, args...) dlog_print(DLOG_DEBUG, LOG_TAG, "%s : %s(%d) > "fmt"\n", rindex(__FILE__, '/') + 1, __func__, __LINE__, ##args)

#define FN_CALL dlog_print(DLOG_DEBUG, LOG_TAG, ">>>>>>>> called")
#define FN_END dlog_print(DLOG_DEBUG, LOG_TAG, "<<<<<<<< ended")

#ifdef __cplusplus
}
#endif


//#define HOST "192.168.1.5"
//#define PORT 26101
//#define QOS 2
//#define delimiter "\n"
//#define clientid "stdout_subscriber"

#define TOPIC "jim" //jim
#define HR "/PRECISE/IoMT/GearFit2/Data/c5c1928d-72ac-4dbc-8ff3-01cdc43f72f6/HR" //HR topic
#define ACCEL "/PRECISE/IoMT/GearFit2/Data/c5c1928d-72ac-4dbc-8ff3-01cdc43f72f6/ACCEL" //acceleration topic



#define MYSERVICELAUNCHER_APP_ID "com.precise.samdware.ui.gearfit2" // ID of the UI application of our package (service launcher)
#define STRNCMP_LIMIT 256 // the limit of characters to be compared using strncmp function




#endif /* __gearfit2_dongle_H__ */



