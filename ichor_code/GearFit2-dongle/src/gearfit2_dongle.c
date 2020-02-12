/*
 * gearfit2_dongle.c
 *
 *  Created on: Feb 18, 2019
 *      Author: James Weimer and Ben Chrepta
 */

#include <gearfit2_dongle.h>
#include <tizen.h>
#include <service_app.h>
#include <string.h>
#include <stdlib.h>
#include <device/power.h>

#include "paho-mqtt/MQTTClient.h"
#include <pthread.h>

#define MYSERVICELAUNCHER_APP_ID "com.precise.samdware.ui.gearfit2" // an ID of the UI application of our package
#define STRNCMP_LIMIT 256 // the limit of characters to be compared using strncmp function
#define QUEUE_SIZE 500
volatile int toStop = 0;


#include "paho-mqtt/log.h"


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}

volatile Ecore_Timer *timer1;

//sample: one data point
//packet: collection of data points

char message[5000] = {0,}; //buffer: stores HR sample
char accel[7500] = {0,};   //buffer: stores accelerometer sample
pthread_mutex_t hr_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t accel_mutex = PTHREAD_MUTEX_INITIALIZER;

int hrMessageIndex; //index for HR PACKET buffer
int accelMessageIndex; //index for ACCEL PACKET buffer
//a packet buffer stores multiple packets: prevents overrun

int sampleMax;      //maximum samples per packet allowed
bool runningThread; //if pthread is running
struct hrData //queue structure: HR
{
	long value;
	unsigned long long timestamp;
};

struct accelData //queue structure: accelerometer
{
	float xVal;
	float yVal;
	float zVal;
	unsigned long long timestamp;
};

struct hrData hrPacket[QUEUE_SIZE]; //reuseable packet for HR data
struct accelData accelPacket[QUEUE_SIZE]; //reusable packet for acceldata
int inPosition; //start index for hrPacket Queue: pushing data
int outPosition;//start index for hrPacket Queue: publishing data
int inPositionAc;//start index for accelPacket Queue: pushing data
int outPositionAc; //start index for accelPacket Queue: publishing data
int currHR; //current samples held within the queue (HR)
int currAc; //current samples held within the queue (Accel)

char ADDRESS[300] = {0,};
char USERNAME[300] = {0,};
char PASSWORD[300] = {0,};
int TIMEOUT;
int sampleRate;
int keepAlive;

// timer call back for testing
static Eina_Bool my_timed_cb(void *data)
{
	//dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);

    return ECORE_CALLBACK_RENEW;
}

void sensor_event_callback(sensor_h sensor, sensor_event_s *event, void *user_data)
{
	//dlog_print(DLOG_DEBUG, LOG_TAG, "callback reached");

	timer1 = ecore_timer_add(1.0, my_timed_cb, NULL);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);


    sensor_type_e type;
    sensor_get_type(sensor, &type); //getting sensor type
    if(type == SENSOR_ACCELEROMETER)
    {

		struct timespec spec;

		clock_gettime(CLOCK_REALTIME, &spec);

		unsigned long long current_time_ms = spec.tv_sec * 1000LL + spec.tv_nsec / 1000000LL; //getting current unix time
    	int rc = pthread_mutex_lock(&accel_mutex); //lock
    	if (rc) { /* an error has occurred */
    	    dlog_print(DLOG_ERROR, LOG_TAG, "Accel: pthread_mutex_lock");
    	    return;
    	}
    	if (currAc >= QUEUE_SIZE-1){ //the acceleration queue is attempting to overwrite
            dlog_print(DLOG_WARN, LOG_TAG, "Overwriting attempted for Accel! nothing done");

    	} else{
        	accelPacket[inPositionAc].xVal = event->values[0];
        	accelPacket[inPositionAc].yVal = event->values[1];
        	accelPacket[inPositionAc].zVal = event->values[2];
        	accelPacket[inPositionAc].timestamp = current_time_ms;  //set values and timestamp for inposition
    		inPositionAc++; //change inposition
        	currAc++;  		//increment amount of unsent items within queue
    		if(inPositionAc >= QUEUE_SIZE){
    			inPositionAc = 0; //have inposition wrap around if it reaches end of queue
    		}
    	    //dlog_print(DLOG_ERROR, LOG_TAG, "Accel index: %d currAC: %d", inPositionAc, currAc);

    	}
    	rc = pthread_mutex_unlock(&accel_mutex); //unlock
    	if (rc) {
    	    dlog_print(DLOG_ERROR, LOG_TAG, "Accel: pthread_mutex_unlock");
    	    return;
    	}

    } else if (type == SENSOR_HRM_LED_GREEN)
    {

		struct timespec spec;

		clock_gettime(CLOCK_REALTIME, &spec);

		unsigned long long current_time_ms = spec.tv_sec * 1000LL + spec.tv_nsec / 1000000LL; //getting current unix time
    	int rc = pthread_mutex_lock(&hr_mutex); //lock
    	if (rc) { /* an error has occurred */
    	    dlog_print(DLOG_ERROR, LOG_TAG, "HR: pthread_mutex_lock");
    	    return;
    	}
    	if(currHR >= QUEUE_SIZE-1){
            dlog_print(DLOG_WARN, LOG_TAG, "Overwriting attempted for HR! nothing done"); //the HR queue is attempting to overwrite
    	}else{
        	hrPacket[inPosition].value = event->values[0];
    		hrPacket[inPosition].timestamp = current_time_ms; //log HR value and timestamp into queue
    		inPosition++; //change in position
    		currHR++; // increment items within queue
    		if(inPosition >= QUEUE_SIZE){
    			inPosition = 0; //wraps around if the queue reaches the end
    		}
    	}
    	rc = pthread_mutex_unlock(&hr_mutex); //unlocks
    	if (rc) {
    	    dlog_print(DLOG_ERROR, LOG_TAG, "HR: pthread_mutex_unlock");
    	    return;
    	}

        //end here

    }
}

bool register_sensor(int sampling_rate)
{

	//dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);


	sensor_h sensorAccel; //accelerometer
	sensor_h sensorHR;    // HR sensor
	sensor_listener_h listenerAccel;
	sensor_listener_h listenerHR;

	//Starting sensor listener
	sensor_type_e type1 = SENSOR_ACCELEROMETER;
	sensor_type_e type2 = SENSOR_HRM_LED_GREEN; //Only shows phototransistor values
    bool supported;
    int error = sensor_is_supported(type2, &supported); //if green_LED is supported

    if (error != SENSOR_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG, "sensor_is_supported error: %d", error);
        return false;
    }

    if(supported){
    	dlog_print(DLOG_DEBUG, LOG_TAG, "HRM is%s supported", supported ? "" : " not");
    }


	if ((sensor_get_default_sensor(type1, &sensorAccel) == SENSOR_ERROR_NONE) &&  (sensor_get_default_sensor(type2, &sensorHR) == SENSOR_ERROR_NONE)) //Gets a specific sensor handle
	{

	    if ((sensor_create_listener(sensorAccel, &listenerAccel) == SENSOR_ERROR_NONE && sensor_listener_set_event_cb(listenerAccel, 100, sensor_event_callback, NULL) == SENSOR_ERROR_NONE) && (sensor_create_listener(sensorHR, &listenerHR) == SENSOR_ERROR_NONE && sensor_listener_set_event_cb(listenerHR, 100, sensor_event_callback, NULL) == SENSOR_ERROR_NONE))
	    {
	    	int error = sensor_listener_set_interval(listenerHR, sampling_rate);
	    	int error2 = sensor_listener_set_interval(listenerAccel, sampling_rate); //sets listener callback as well as sampling interval
	    	if (error != SENSOR_ERROR_NONE || error2 != SENSOR_ERROR_NONE) {
	            dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_set_interval error: %d", error);
	   	        return false;
	   	    }
	    	dlog_print(DLOG_DEBUG, LOG_TAG, "sensor_listener_set_intervals");

	        error = sensor_listener_set_option(listenerHR, SENSOR_OPTION_ALWAYS_ON);
	   	    if (error != SENSOR_ERROR_NONE) {
	   	        dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_set_option error: %d", error);
	   	        return false;
	   	    }

	        if ((sensor_listener_start(listenerAccel) == SENSOR_ERROR_NONE) && (sensor_listener_start(listenerHR) == SENSOR_ERROR_NONE)){ //Starts the sensor server for the given listener.
	        	dlog_print(DLOG_DEBUG, "sensor","sensor1 and sensor2 listener started!");
	        }
	        sensor_event_s event;
	        error = sensor_listener_read_data(listenerHR, &event);    //reading data
	        error2 = sensor_listener_read_data(listenerAccel, &event); //reading data
	        if (error != SENSOR_ERROR_NONE) {
	            dlog_print(DLOG_ERROR, LOG_TAG, "sensor_listener_read_data error: %d", error);
	            return false;
	        }
	    }
	}
    return true;
}

void service_app_terminate(void *dataIn)
{
	//dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);

	ecore_timer_del(timer1);

}


void *pubMessage(void *thing){
	char *msg;
	int rc;
	msg = (char *) thing; //this is useless

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token; //MQTTClient arguments

	dlog_print(DLOG_DEBUG, LOG_TAG, "the address %s", ADDRESS);
    rc = MQTTClient_create(&client, ADDRESS, "jim", MQTTCLIENT_PERSISTENCE_NONE, NULL); //create client
    conn_opts.keepAliveInterval = keepAlive;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD; //connection options
	//dlog_print(DLOG_DEBUG, LOG_TAG, "return val %d", rc);
	char hrMessages[50][5000];
	char accMessages[50][5000]; //buffers for holding packets
	while(runningThread){
		char message[5000] = {0,};
		char accel[5000] = {0,}; //actual buffers for holding raw sensor data
		struct timespec spec;
		clock_gettime(CLOCK_REALTIME, &spec);

		unsigned long long current_time_ms = spec.tv_sec * 1000000LL + spec.tv_nsec / 1000LL; //getting current unix time
		int rc = pthread_mutex_lock(&hr_mutex); //lock
		if (rc) { /* an error has occurred */
			dlog_print(DLOG_ERROR, LOG_TAG, "HR: pthread_mutex_lock");
			pthread_exit(NULL);
		}
		if(currHR >= sampleMax){ //if there are enough items to make one packet
			while(currHR >= sampleMax){
				char buf[100];
				if(hrMessageIndex >= 50){ //if there are too many packets
					dlog_print(DLOG_DEBUG, LOG_TAG, "Message holding buffer too full!");
				} else{
					for(int i = 0; i < sampleMax; i++){
						sprintf(buf, "%ld %llu \n", hrPacket[outPosition].value, hrPacket[outPosition].timestamp); //get the sensor value/timestamp at the specific position
						strcat(message, buf); //add to packet
						outPosition++; //move to the next sample
						currHR--; //decrement the amount of samples to be processed
						if(outPosition >= QUEUE_SIZE){
							outPosition = 0; //wrap around queue
						}
					}
					strcpy(hrMessages[hrMessageIndex], message); //add to the packet buffer
					hrMessageIndex++; //increment packet buffer
					strcpy(message, "");
				}
			}
		}
		rc = pthread_mutex_unlock(&hr_mutex); //unlock
		clock_gettime(CLOCK_REALTIME, &spec);

		unsigned long long after_time_ms = spec.tv_sec * 1000000LL + spec.tv_nsec / 1000LL; //getting current unix time
		dlog_print(DLOG_ERROR, LOG_TAG, "HR DIFF: %llu", after_time_ms-current_time_ms);
		if (rc) { /* an error has occurred */
			dlog_print(DLOG_ERROR, LOG_TAG, "HR: pthread_mutex_unlock");
			pthread_exit(NULL);
		}

		if(hrMessageIndex >0){ //if there is at least one packet ready to go
			if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
				dlog_print(DLOG_DEBUG, LOG_TAG, "HR Failed to connect, return code %d\n", rc);
				exit(EXIT_FAILURE);
			}
			for(int i = 0; i< hrMessageIndex; i++){
				pubmsg.payload= hrMessages[i]; //set message
				pubmsg.payloadlen = (int)strlen(hrMessages[i]); //length
				pubmsg.qos = 1; //QoS 1
				pubmsg.retained = 0;
				MQTTClient_publishMessage(client, HR, &pubmsg, &token); //publish
				rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
				dlog_print(DLOG_DEBUG, LOG_TAG, "rc for HR: %d", rc); //result
			}

			MQTTClient_disconnect(client, 10000); //disconnect
			hrMessageIndex = 0; //reset packet buffer index
		}


		rc = pthread_mutex_lock(&accel_mutex); //same thing for accel
		if (rc) { /* an error has occurred */
			dlog_print(DLOG_ERROR, LOG_TAG, "Acc: pthread_mutex_lock");
			pthread_exit(NULL);
		}
		if(currAc >= sampleMax){ //if there are enough items to make one packet
			while(currAc >= sampleMax){
				char buf[100];
				if(accelMessageIndex >= 50){ //checks to see if packet buffer is too full
					dlog_print(DLOG_ERROR, LOG_TAG, "Acc is too full!");
				} else{
					for(int i = 0; i < sampleMax; i++){
						sprintf(buf, "%f, %f, %f %llu \n",  accelPacket[outPositionAc].xVal,  accelPacket[outPositionAc].yVal,  accelPacket[outPositionAc].zVal, accelPacket[outPositionAc].timestamp);
						strcat(accel, buf);
						outPositionAc++;
						currAc--;
						if(outPositionAc >= QUEUE_SIZE){
							outPositionAc = 0;
						}
					}
					strcpy(accMessages[accelMessageIndex], accel);
					accelMessageIndex++;
					dlog_print(DLOG_DEBUG, LOG_TAG, "acindex: %d", accelMessageIndex);
			    	strcpy(accel, "");
				}
	    	}
		}
		rc = pthread_mutex_unlock(&accel_mutex);
		if (rc) { /* an error has occurred */
	    	dlog_print(DLOG_ERROR, LOG_TAG, "ac: pthread_mutex_unlock");
	    	pthread_exit(NULL);
		}

		if(accelMessageIndex >0){
	    	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
	    		dlog_print(DLOG_DEBUG, LOG_TAG, "accel Failed to connect, return code %d\n", rc);
	        	exit(EXIT_FAILURE);
	    	}
	    	for(int i = 0; i< accelMessageIndex; i++){
	    		pubmsg.payload= accMessages[i];
	    		pubmsg.payloadlen = (int)strlen(accMessages[i]);
	    		pubmsg.qos = 1;
	    		pubmsg.retained = 0;
		    	MQTTClient_publishMessage(client, ACCEL, &pubmsg, &token);
		    	rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
		    	dlog_print(DLOG_DEBUG, LOG_TAG, "rc for Ac: %d", rc);
	    	}

	    	MQTTClient_disconnect(client, 10000);
	    	accelMessageIndex = 0;
		}
	//sleep(1);
	}
    MQTTClient_destroy(&client); //destroy client
    dlog_print(DLOG_INFO, LOG_TAG, "Thread Destroyed");

   return NULL; //end
}
void service_app_control(app_control_h app_control, void *data)
{
    char *caller_id = NULL, *action_value, *maybeData = NULL;
    char **testor =  NULL;
	pthread_t pub;

    int i;
    /*app_control_get_extra_data(app_control, "config_data", &maybeData);
	dlog_print(DLOG_DEBUG, LOG_TAG, "its a %s also ", maybeData);
	*/
	app_control_get_extra_data_array(app_control, "config_data", &testor, &i); //from UI
	if(testor[0] != NULL){
		strcpy(ADDRESS, testor[0]); //ADDRESS
		strcpy(USERNAME, testor[1]); //USERNAME
		strcpy(PASSWORD, testor[2]); //PASSWORD
		caller_id = testor[6];
		TIMEOUT = atoi(caller_id);
		caller_id = testor[7]; //TIMEOUT
		sampleMax = atoi(caller_id);
		caller_id = testor[8]; //sampleMax (maximum amount of samples per packet)
		keepAlive = atoi(caller_id); // keep alive
		caller_id = testor[9];
		sampleRate = atoi(caller_id); //sampling rate

	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "address should be %s", ADDRESS);
	register_sensor(sampleRate); //registers sensors
	pthread_create(&pub, NULL, pubMessage, (void *) maybeData); //calls publishing pThread

    if ((app_control_get_caller(app_control, &caller_id) == APP_CONTROL_ERROR_NONE)
        && (app_control_get_extra_data(app_control, "service_action", &action_value) == APP_CONTROL_ERROR_NONE))
    {

        if((caller_id != NULL) && (action_value != NULL)
            && (!strncmp(caller_id, MYSERVICELAUNCHER_APP_ID, STRNCMP_LIMIT))
            && (!strncmp(action_value,"stop", STRNCMP_LIMIT)))
        {
            dlog_print(DLOG_INFO, LOG_TAG, "Stopping MyService!");
            runningThread = false;
            free(caller_id);
            free(action_value);
            service_app_exit();
            return;
        }
        else
        {
            dlog_print(DLOG_INFO, LOG_TAG, "Unsupported action! Doing nothing...");
            free(caller_id);
            free(action_value);
            caller_id = NULL;
            action_value = NULL;
        }
    }
}

bool service_app_create(void *data){
	return true;
}


static void
service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);

	/*APP_EVENT_LANGUAGE_CHANGED*/


	return;
}

static void
service_app_region_changed(app_event_info_h event_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
service_app_low_battery(app_event_info_h event_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);
	/*APP_EVENT_LOW_BATTERY*/
}

static void
service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "%s", __func__);
	/*APP_EVENT_LOW_MEMORY*/
}




int main(int argc, char* argv[])
{
	char *mes = "H";
	runningThread = true;
	inPosition = 0;
	outPosition = 0;
	inPositionAc = 0;
	outPositionAc = 0;
	currHR = 0;
	currAc= 0;
	hrMessageIndex = 0;
	accelMessageIndex = 0; //sets all environment variables to 0
	//pthread_create(&pub, NULL, pubMessage, (void *) mes);


    char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = service_app_create;

	event_callback.terminate = service_app_terminate;

	event_callback.app_control = service_app_control;


	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);
	//all handlers were previously commented out
	return service_app_main(argc, argv, &event_callback, ad);
}



