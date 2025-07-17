#define FIREBASE_DATABASE_URL "https://anisa-km-default-rtdb.firebaseio.com/"
#define FIREBASE_PROJECT_ID "anisa-km"
#define FIREBASE_API_KEY "AIzaSyAE0KeDPNL-oHbnMY_BJz-ekqd6wJ_anpQ"
#define FIREBASE_USER_EMAIL "iwan.dwp@gmail.com"
#define FIREBASE_USER_PASSWORD "admin123"
#define FIREBASE_CLIENT_EMAIL "firebase-adminsdk-fbsvc@anisa-km.iam.gserviceaccount.com"
#define FIREBASE_MSG_DEVICE_TOKEN "cJnjCBzORlawc7T2WvCq2L:APA91bEyoA65YjDAEU6Y_Mj6DQzw5KH_Svfs7ZoLv3Vdl-ZurpiN8BGi1R3qaOh1Ux_wNHacMHSGOfHuxxKQraLcWC-RowpmEvPQboZasgsWJQ_MWdS285Q"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQComXoZhw8Bfxsw\n3V4saUeF3DZ2P+tq8ZU4mU0NuYvnewG0F4F9q4Q1r0B7c8+gc+p3pek5v/oGAxzg\nrEBmaXwngb6dssH766Z191ZQKm40DzA9/eX6/iD/PsC+pQo1wr+lbNIyHfqQpj5y\nwpza806vDv9fSeILM0xQS8z3I/1Sf9M8gOlDhyV1TtKJpCsXB7HYfBiIk7Wmvhp0\n+EGEG7wJOaj7mv/tK0M2suaV+3/NnZy9hx8Sl1I+utwkdGjhIVGEQMU0DU8AwETk\ncxeJkPWN6Cbx4ji87z27GASR4iwt15aaRlYjb/aPwmQIunYubw4Rn2umxcCqGJPO\nXKSYMypzAgMBAAECggEAHSAiVS2dPINyNY9QWPi499lyeRwSta9VOWlWQ9Q1ZIA9\n65gR8R1DoiiLyafYit3rd87sCuNstEQuZWusVmTLo2GJBeIfG796Sq+3M/MYhZrI\nGTV22NS065MyJd0J9f6WL+QsLac7JWjqqdWP1k0o3xoGqLXxPEEY4lgt6NXJScpu\ncV1FakwgYVJyMf+vL0pzGNOz2YZGW0krexywIlgvbuGW+sY62E78ba7nQGdXDYJ1\nOtwUa9DV73rWFeYOaU4rSC4ENJvSmvPuvY/R7QWe+uGE1TxXCY89M8UrafhjdlGO\nX6NbICG0/AOAgx4X/KHiFQOb6pd+OgPOEPZxrbK71QKBgQDR2ITkMoLyt9MjqAcg\nbtI8CoqiHpITMfqNgTQ8J9xLYWPHE/L5+4o7DqlP1Py5mFFvNiuBa7HbDEq+1pki\njAMDwDvtCAedic6ShfuDuh0dJAL3o7AGqzxkSjkSXN3zufyFA5O5YKTosXGqf5Rd\nxzNdjeYufXIC5MDbphBpBxkCRwKBgQDNrpCVqAdWVRaS6WUHJx1hj4Q1nV3KtxKP\nyJtwPAevLQTLOnHjRIoHvty9YRinjMhVhZwZNJZxXgnrM8gpJMuH+NVctV2usD3f\nJuLE5Jfx9CTjiej45blBAMDBUS8a/BGu6uBciEFBRX5VDqtbGmPGhKhcWZU06Zmb\nwx7wHoHgdQKBgQDRGYeSPCLv6OyjYxN25fbnjTmHQh8csXiQ+GYsayvAkVLzVJWR\nM20SXBGqUvlhlr3Oq5O+aqnXM/BnKT9+hTWbzznnf1DXUjJdHtT49cN8/h1W1ezZ\nOFf9N1iXS6J+8p0FjgLtfbfVhrtUwbv9ljlAx9zOBoJND9R9CoTfowrwkQKBgQCQ\ne6DbUCafUbRCzSsH5EUyZFN2Ki5ZmVKzqDLwm2q2doRYkpywDlR77mo3qj5cbHvJ\n/8pQtm08A9gp3b0skclbocOXlVvSu8EqwbEoqH/IGj3TjOwX85+39uRwqCvT4+A0\nJJAo0Bou1LdS3rQmkhomuMdf4Bn9Cl+6WE2iVXvchQKBgH4EXXtYLzbFsUapaL7b\nnnQLpqGzEDDj4mwCFZsYbhmZqZxLbMPjeSUc+moOVoanni4AGhAQGtWtz6yyQlYb\nxJeEK6RPcZXHt8wiGi1KEBCWiwqLeYdKZ8EfDKBvUppPeJydFTZ7S8g3snDofE7B\nGy0XToC5qXxI84XabPHtmnKL\n-----END PRIVATE KEY-----\n";

void wifiTask() {
  task.setInitCoreID(1);
  task.createTask(10000, [](void* pvParameter) {
    WiFi.begin("TIMEOSPACE", "1234Saja");
    // WiFi.begin("silenceAndSleep", "11111111");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("IP: " + WiFi.localIP().toString());
    client.setInsecure();

    if (!dateTime.begin()) {
      Serial.println("Gagal memulai NTP Client!");
    }

    if (!firebase.begin(client, FIREBASE_DATABASE_URL, FIREBASE_API_KEY, FIREBASE_USER_EMAIL, FIREBASE_USER_PASSWORD)) {
      if (firebase.hasError()) Serial.println("Firebase RTDB Error: " + firebase.getError());
      while (1) { delay(1000); }
    }

    if (!firestore.begin(FIREBASE_API_KEY, FIREBASE_USER_EMAIL, FIREBASE_USER_PASSWORD, FIREBASE_PROJECT_ID)) {
      Serial.println("Firebase Firestore Error: " + firestore.getLastError());
      while (1) { delay(1000); }
    }

    if (!messaging.begin(FIREBASE_CLIENT_EMAIL, FIREBASE_PROJECT_ID, PRIVATE_KEY, 10000)) {
      Serial.println("Firebase Mesagging Error: " + messaging.getLastError());
      while (1) { delay(1000); }
    }

    Serial.println("Firebase Init Success");

    disableCore1WDT();
    buzzer.toggleInit(100, 2);

    JsonDocument documentData;
    String documentDataStr;
    String resultStr;

    for (;;) {
      firebase.loop();
      firestore.loop();
      messaging.loop();

      static uint32_t dateTimeNTPTimer;
      if (millis() - dateTimeNTPTimer >= 1000 && dateTime.update()) {
        Serial.println(dateTime.getDateTimeString());
        dateTimeNTPTimer = millis();
      }

      static uint32_t firebaseRTDBTimer;
      if (millis() - firebaseRTDBTimer >= 2000 && firebase.ready()) {  // FIREBASE_RTDB_START
        // firebase.set("/test/float", random(100) + 3.14159, 2);
        firebaseRTDBTimer = millis();
      }  // FIREBASE_RTDB_END

      if (!firestore.isReady()) {  // FIREBASE_FIRESTORE_START
        buzzer.toggleAsync(250, [](bool state) {
          Serial.println("Wait for Firestore Ready !!");
        });
      } else {
        switch (firebaseFirestoreState) {
          case FIRESTORE_CREATE:
            Serial.print("| FIRESTORE_CREATE: ");
            Serial.print(FIRESTORE_CREATE);
            Serial.println();

            documentData["fields"]["name"]["stringValue"] = "John Doe";
            documentData["fields"]["age"]["integerValue"] = 30;
            documentData["fields"]["active"]["booleanValue"] = true;
            serializeJson(documentData, documentDataStr);

            resultStr = firestore.createDocument("users/user1", documentDataStr, true);
            Serial.print("| resultStr");
            Serial.print(resultStr);
            Serial.println();

            firebaseFirestoreState = FIRESTORE_IDE;
            break;
          case FIRESTORE_READ:
            Serial.print("| FIRESTORE_READ: ");
            Serial.print(FIRESTORE_READ);
            Serial.println();

            resultStr = firestore.getDocument("users/user1", "", true);
            Serial.print("| resultStr");
            Serial.print(resultStr);
            Serial.println();

            firebaseFirestoreState = FIRESTORE_IDE;
            break;
          case FIRESTORE_UPDATE:
            Serial.print("| FIRESTORE_UPDATE: ");
            Serial.print(FIRESTORE_UPDATE);
            Serial.println();

            documentData["fields"]["name"]["stringValue"] = "John Doe " + String(random(0, 30));
            documentData["fields"]["age"]["integerValue"] = random(0, 30);
            serializeJson(documentData, documentDataStr);

            resultStr = firestore.updateDocument("users/user1", documentDataStr, "name,age", true);
            Serial.print("| resultStr");
            Serial.print(resultStr);
            Serial.println();

            firebaseFirestoreState = FIRESTORE_IDE;
            break;
          case FIRESTORE_DELETE:
            Serial.print("| FIRESTORE_DELETE: ");
            Serial.print(FIRESTORE_DELETE);
            Serial.println();

            resultStr = firestore.deleteDocument("users/user1", true);
            Serial.print("| resultStr");
            Serial.print(resultStr);
            Serial.println();

            firebaseFirestoreState = FIRESTORE_IDE;
            break;
        }
      }  // FIREBASE_FIRESTORE_END

      if (!messaging.isReady()) {  // FIREBASE_MESSAGING_START
        buzzer.toggleAsync(250, [](bool state) {
          Serial.println("Wait for Mesagging Ready !!");
        });
      } else {
        if (firebaseMessagingState) {
          messaging.clearMessage();
          messaging.setToken(FIREBASE_MSG_DEVICE_TOKEN);
          messaging.setNotification("Pesan dari ESP32", "Hallo ESP32");
          messaging.setAndroidPriority(true);
          resultStr = messaging.sendMessage(true);
          Serial.println(resultStr);
          firebaseMessagingState = 0;
        }
      }  // FIREBASE_MESSAGING_END
    }
  });
}