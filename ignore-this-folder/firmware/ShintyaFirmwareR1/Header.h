#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_TASK_HANDLER
#define ENABLE_MODULE_TIMER_DURATION
#define ENABLE_MODULE_TIMER_TASK
#define ENABLE_MODULE_SERIAL_HARD
#define ENABLE_MODULE_DATETIME_NTP_V2
#define ENABLE_MODULE_LCD_MENU
#define ENABLE_MODULE_SERVO_HCPA9685
#define ENABLE_MODULE_PCF8574_INPUT_MODULE
#define ENABLE_MODULE_PCF8574_MODULE
#define ENABLE_MODULE_PCF8574_OUTPUT_MODULE

#define ENABLE_MODULE_FIREBASE_APPLICATION_V3
#define ENABLE_MODULE_FIREBASE_RTDB_V3
#define ENABLE_MODULE_FIREBASE_FIRESTORE_V3
#define ENABLE_MODULE_FIREBASE_MESSAGING_V3

#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_ULTRASONIC
#define ENABLE_SENSOR_GM67
#define ENABLE_SENSOR_KEYPADI2C

#include "Kinematrix.h"
#include "Preferences.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "DFRobotDFPlayerMini.h"

#define JUST_TESTING 0

////////// Utility //////////
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // Offset for WIB (UTC+7)
const int daylightOffset_sec = 0;

DateTimeNTPV2 dateTime(ntpServer, gmtOffset_sec, daylightOffset_sec);
TaskHandle task;
Preferences preferences;
FirebaseV3RTDB* firebase = nullptr;
FirebaseV3Firestore* firestore = nullptr;
FirebaseV3Messaging* messaging = nullptr;
WiFiClientSecure client;

////////// Sensor //////////
SensorModule sensor;

////////// Communication //////////
HardSerial usbSerial;

////////// Input Module //////////
DigitalIn buttonDown(36);
DigitalIn buttonOk(39);
#if JUST_TESTING
PCF8574Module pcfModuleA(0x21);
PCF8574Module pcfModuleB(0x22);

PCF8574DigitalIn limitSwitch1(PCF_PIN0);
PCF8574DigitalIn limitSwitch2(PCF_PIN1);
PCF8574DigitalIn limitSwitch3(PCF_PIN2);
PCF8574DigitalIn limitSwitch4(PCF_PIN3);
PCF8574DigitalIn limitSwitch5(PCF_PIN4);
PCF8574DigitalIn limitSwitch6(PCF_PIN5);
PCF8574DigitalIn limitSwitch7(PCF_PIN6);
PCF8574DigitalIn limitSwitch8(PCF_PIN7);
PCF8574DigitalIn limitSwitch9(PCF_PIN0);
PCF8574DigitalIn limitSwitch10(PCF_PIN1);
PCF8574DigitalIn limitSwitch11(PCF_PIN2);
PCF8574DigitalIn limitSwitch12(PCF_PIN3);
#endif

////////// Output Module //////////
DigitalOut buzzer(4);
DigitalOut ledRed(LED_BUILTIN);  // 5
DigitalOut ledGreen(18);
DigitalOut ledYellow(19);
DigitalOut relayA(33);
DigitalOut relayB(32);

LcdMenu menu(0x27, 20, 4);
#if JUST_TESTING
HCPCA9685 servoDriver(0x40);
DFRobotDFPlayerMini mp3Player;
#endif

////////// Global Variable //////////
bool firebaseEnable = false;

enum FirebaseRTDBState {
  RTDB_IDLE,
  RTDB_SET_VALUE,
  RTDB_SET_VALUE_JSON,
  RTDB_SET_VALUE_PERIODIC,
  RTDB_GET_VALUE,
  RTDB_GET_VALUE_JSON,
  RTDB_GET_VALUE_PERIODIC,
};

enum FirebaseFirestoreState {
  FIRESTORE_IDE,
  FIRESTORE_CREATE,
  FIRESTORE_READ,
  FIRESTORE_UPDATE,
  FIRESTORE_DELETE,
};

enum FirebaseMessagingState {
  MESSAGING_IDLE,
  MESSAGING_SEND,
};

FirebaseRTDBState firebaseRTDBState = RTDB_IDLE;
FirebaseFirestoreState firebaseFirestoreState = FIRESTORE_IDE;
FirebaseMessagingState firebaseMessagingState = MESSAGING_IDLE;

String buttonDownStr = "";
String buttonOkStr = "";

const uint8_t PAKET_MAX = 5;
struct ResiData {
  String nama;
  String noResi;
  String packetType;
  int resiId;
};

const uint8_t USER_MAX = 20;
struct UserData {
  String displayName;
  String name;
  String email;
};

ResiData resiData[PAKET_MAX];
UserData userData[USER_MAX];

String resiBarcode = "";
String userQRCode = "";
String statusTinggiPaket = "";
int tinggiPaket = 0;
bool ambilPaketState = false;