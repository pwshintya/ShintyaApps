#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_TASK_HANDLER
#define ENABLE_MODULE_TIMER_DURATION
#define ENABLE_MODULE_TIMER_TASK
#define ENABLE_MODULE_SERIAL_HARD
#define ENABLE_MODULE_DATETIME_NTP_V2
#define ENABLE_MODULE_FIREBASE_RTDB_V2
#define ENABLE_MODULE_FIREBASE_FIRESTORE_V2
#define ENABLE_MODULE_FIREBASE_MESSAGING_V2

#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_ANALOG

#include "Kinematrix.h"
#include "Preferences.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"

////////// Utility //////////
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // Offset for WIB (UTC+7)
const int daylightOffset_sec = 0;

DateTimeNTPV2 dateTime(ntpServer, gmtOffset_sec, daylightOffset_sec);
TaskHandle task;
Preferences preferences;
FirebaseV2RTDB firebase;
FirebaseV2Firestore firestore;
FirebaseV2Messaging messaging;
WiFiClientSecure client;

////////// Sensor //////////
SensorModule sensor;

////////// Communication //////////
HardSerial usbSerial;

////////// Input Module //////////
DigitalIn buttonDown(-1);
DigitalIn buttonOk(-1);

////////// Output Module //////////
DigitalOut buzzer(LED_BUILTIN);

////////// Global Variable //////////
bool firebaseEnable = false;

enum FirebaseState {
  FIRESTORE_IDE,
  FIRESTORE_CREATE,
  FIRESTORE_READ,
  FIRESTORE_UPDATE,
  FIRESTORE_DELETE,
};

int firebaseFirestoreState = FIRESTORE_IDE;
int firebaseMessagingState = 0;