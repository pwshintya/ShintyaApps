void usbCommunicationTask(const String& dataRecv) {
  String data = dataRecv;
  String dataHeader = usbSerial.getStrData(dataRecv, 0, "#");
  String dataValue = usbSerial.getStrData(dataRecv, 1, "#");

  Serial.print("| dataRecv: ");
  Serial.print(dataRecv);
  Serial.println();

  if (isDigit(data[0]) || isDigit(data[1])) {
    // nums
  } else {
    dataHeader.toUpperCase();
    if (dataHeader == "R") ESP.restart();
    if (dataHeader == "FIRESTORE_CREATE") firebaseFirestoreState = FIRESTORE_CREATE;
    if (dataHeader == "FIRESTORE_READ") firebaseFirestoreState = FIRESTORE_READ;
    if (dataHeader == "FIRESTORE_UPDATE") firebaseFirestoreState = FIRESTORE_UPDATE;
    if (dataHeader == "FIRESTORE_DELETE") firebaseFirestoreState = FIRESTORE_DELETE;
    if (dataHeader == "MESSAGING_SEND") firebaseMessagingState = 1;
  }
}