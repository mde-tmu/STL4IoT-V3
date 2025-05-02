#include "Arduino.h"
#include "esp32-hal-gpio.h"

#include "iot-sc-template-library/src-gen/SmartLightSystem.h"
#include "iot-sc-template-library/src-gen/Sensor_Component.h"
#include "iot-sc-template-library/src-gen/LEDLight_Unit.h"
#include "iot-sc-template-library/src-gen/Actuator_Component.h"
#include "iot-sc-template-library/src-gen/Controller_Component.h"
#include "iot-sc-template-library/src-gen/Actuator_Component_2.h"
#include "iot-sc-template-library/src-gen/Controller_Component_2.h"
#include "iot-sc-template-library/src-gen/DeviceTemp_Component.h"
#include "iot-sc-template-library/src-gen/Network_Component.h"
#include "iot-sc-template-library/src-gen/Power_Component.h"
#include "iot-sc-template-library/src-gen/SmartGarageDoorSystem.h"
#include "iot-sc-template-library/src-gen/GarageDoor_Unit.h"
#include "iot-sc-template-library/src-gen/Ultrasonic_Sensor.h"
#include "iot-sc-template-library/src-gen/Ultrasonic_MotionDetector.h"
#include "iot-sc-template-library/src-gen/SmartFireSystem.h"
#include "iot-sc-template-library/src-gen/Sensor_Component_Fire.h"
#include "iot-sc-template-library/src-gen/FireAlarm_Unit.h"
#include "timer_functions.h"


static SmartLightSystem smartLight;
static SmartGarageDoorSystem smartGarageDoor;
static SmartFireSystem smartFire;


static Actuator_Component_2 smartFireActuator;
static Sensor_Component_Fire flameSensor;
static Sensor_Component_Fire smokeSensor;
static Sensor_Component_Fire heatSensor;

static FireAlarm_Unit smartFireAlarm;


static Actuator_Component_2 smartGarageDoorActuator;
static Controller_Component_2 smartGarageDoorController;
static Ultrasonic_Sensor smartGarageDoorSensor;
static Ultrasonic_MotionDetector smartGarageDoorMotionDetector;
static GarageDoor_Unit smartGarageDoorUnit;

static LEDLight_Unit light;
static Sensor_Component sensor;
static Actuator_Component actuator;
static Controller_Component controller;
static DeviceTemp_Component deviceTemp;
static Network_Component network;
static Power_Component power;
//static Ultrasonic_MotionDetector ultrasonic;



//The setup function is called once at startup of the sketch
const int ledPin = 26;  // LED pin
const int sensorPin = 23;  // Sensor pin

const int ultrasoundSensorPin = 18;
const int garageDoorPin = 25;


// Garage Door
#define SERVO_PIN 25
#define TRIG_PIN 5
#define ECHO_PIN 18
#define BUTTON_DOOR 16

// Fire Alarm
const int flameSensorPin = 13;
const int smokeSensorPin = 34;
//const int heatSensorPin = ;
const int buzzerPin = 21;

const int sensorThreshold = 600;


void checkSensor(){
	Serial.println("Checking sensor");


	int garage_door_ultrasound = analogRead(ultrasoundSensorPin);
	Serial.print("Garage Door Sensor Value: ");
	Serial.println(garage_door_ultrasound);

	ultrasonic_MotionDetector_set_sensorData(smartGarageDoorSystem_get_sensor(&smartGarageDoor), garage_door_ultrasound);
	// Send to Firebase
	


	//Smart Fire

	int flame = analogRead(flameSensorPin);
	Serial.print("Flame Sensor Value: ");
	Serial.println(flame);
	sensor_Component_Fire_set_environmentData(smartFireSystem_get_flameSensor(&smartFire), flame);

	int smoke = analogRead(smokeSensorPin);
	Serial.print("Smoke Sensor Value: ");
	Serial.println(smoke);
	sensor_Component_Fire_set_environmentData(smartFireSystem_get_smokeSensor(&smartFire), smoke);

//	int heat = analogRead(heatSensorPin);
//	Serial.print("Heat Sensor Value: ");
//	Serial.println(heat);
//	sensor_Component_Fire_set_environmentData(smartFireSystem_get_heatSensor(&smartFire), heat);




	int light_intensity = analogRead(sensorPin);
	Serial.print("Sensor Value: ");
	Serial.println(light_intensity);
//	ultrasonic_MotionDetector_set_sensorData(smartLightSystem_get_motionDetector(&smartLight), light_intensity);
	// Send to Firebase

	Serial.print("UPLOADED DATA: ");
//	Serial.println(ultrasonic_MotionDetector_set_sensorData(smartLightSystem_get_motionDetector(&smartLight)));

	Serial.print("PRINT LIGHT DEVICE STATUS: ");
	Serial.println(lEDLight_Unit_device_get_isOn(smartLightSystem_get_lights(&smartLight)));

	Serial.print("PRINT LIGHT BULB STATUS: ");
	Serial.println(lEDLight_Unit_Bulb_get_isOn(smartLightSystem_get_lights(&smartLight)));
}

void fireAlarm(){
	Serial.println("Checking fire alarm");
	if (actuator_Component_2_get_isTriggered(smartFireSystem_get_actuator(&smartFire))) {
		digitalWrite(buzzerPin, HIGH);
		// Send to Firebase
	} else {
		digitalWrite(buzzerPin, LOW);
		// Send to Firebase
	}
}

void garageDoor(){
	Serial.println("Checking garage door");
	if (garageDoor_Unit_system_get_door_status(smartGarageDoorSystem_get_garageDoor(&smartGarageDoor)) == "Door Opened" || garageDoor_Unit_system_get_door_status(smartGarageDoorSystem_get_garageDoor(&smartGarageDoor)) == "Door Opening") {
		digitalWrite(garageDoorPin, HIGH);
		// Send to Firebase
	} else {
		digitalWrite(garageDoorPin, LOW);
		// Send to Firebase
	}
}

void lights(){
	Serial.println("Checking lights");
	if (lEDLight_Unit_Bulb_get_isOn(smartLightSystem_get_lights(&smartLight))) {
		digitalWrite(ledPin, HIGH);
		// Send to Firebase
	} else {
		digitalWrite(ledPin, LOW);
		// Send to Firebase
	}
}

void checkFirebase() {
	//Get from firebase
	// read firebase if off
	// if firebase is on => turn on
	// if firebase if off => turn off
}


void setup()
{
	Serial.begin(9600);
	pinMode(ledPin, OUTPUT);
	pinMode(sensorPin, INPUT);

	lEDLight_Unit_init(&light);
	sensor_Component_init(&sensor);
	deviceTemp_Component_init(&deviceTemp);
	power_Component_init(&power);
	network_Component_init(&network);
	controller_Component_init(&controller);
	actuator_Component_init(&actuator);


	smartFireSystem_init(&smartFire);
	actuator_Component_2_init(&smartFireActuator);
	sensor_Component_Fire_init(&flameSensor);
	sensor_Component_Fire_init(&smokeSensor);
	sensor_Component_Fire_init(&heatSensor);
	fireAlarm_Unit_init(&smartFireAlarm);

	smartFireSystem_set_actuator(&smartFire, &smartFireActuator);
	smartFireSystem_set_flameSensor(&smartFire, &flameSensor);
	smartFireSystem_set_smokeSensor(&smartFire, &smokeSensor);
	smartFireSystem_set_heatSensor(&smartFire, &heatSensor);
	smartFireSystem_set_wiFi(&smartFire, &network);
	smartFireSystem_set_power(&smartFire, &power);
	smartFireSystem_set_temp(&smartFire, &deviceTemp);
	smartFireSystem_set_fireAlarm(&smartFire, &smartFireAlarm);


	smartGarageDoorSystem_init(&smartGarageDoor);
	actuator_Component_2_init(&smartGarageDoorActuator);
	controller_Component_2_init(&smartGarageDoorController);
	ultrasonic_Sensor_init(&smartGarageDoorSensor);
	ultrasonic_MotionDetector_init(&smartGarageDoorMotionDetector);
	garageDoor_Unit_init(&smartGarageDoorUnit);


	smartGarageDoorSystem_set_actuator(&smartGarageDoor, &smartGarageDoorActuator);
	smartGarageDoorSystem_set_controller(&smartGarageDoor, &smartGarageDoorController);
	smartGarageDoorSystem_set_sensor(&smartGarageDoor, &smartGarageDoorMotionDetector);
	smartGarageDoorSystem_set_wiFi(&smartGarageDoor, &network);
	smartGarageDoorSystem_set_power(&smartGarageDoor, &power);
	smartGarageDoorSystem_set_temp(&smartGarageDoor, &deviceTemp);
	smartGarageDoorSystem_set_garageDoor(&smartGarageDoor, &smartGarageDoorUnit);


	smartGarageDoorSystem_enter(&smartGarageDoor);
//	ultrasonic_MotionDetector_init(&ultrasonic);

	smartLightSystem_init(&smartLight); //initialize statechart



	smartLightSystem_set_actuator(&smartLight, &actuator);
	smartLightSystem_set_controller(&smartLight, &controller);
	smartLightSystem_set_wiFi(&smartLight, &network);
	smartLightSystem_set_power(&smartLight, &power);
	smartLightSystem_set_temp(&smartLight, &deviceTemp);
	smartLightSystem_set_lights(&smartLight, &light);
//	smartLightSystem_set_motionDetector(&smartLight, &ultrasonic);

	smartLightSystem_enter(&smartLight); // Enter the statechart
	smartLightSystem_system_raise_on(&smartLight);
	// Send Firebase

//	sensor_Component_set_sensorThreshold(smartLightSystem_get_sensor(&smartLight), sensorThreshold);

}

// The loop function is called in an endless loop
void loop()
{
	Serial.println("Checking sensor");

	int light_intensity = analogRead(sensorPin);

	Serial.print("sensor value: ");
	Serial.println(light_intensity);

	if(light_intensity > sensorThreshold){
		Serial.println("Light ON");
//		lEDLight_Unit_device_raise_on(SmartLightSystem_get_lights(&smartLight));
		digitalWrite(ledPin, HIGH);
	} else {
		Serial.println("Light OFF");
		digitalWrite(ledPin, LOW);
//		lEDLight_Unit_device_raise_off(SmartLightSystem_get_lights(&smartLight));
	}
	checkSensor();
	lights();
	garageDoor();
	fireAlarm();
	delay(500);
}
