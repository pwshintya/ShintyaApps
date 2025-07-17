#define FIREBASE_DATABASE_URL "https://shintya-b7e78-default-rtdb.firebaseio.com/"
#define FIREBASE_PROJECT_ID "shintya-b7e78"
#define FIREBASE_API_KEY "AIzaSyCI2bkv6iIel9WJv0Mx53pZ1h9bL7lhsZw"
#define FIREBASE_USER_EMAIL "iwan.dwp@gmail.com"
#define FIREBASE_USER_PASSWORD "admin123"
#define FIREBASE_CLIENT_EMAIL "firebase-adminsdk-fbsvc@shintya-b7e78.iam.gserviceaccount.com"
#define FIREBASE_MSG_DEVICE_TOKEN "cJnjCBzORlawc7T2WvCq2L:APA91bEyoA65YjDAEU6Y_Mj6DQzw5KH_Svfs7ZoLv3Vdl-ZurpiN8BGi1R3qaOh1Ux_wNHacMHSGOfHuxxKQraLcWC-RowpmEvPQboZasgsWJQ_MWdS285Q"
const char FIREBASE_PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQDhOh4YETBWsmRe\nxdGbfViM1JE10QcFdBIAV8TFZnqE046b+ZcLsR4u+nC104vPVB+roiQ4ao2lZRgo\n2Joz1T+cV8YJYhjcvJBDeD3LfGNAhZXuOzUsdvtBBfEqSsbHYaTobVGwC8LNT2m5\nf2nvJGCByIMZHRbju3TwWzxqsnf6QJTBAz1Ry/bgk+Ei2BPSfzH9pQjTjP45QoJO\noPkPOn2Tji+4hghkGQCi6goNDt9AQz7Tg662Dg2uWcu9yVU/J8ysoWbz9q5fL7d6\n/8yday6KRSK0tl7oLnZiJ/duoylcY7qvy701gN2VnUqvDX2v57tpcnHIFOr6ACWl\nLpGkkIJNAgMBAAECggEAAYHIisOurZ3S8xliviYYDhUsBY6o+93bcvojXGjDoBjk\nRuHkI1VjjM/rKiRItrHq5lSW5nO8D+O9xiQw3/Do1Ix3zD9GSE/QrUrkKvThmyBp\nsObfg7UmGDjIdM58RwA9wLpehg2GF6aU4ANcu5dkN+oWCnh52zs5XJRXsqUQDXIW\n9htedSuD4B3nmTsPGBpaZ/41xncTVREPodj0i7Z4yt+hh+yVyf2mX1J6DOoS9EY7\nXgyzqdazwtekYXN5iCoRId1mRvSC6oG52FrX5EzkttSPMkSwDfwmoTMsVseDOVdv\nykkgRRLgJSGmiOxfMhUV+TPWJTtaxjXzePB4q2jjxwKBgQDwYMEK3ucX7cfQrl5z\nRqK2rn1zq1/oceNrhstunDbYm/ebKWrGX0tTBhA+FoWhLZ85hd4mHLZwyUTGfUIM\nEeemxc8p9lMQ3CngHmAATIjRI1Ffj3W3FUOt73MuxtcfIjEa+UzJbrayc65ym0nE\nVDyIq1GuJSDW50FiDQWfEE53iwKBgQDv3UrqnhyZGzVQ4wPM0j2uMLqLtrZl+C6E\nTE6+T2RC+kKRUNz1NCCoqqiKDmZKa9rQ41pjy6DSlEf5OsZY28BpqctxCNrcQriS\nt5aa5nbvg/ugzSpIRTk9AUVA8px1JkEkZkVqLEmaWA37EAY1cbBrfuVN9Y/zojsC\nIDha271ohwKBgQDcnpDO32KOjciU+VTqzGvO6zttDF9MQisOp3rTHEEicZPCLzM2\nuoOxwaDqu1UOZ4b6DqmjpTq1LaUX4CLcXRqV8HuA8fmvQcKVVouZ+qSf/qBS6qKJ\n1NR3MYwv5IPQqVsUfLdDMYAxLQdxXKFJtl6QuVyQTrdjRoC182mzYg4CnQKBgQCU\nySPLXMBIiIQN5INa7Z6tCjTsrS1/Gu9fySWfDwgWM75O/WXSA7+uYA2jdUMmLrjQ\nkfZjqM/dT3VFt5uJSuaTpGs5dlCsCAcNmyukcgickbXoDNuJcMcMfFlE7Dj4wf3u\nYjH0pfwR7UTe0xXG2Pqh81ixfv0IFz2bF/ldAqj+yQKBgQCVVFIxakgwIUK/11fj\nITv3i9mzAYGIkZClIlZfgvfqEI5IES1Ec1buVQotHlpEwNkGzFzo26PRkixk/ZRw\ndgQMJJ7laYYC5YS2vlOZceqLyF34jGHshnu95SGn+WQML+Qg2nsFv8hlutyHIX5t\nRjj8WLer+Qar1H1gl9pAFybxtQ==\n-----END PRIVATE KEY-----\n";

void wifiTask() {
  task.setInitCoreID(1);
  task.createTask(10000, [](void* pvParameter) {
    WiFi.begin("TIMEOSPACE", "1234Saja");
    // WiFi.begin("silenceAndSleep", "11111111");
    while (WiFi.status() != WL_CONNECTED) {
      ledRed.toggle();
      delay(50);
    }
    Serial.println("IP: " + WiFi.localIP().toString());
    client.setInsecure();

    if (!dateTime.begin()) {
      Serial.println("Gagal memulai NTP Client!");
    }

    FirebaseV3Application::getInstance()->setTime(dateTime.now());
    if (!FirebaseV3Application::getInstance()->begin(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, FIREBASE_PRIVATE_KEY)) {
      Serial.println("Failed to initialize Firebase Application");
      while (1) { delay(1000); }
    }

    firebase = new FirebaseV3RTDB(FirebaseV3Application::getInstance());
    firestore = new FirebaseV3Firestore(FirebaseV3Application::getInstance());
    messaging = new FirebaseV3Messaging(FirebaseV3Application::getInstance());

    firebase->begin(FIREBASE_DATABASE_URL);
    firestore->begin(FIREBASE_PROJECT_ID);
    messaging->begin(FIREBASE_PROJECT_ID);

    Serial.println("Firebase Init Success");

    disableCore1WDT();
    buzzer.toggleInit(100, 2);

    JsonDocument documentData;
    String documentDataStr;
    String resultStr;

    for (;;) {
      FirebaseV3Application::getInstance()->loop();
      if (firebase) firebase->loop();
      if (firestore) firestore->loop();
      if (messaging) messaging->loop();

      if (!firebase->ready() || !firestore->isReady()) ledRed.toggleAsync(150);
      else ledRed.on();

      static uint32_t dateTimeNTPTimer;
      if (millis() - dateTimeNTPTimer >= 1000 && dateTime.update()) {
        // Serial.println(dateTime.getDateTimeString());
        dateTimeNTPTimer = millis();
      }

      static uint32_t firebaseRTDBTimer;
      if (millis() - firebaseRTDBTimer >= 2000 && firebase->ready()) {  // FIREBASE_RTDB_START
        // firebase->set("/test/float", random(100) + 3.14159, 2);
        firebaseRTDBTimer = millis();
      }  // FIREBASE_RTDB_END

      static uint32_t firebaseFirestoreTimer;
      if (millis() - firebaseFirestoreTimer >= 5000 && firestore->isReady() && FirebaseV3Application::getInstance()->isReady()) {
        firebaseFirestoreTimer = millis();

        String userDataResultStr = firestore->getDocument("users", "", true);
        String resiDataResultStr = firestore->getDocument("resiData", "", true);

        JsonDocument userDataDocument;
        JsonDocument resiDataDocument;
        deserializeJson(userDataDocument, userDataResultStr);
        deserializeJson(resiDataDocument, resiDataResultStr);

        uint8_t userDataIndex = 0;
        uint8_t resiDataIndex = 0;

        for (JsonVariant fields : userDataDocument["documents"].as<JsonArray>()) {
          if (userDataIndex < USER_MAX) {
            userData[userDataIndex].displayName = fields["fields"]["displayName"]["stringValue"].as<String>();
            userData[userDataIndex].name = fields["fields"]["name"]["stringValue"].as<String>();
            userData[userDataIndex].email = fields["fields"]["email"]["stringValue"].as<String>();
            userDataIndex++;
          }
        }

        for (JsonVariant fields : resiDataDocument["documents"].as<JsonArray>()) {
          if (resiDataIndex < PAKET_MAX) {
            resiData[resiDataIndex].nama = fields["fields"]["nama"]["stringValue"].as<String>();
            resiData[resiDataIndex].noResi = fields["fields"]["noResi"]["stringValue"].as<String>();
            resiData[resiDataIndex].packetType = fields["fields"]["packetType"]["stringValue"].as<String>();
            resiData[resiDataIndex].resiId = fields["fields"]["resiId"]["integerValue"].as<int>();
            resiDataIndex++;
          }
        }
      }
      
      // if (!firestore->isReady()) {  // FIREBASE_FIRESTORE_START
      //   buzzer.toggleAsync(250, [](bool state) {
      //     Serial.println("Wait for Firestore Ready !!");
      //   });
      // } else {
      //   switch (firebaseFirestoreState) {
      //     case FIRESTORE_CREATE:
      //       Serial.print("| FIRESTORE_CREATE: ");
      //       Serial.print(FIRESTORE_CREATE);
      //       Serial.println();

      //       documentData["fields"]["name"]["stringValue"] = "John Doe";
      //       documentData["fields"]["age"]["integerValue"] = 30;
      //       documentData["fields"]["active"]["booleanValue"] = true;
      //       serializeJson(documentData, documentDataStr);

      //       resultStr = firestore->createDocument("users/user1", documentDataStr, true);
      //       Serial.print("| resultStr");
      //       Serial.print(resultStr);
      //       Serial.println();

      //       firebaseFirestoreState = FIRESTORE_IDE;
      //       break;
      //     case FIRESTORE_READ:
      //       Serial.print("| FIRESTORE_READ: ");
      //       Serial.print(FIRESTORE_READ);
      //       Serial.println();

      //       resultStr = firestore->getDocument("users/user1", "", true);
      //       Serial.print("| resultStr");
      //       Serial.print(resultStr);
      //       Serial.println();

      //       firebaseFirestoreState = FIRESTORE_IDE;
      //       break;
      //     case FIRESTORE_UPDATE:
      //       Serial.print("| FIRESTORE_UPDATE: ");
      //       Serial.print(FIRESTORE_UPDATE);
      //       Serial.println();

      //       documentData["fields"]["name"]["stringValue"] = "John Doe " + String(random(0, 30));
      //       documentData["fields"]["age"]["integerValue"] = random(0, 30);
      //       serializeJson(documentData, documentDataStr);

      //       resultStr = firestore->updateDocument("users/user1", documentDataStr, "name,age", true);
      //       Serial.print("| resultStr");
      //       Serial.print(resultStr);
      //       Serial.println();

      //       firebaseFirestoreState = FIRESTORE_IDE;
      //       break;
      //     case FIRESTORE_DELETE:
      //       Serial.print("| FIRESTORE_DELETE: ");
      //       Serial.print(FIRESTORE_DELETE);
      //       Serial.println();

      //       resultStr = firestore->deleteDocument("users/user1", true);
      //       Serial.print("| resultStr");
      //       Serial.print(resultStr);
      //       Serial.println();

      //       firebaseFirestoreState = FIRESTORE_IDE;
      //       break;
      //   }
      // }  // FIREBASE_FIRESTORE_END

      // if (!messaging->isReady()) {  // FIREBASE_MESSAGING_START
      //   buzzer.toggleAsync(250, [](bool state) {
      //     Serial.println("Wait for Mesagging Ready !!");
      //   });
      // } else {
      //   if (firebaseMessagingState == MESSAGING_SEND) {
      //     messaging->clearMessage();
      //     messaging->setToken(FIREBASE_MSG_DEVICE_TOKEN);
      //     messaging->setNotification("Pesan dari ESP32", "Hallo ESP32");
      //     messaging->setAndroidPriority(true);
      //     resultStr = messaging->sendMessage(true);
      //     Serial.println(resultStr);
      //     firebaseMessagingState = MESSAGING_IDLE;
      //   }
      // }  // FIREBASE_MESSAGING_END
    }
  });
}