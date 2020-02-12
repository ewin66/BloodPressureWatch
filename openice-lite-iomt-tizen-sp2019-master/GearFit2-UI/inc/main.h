/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#ifndef __gearfit2-ui_H__
//#define __gearfit2-ui_H__
 
#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <dlog.h>
#include <efl_extension.h>
 
#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "gearfit2-ui"
 
#define KEY_END "XF86Stop"
 
#if !defined(PACKAGE)
#define PACKAGE "com.precise.samdware.gearfit2-ui"
#endif
 
//#define ELM_DEMO_EDJ "/opt/usr/apps/$(packageName)/res/settings.edj"
//#define ELM_DEMO_EDJ "/Users/weimer/Documents/code/precise-biostat/GearFit2-UI/res/settings.edj"
//#define ELM_DEMO_EDJ "/opt/usr/apps/com.precise.samdware.gearfit2-ui/res/settings.edj"
//#define ICON_DIR "/opt/usr/apps/$(packageName)/res/images"
//#define ICON_DIR "/Users/weimer/Documents/code/precise-biostat/GearFit2-UI/res/images"
#define ICON_DIR "/opt/usr/apps/com.precise.samdware.gearfit2-ui/res/images"

#define OPENICE_OFF 0
#define OPENICE_ON 1
#define OPENICE_DISCONNECT 2

// main menu constants
#define OPENICE_MAIN_MENU_NUM 3
#define OPENICE_MAIN_MENU_START 0
#define OPENICE_ONOFF_IDX 0
#define OPENICE_SETTINGS_IDX 1
#define OPENICE_ABOUT_IDX 2

// settings settings constants
#define OPENICE_SETTING_MENU_NUM 7
#define OPENICE_SETTING_MENU_START 100
#define OPENICE_CLIENT_ID_IDX 100
#define OPENICE_BROKER_IDX 101
#define OPENICE_PASSWORD_IDX 102
#define OPENICE_PORT_NUM_IDX 103
#define OPENICE_NETWORK_PROTOCOL_IDX 104
#define OPENICE_CONNECTION_TIMEOUT_IDX 105
#define OPENICE_KEEP_ALIVE_IDX 106
#define OPENICE_USER_IDX 107
#define OPENICE_SAMPLE_NUM 108
#define OPENICE_SAMPLE_RATE 109




// about menu constants
#define OPENICE_ABOUT_MENU_NUM 3
#define OPENICE_ABOUT_MENU_START 200
#define OPENICE_DEV_INFO_IDX 200
#define OPENICE_DISCLAIMER_IDX 201
#define OPENICE_LICENSE_IDX 202

// OpenICE-lite Service APP Information
#define OPENICE_SERVICE_APP_ID "com.precise.samdware.cdongle" // can this be automatically imported?



// Text Inputs
#define OPENICE_DEV_INFO "coming soon."

#define OPENICE_DISCLAIMER "coming soon."

#define OPENICE_LICENSE "coming soon."

#define OPENICE_USERID "c5c1928d-72ac-4dbc-8ff3-01cdc43f72f6"

