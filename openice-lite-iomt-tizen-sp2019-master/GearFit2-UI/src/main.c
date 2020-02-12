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
     
    #include "main.h"

    typedef struct appdata {
        Evas_Object *win;
    } appdata_s;
     
    typedef struct _Item_Data
    {
        int index;
        Elm_Object_Item *item;
    } Item_Data;
     
    const char *message_openice[] = {"<color=#D3D3D3>Off",
    									"<color=#00FF00>On",
									"<color=#FF0000>ERROR: 001"};


    void gl_selected_cb(void* , Evas_Object* , void* );

    char *deviceInfo[]= {
    		"tcp://mqtt-dev.precise.cis.upenn.edu:1883", /* Address of MQTT Server*/
			"mqtt-admin", /* username*/
			"precise-mqtt-dev", /* password*/
			"c5c1928d-72ac-4dbc-8ff3-01cdc43f72f6", /*User ID*/
			"1883", /*Port : 1883 is public*/
			"tcp", /*protocol used, SSL not implemented yet*/
			"10000", /*timeout in ms*/
			"20",   /*samples per packet*/
			"20",  /*keepalive interval*/
			"50"   /*sampling rate in ms*/
    };
    static Evas_Object *
    create_label(Evas_Object *parent, char *text);

     
    static void
    win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
    {
        ui_app_exit();
    }
     
    static Eina_Bool
    nf_it_pop_cb(void *data, Elm_Object_Item *it)
    {
        appdata_s *ad = data;
        /* Let window go to hide state. */
        elm_win_lower(ad->win);
        return EINA_FALSE;
    }
     
    static void
    nf_back_cb(void *data, Evas_Object *obj, void *event_info)
    {
        elm_naviframe_item_pop(obj);
    }
     
     
    static void
    gl_del(void *data, Evas_Object *obj EINA_UNUSED)
    {
        /* FIXME: Unrealized callback can be called after this. */
        /* Accessing Item_Data can be dangerous on unrealized callback. */
        Item_Data *id = data;
        free(id);
    }

    static int
	openice_status_get()
    {
		return OPENICE_ON;
    }


     
    static char *
    gl_text_get(void *data, Evas_Object *obj, const char *part)
    {
        char buf[1024];
        Item_Data *id = (Item_Data *)data;
        int index = id->index;
     
        if (part && !strcmp(part, "elm.text")) {
			switch(index) {
					case OPENICE_ONOFF_IDX:
						break;
					case OPENICE_SETTINGS_IDX:
						snprintf(buf, 1023, "%s", "Settings");
						break;
					case OPENICE_ABOUT_IDX:
						snprintf(buf, 1023, "%s", "About");
						break;
					case OPENICE_CLIENT_ID_IDX:
						snprintf(buf, 1023, "%s", "Client ID");
						break;
					case OPENICE_BROKER_IDX:
						snprintf(buf, 1023, "%s", "Broker");
						break;
					case OPENICE_PASSWORD_IDX:
						snprintf(buf, 1023, "%s", "Password");
						break;
					case OPENICE_PORT_NUM_IDX:
						snprintf(buf, 1023, "%s", "Port Number");
						break;
					case OPENICE_NETWORK_PROTOCOL_IDX:
						snprintf(buf, 1023, "%s", "Network Protocol");
						break;
					case OPENICE_CONNECTION_TIMEOUT_IDX:
						snprintf(buf, 1023, "%s", "Timeout");
						break;
					case OPENICE_KEEP_ALIVE_IDX:
						snprintf(buf, 1023, "%s", "Keep Alive");
						break;
					case OPENICE_USER_IDX:
						snprintf(buf, 1023, "%s", "User ID");
						break;
					case OPENICE_SAMPLE_NUM:
						// Enter the broker
						snprintf(buf, 1023, "%s", "Packets per Sample");
						break;
					case OPENICE_SAMPLE_RATE:
						// Enter the broker
						snprintf(buf, 1023, "%s", "Sampling Rate");
						break;
					case OPENICE_DEV_INFO_IDX:
						snprintf(buf, 1023, "%s", "Dev Info");
						break;
					case OPENICE_DISCLAIMER_IDX:
						snprintf(buf, 1023, "%s", "Disclaimer");
						break;
					case OPENICE_LICENSE_IDX:
						snprintf(buf, 1023, "%s", "License");
						break;
					default:
						dlog_print(DLOG_INFO, LOG_TAG, "Received an unknown OPENICE menu index");
						return NULL;
			}
			return strdup(buf);

        } else if (part) {
			switch(index) {
					case OPENICE_ONOFF_IDX:
						snprintf(buf, 1023, "%s", message_openice[openice_status_get()]);
						break;
					case OPENICE_SETTINGS_IDX:
						break;
					case OPENICE_ABOUT_IDX:
						snprintf(buf, 1023, "%s", "v0.1"); // have this link to actual version
						break;
					case OPENICE_CLIENT_ID_IDX:
						snprintf(buf, 1023, "%s", "wearable/jim"); //THIS WAS wearable/jim
						break;
					case OPENICE_BROKER_IDX:
						snprintf(buf, 1023, "%s", "precise");
						break;
					case OPENICE_PASSWORD_IDX:
						break;
					case OPENICE_PORT_NUM_IDX:
						break;
					case OPENICE_NETWORK_PROTOCOL_IDX:
						break;
					case OPENICE_CONNECTION_TIMEOUT_IDX:
						break;
					case OPENICE_KEEP_ALIVE_IDX:
						break;
					case OPENICE_USER_IDX:
						break;
					case OPENICE_SAMPLE_NUM:
						break;
					case OPENICE_SAMPLE_RATE:
						break;
					case OPENICE_DEV_INFO_IDX:
						snprintf(buf, 1023, "%s", "v0.1"); // have this link to actual version
						break;
					case OPENICE_DISCLAIMER_IDX:
						break;
					case OPENICE_LICENSE_IDX:
						break;
					default:
						dlog_print(DLOG_INFO, LOG_TAG, "Received an unknown OPENICE menu index");
						return NULL;
			}

			return strdup(buf);
        }

		return NULL;
    }




     
    static void
    gl_loaded_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
    {
    }
     
    static void
    gl_realized_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
    {
        /* If you need, you can get item's data. */
        // Elm_Object_Item *it = event_info;
        // Item_Data *id = elm_object_item_data_get(it);
    }
     
    static void
    gl_longpressed_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
    {
        /* If you need, you can get item's data. */
        // Elm_Object_Item *it = event_info;
        // Item_Data *id = elm_object_item_data_get(it);
    }
     

    static void
    gl_contracted_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
    {
       Elm_Object_Item *it = event_info;
       elm_genlist_item_subitems_clear(it);
    }




    static void launch_openice_service()
    {
        app_control_h app_control = NULL;
        if (app_control_create(&app_control)== APP_CONTROL_ERROR_NONE)
        {
            if ((app_control_set_app_id(app_control, OPENICE_SERVICE_APP_ID) == APP_CONTROL_ERROR_NONE)
            		&& (app_control_add_extra_data_array(app_control, "config_data", deviceInfo, 10) == APP_CONTROL_ERROR_NONE)
					/*(app_control_add_extra_data(app_control, "config_data", test) == APP_CONTROL_ERROR_NONE) */
                && (app_control_send_launch_request(app_control, NULL, NULL) == APP_CONTROL_ERROR_NONE))
            {
            	dlog_print(DLOG_INFO, LOG_TAG, "App launch request sent!");
            }
            else
            {
                dlog_print(DLOG_ERROR, LOG_TAG, "App launch request sending failed!");
            }
            if (app_control_destroy(app_control) == APP_CONTROL_ERROR_NONE)
            {
                dlog_print(DLOG_INFO, LOG_TAG, "App control destroyed.");
            }
            // We exit our launcher app, there is no point in keeping it open. -- TODO: Jim disagrees -- we want feedback!!!
            //ui_app_exit(); // JIM: we could fire up some sort of monitor (like the old app)
        }
        else
        {
            dlog_print(DLOG_ERROR, LOG_TAG, "App control creation failed!");
        }
    }



    static void stop_openice_service()
    {
        app_control_h app_control = NULL;
        if (app_control_create(&app_control)== APP_CONTROL_ERROR_NONE)
        {
            if ((app_control_set_app_id(app_control, OPENICE_SERVICE_APP_ID) == APP_CONTROL_ERROR_NONE)
                && (app_control_add_extra_data(app_control, "service_action", "stop") == APP_CONTROL_ERROR_NONE)
                && (app_control_send_launch_request(app_control, NULL, NULL) == APP_CONTROL_ERROR_NONE))
            {
                dlog_print(DLOG_INFO, LOG_TAG, "Stop launch request sent!");
            }
            else
            {
                dlog_print(DLOG_ERROR, LOG_TAG, "Stop launch request sending failed!");
            }
            if (app_control_destroy(app_control) == APP_CONTROL_ERROR_NONE)
            {
                dlog_print(DLOG_INFO, LOG_TAG, "Stop control destroyed.");
            }
            ui_app_exit();
        }
        else
        {
            dlog_print(DLOG_ERROR, LOG_TAG, "App control creation failed!");
        }
    }



    static void
    c_changed_cb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
    {

    		if (elm_check_state_get(obj)) {
    			//if (service_app_create()) {
    				// emit signal for error code
    			//}
    			dlog_print(DLOG_INFO, LOG_TAG, "On Selected");
    			launch_openice_service();
    		} else {
			// emit signal for error code
    			dlog_print(DLOG_INFO, LOG_TAG, "Off Selected");
    			stop_openice_service();
    		}





    }



    static Evas_Object *
    create_entry(Evas_Object *parent)
    {
    		Evas_Object *entry;
    		entry = elm_entry_add(parent);
    		elm_entry_entry_set(entry, "A short text.");

    		evas_object_show(entry);

    		return entry;
    }


    static Evas_Object *
    create_label(Evas_Object *parent, char *text)
    {
        Evas_Object *label;
        label = elm_label_add(parent);
        elm_label_line_wrap_set(label, ELM_WRAP_MIXED);

        if (text) {
            elm_object_text_set(label, text);
        }
        evas_object_show(label);

        return label;
    }


    static Evas_Object *
    create_onoff_box(Evas_Object *parent)
    {
    		Evas_Object *box;
        Evas_Object *check;
        Evas_Object *label;
        char buf[1024];

        box = elm_box_add(parent);
        evas_object_show(box);


        label = elm_label_add(box);
        snprintf(buf, 1023,
        		"<font=Sans:style=Regular font_size=24>%s</color></font/>",
			message_openice[openice_status_get()]);
        elm_object_text_set(label, buf);
        evas_object_show(label);

        check = elm_check_add(box);
        elm_object_style_set(check, "on&off");
        evas_object_smart_callback_add(check, "changed", c_changed_cb, NULL);
        evas_object_show(check);

        elm_box_pack_end(box, check);
        elm_box_pack_end(box, label);

        return box;
    }


    static Evas_Object *
	gl_content_get(void *data, Evas_Object *obj, const char *part)
	{
		//Item_Data *id = (Item_Data *)data;
		//int index = id->index;
		Evas_Object *box;

		box = create_onoff_box(obj);

		return box;
	}


    static Evas_Object *
    create_genlist(Evas_Object *parent,int idx_start, int idx_num)
    {
        Evas_Object *genlist;
        Elm_Object_Item *item;

        Elm_Genlist_Item_Class *it = elm_genlist_item_class_new();
        it->item_style = "2text";
        it->func.text_get = gl_text_get;
        it->func.del = gl_del;

        genlist = elm_genlist_add(parent);

        elm_scroller_single_direction_set(genlist, ELM_SCROLLER_SINGLE_DIRECTION_HARD);

        /* SCROLL MODE */
        elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

        /* HOMOGENEOUS MODE */
        elm_genlist_homogeneous_set(genlist, EINA_TRUE);

        /* Add Smart Callback */
        evas_object_smart_callback_add(genlist, "realized", gl_realized_cb, NULL);
        evas_object_smart_callback_add(genlist, "loaded", gl_loaded_cb, NULL);
        evas_object_smart_callback_add(genlist, "longpressed", gl_longpressed_cb, NULL);
        evas_object_smart_callback_add(genlist, "contracted", gl_contracted_cb, NULL);

        for (int i=idx_start; i<idx_num+idx_start; i++){ // +3 for idk_start
            Item_Data *id = calloc(sizeof(Item_Data), 1);
            id->index = i;
            item = elm_genlist_item_append(genlist,
                                            it,
                                            id,
                                            NULL,
                                            ELM_GENLIST_ITEM_NONE,
                                            gl_selected_cb,
                                            parent);
            id->item = item;
        }

		elm_genlist_item_class_free(it);
        evas_object_show(genlist);

        return genlist;
    }


    static void
    prepend_genlist(Evas_Object *parent, Evas_Object *genlist, int index)
    {
        Elm_Object_Item *item;

        Elm_Genlist_Item_Class *it = elm_genlist_item_class_new();
        it->item_style = "1icon"; //"2text.1icon";
        it->func.text_get = gl_text_get;
        it->func.content_get = gl_content_get;
        it->func.del = gl_del;

        Item_Data *id = calloc(sizeof(Item_Data), 1);
		id->index = index;
		item = elm_genlist_item_prepend(genlist,
										it,
										id,
										NULL,
										ELM_GENLIST_ITEM_NONE,
										gl_selected_cb,
										parent);
		id->item = item;

		elm_genlist_item_class_free(it);
		evas_object_show(genlist);
    }
     



    static void
    create_base_gui(appdata_s *ad)
    {
        Elm_Object_Item *nf_it;
        Evas_Object *conform, *nf, *genlist;
        int idx_start = OPENICE_MAIN_MENU_START;
        int idx_num = OPENICE_MAIN_MENU_NUM;
     
        /* Window */
        ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
        elm_win_conformant_set(ad->win, EINA_TRUE);
        elm_win_autodel_set(ad->win, EINA_TRUE);
     
        evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
     
        /* Conformant */
        conform =  elm_conformant_add(ad->win);
        elm_win_resize_object_add(ad->win, conform);
        evas_object_show(conform);
     
        /* Naviframe */
        nf = elm_naviframe_add(conform);
        eext_object_event_callback_add(nf, EEXT_CALLBACK_BACK, nf_back_cb, ad);
        elm_object_content_set(conform, nf);
        evas_object_show(nf);

        /* Genlist */
        genlist = create_genlist(nf,idx_start+1, idx_num-1); // do first item differently (next line)
        prepend_genlist(nf, genlist, OPENICE_ONOFF_IDX); // prepend on/off toggle
        nf_it = elm_naviframe_item_push(nf, "OpenICE-lite", NULL, NULL, genlist, NULL);
        elm_naviframe_item_pop_cb_set(nf_it, nf_it_pop_cb, ad);

        /* Show window after base gui is set up */
        evas_object_show(ad->win);
    }

     
    static bool
    app_create(void *data)
    {
        /* Hook to take necessary actions before main event loop starts
            Initialize UI resources and application's data
            If this function returns true, the main loop of application starts
            If this function returns false, the application is terminated */
        appdata_s *ad = data;

        create_base_gui(ad);
     
        return true;
    }
     
    static void
    app_control(app_control_h app_control, void *data)
    {
        /* Handle the launch request. */
    }
     
    static void
    app_pause(void *data)
    {
        /* Take necessary actions when application becomes invisible. */
    }
     
    static void
    app_resume(void *data)
    {
        /* Take necessary actions when application becomes visible. */
    }
     
    static void
    app_terminate(void *data)
    {
        /* Release all resources. */
    }
     

    static void
    ui_app_orient_changed(app_event_info_h event_info, void *user_data)
    {
        /*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
        return;
    }
     
    static void
    ui_app_region_changed(app_event_info_h event_info, void *user_data)
    {
        /*APP_EVENT_REGION_FORMAT_CHANGED*/
    }
     
    static void
    ui_app_low_battery(app_event_info_h event_info, void *user_data)
    {
        /*APP_EVENT_LOW_BATTERY*/
    }
     
    static void
    ui_app_low_memory(app_event_info_h event_info, void *user_data)
    {
        /*APP_EVENT_LOW_MEMORY*/
    }


    void
	gl_selected_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info)
	{
		Evas_Object *nf = (Evas_Object *)data, *objt;
		Elm_Object_Item *it = event_info;
		Item_Data *id = elm_object_item_data_get(it);
		int index = id->index;
		int idx_start = 0;
		int idx_num = 0;

		/* Unhighlight item */
		elm_genlist_item_selected_set(id->item, EINA_FALSE);

		switch(index) {
			case OPENICE_ONOFF_IDX:
				// Do nothing when this is pushed
				dlog_print(DLOG_INFO, LOG_TAG, "On/Off Section of List Pushed");
				break;
			case OPENICE_SETTINGS_IDX:
				idx_start = OPENICE_SETTING_MENU_START;
				idx_num = OPENICE_SETTING_MENU_NUM;
				objt = create_genlist(nf,idx_start, idx_num);
				elm_naviframe_item_push(nf, "Settings", NULL, NULL, objt, NULL);
				dlog_print(DLOG_INFO, LOG_TAG, "View Settings");
				break;
			case OPENICE_ABOUT_IDX:
				idx_start = OPENICE_ABOUT_MENU_START;
				idx_num = OPENICE_ABOUT_MENU_NUM;
				objt = create_genlist(nf,idx_start, idx_num);
				elm_naviframe_item_push(nf, "About OpenICE-lite", NULL, NULL, objt, NULL);
				dlog_print(DLOG_INFO, LOG_TAG, "View About");
				break;
			case OPENICE_CLIENT_ID_IDX:
				// Enter the client ID
				objt = create_entry(nf);
				elm_naviframe_item_push(nf, "Client ID", NULL, NULL, objt, NULL);
				dlog_print(DLOG_INFO, LOG_TAG, "Enter Client ID");
				break;
			case OPENICE_BROKER_IDX:
				// Enter the broker
				dlog_print(DLOG_INFO, LOG_TAG, "Enter Broker");
				break;
			case OPENICE_PASSWORD_IDX:
				// Enter the broker
				dlog_print(DLOG_INFO, LOG_TAG, "Enter Password");
				break;
			case OPENICE_PORT_NUM_IDX:
				// Enter the port number
				dlog_print(DLOG_INFO, LOG_TAG, "Enter Port Number");
				break;
			case OPENICE_NETWORK_PROTOCOL_IDX:
				// Select the network protocol
				dlog_print(DLOG_INFO, LOG_TAG, "Select Network Protocol");
				break;
			case OPENICE_CONNECTION_TIMEOUT_IDX:
				// Enter the connection timeout value
				dlog_print(DLOG_INFO, LOG_TAG, "Enter TimeOut");
				break;
			case OPENICE_KEEP_ALIVE_IDX:
				// Enter the keep alive value
				dlog_print(DLOG_INFO, LOG_TAG, "Enter Keep Alive Value");
				break;
			case OPENICE_USER_IDX:
				// Enter the broker
				dlog_print(DLOG_INFO, LOG_TAG, "View User ID");
				break;
			case OPENICE_SAMPLE_NUM:
				// Enter the broker
				dlog_print(DLOG_INFO, LOG_TAG, "View Packets per Sample");
				break;
			case OPENICE_SAMPLE_RATE:
				// Enter the broker
				dlog_print(DLOG_INFO, LOG_TAG, "View Sampling Rate");
				break;
			case OPENICE_DEV_INFO_IDX:
				// Enter the broker
				objt = create_label(nf, OPENICE_DEV_INFO);
				elm_naviframe_item_push(nf, "Development Information", NULL, NULL, objt, NULL);
				dlog_print(DLOG_INFO, LOG_TAG, "View Development Information");
				break;
			case OPENICE_DISCLAIMER_IDX:
				// Enter the broker
				objt = create_label(nf, OPENICE_DISCLAIMER);
				elm_naviframe_item_push(nf, "Disclaimer", NULL, NULL, objt, NULL);
				dlog_print(DLOG_INFO, LOG_TAG, "View Disclaimer");
				break;
			case OPENICE_LICENSE_IDX:
				// Enter the broker
				objt = create_label(nf, OPENICE_LICENSE);
				elm_naviframe_item_push(nf, "License Agreement", NULL, NULL, objt, NULL);
				dlog_print(DLOG_INFO, LOG_TAG, "View License");
				break;
			default:
				dlog_print(DLOG_INFO, LOG_TAG, "Received an unknown OPENICE menu index");
		}

	}




     
    int
    main(int argc, char *argv[])
    {
        appdata_s ad = {0,};
        int ret = 0;
     
        ui_app_lifecycle_callback_s event_callback = {0,};
        app_event_handler_h handlers[5] = {NULL, };
     
        event_callback.create = app_create;
        event_callback.terminate = app_terminate;
        event_callback.pause = app_pause;
        event_callback.resume = app_resume;
        event_callback.app_control = app_control;
     
        ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
        ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
        ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
        //ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
        ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
        ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);
     
        ret = ui_app_main(argc, argv, &event_callback, &ad);
        if (ret != APP_ERROR_NONE) {
            dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
        }
     
        return ret;
    }
