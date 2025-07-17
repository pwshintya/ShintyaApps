void onLcdMenu() {
  static auto menuMain = menu.createMenu(4, "Resi Non-COD", "Resi COD", "Ambil Paket", "Info: Paket Penuh");

  menu.onSelect(menuMain, "Resi Non-COD", []() {
    static auto menuNonCOD = menu.createMenu(4, "    [NON-COD]    ", "  Silahkan Scan  ", " Resi Paket Anda ", "");
    if (!buttonOkStr.isEmpty()) {
      menu.clearMenu(menuNonCOD, menuMain, menu.end());
    }
    if (!resiBarcode.isEmpty()) {
      menu.formatMenu(menuNonCOD, 3, "[%s]", resiBarcode.c_str());
      menu.showMenu(menuNonCOD, true);
      delay(2000);
      menu.formatMenu(menuNonCOD, 3, "%s", "                 ");
      auto menuNonCODCheck = menu.createMenu(4, "    [NON-COD]    ", " Pengecekan Resi ", "", "");
      menu.showMenu(menuNonCODCheck, true);
      delay(2000);
      bool isResiTerdaftar = false;
      int indexResiTerdaftar = -1;
      for (int i = 0; i < PAKET_MAX; i++) {
        if (resiBarcode == resiData[i].noResi) {
          isResiTerdaftar = true;
          indexResiTerdaftar = i;
          Serial.print("| resiData[" + String(i) + "].resiId: ");
          Serial.print(resiData[i].resiId);
          Serial.println();
          break;
        }
      }
      if (isResiTerdaftar) {
        if (resiData[indexResiTerdaftar].packetType != "non-COD") {
          menu.formatMenu(menuNonCODCheck, 1, "%s", "Tipe Paket Salah ");
          menu.formatMenu(menuNonCODCheck, 2, "%s", " Silahkan Pilih  ");
          menu.formatMenu(menuNonCODCheck, 3, "%s", "    Menu COD     ");
          menu.showMenu(menuNonCODCheck, true);
          delay(4000);
          menu.clearMenu(menuNonCOD, menuMain, menu.end());
          resiBarcode = "";
          menu.freeMenu(menuNonCODCheck);
          return;
        }
        menu.formatMenu(menuNonCODCheck, 2, "%s", "   Nomor Resi    ");
        menu.formatMenu(menuNonCODCheck, 3, "%s", "    Terdaftar    ");
        menu.showMenu(menuNonCODCheck, true);
        delay(4000);
        auto menuNonCODResiTerdaftar = menu.createMenu(4, "    [NON-COD]    ", "  Pintu Terbuka  ", "  Mohon Tunggu   ", "");
        menu.showMenu(menuNonCODResiTerdaftar, true);
        delay(2000);
        auto menuNonCODMasukanPaket = menu.createMenu(4, "    [NON-COD]    ", "    Silahkan     ", " Memasukan Paket ", "");
        menu.showMenu(menuNonCODMasukanPaket, true);
        delay(4000);
        auto menuNonCODTerimaKasih = menu.createMenu(4, "    [NON-COD]    ", "     Terima      ", "      Kasih      ", "");
        menu.showMenu(menuNonCODTerimaKasih, true);
        delay(2000);
        menu.clearMenu(menuNonCOD, menuMain, menu.end());
        resiBarcode = "";
        menu.freeMenu(menuNonCODCheck);
        menu.freeMenu(menuNonCODResiTerdaftar);
        menu.freeMenu(menuNonCODMasukanPaket);
        menu.freeMenu(menuNonCODTerimaKasih);
        return;
      } else {
        menu.formatMenu(menuNonCODCheck, 2, "%s", "   Nomor Resi    ");
        menu.formatMenu(menuNonCODCheck, 3, "%s", " Tidak Terdaftar ");
        menu.showMenu(menuNonCODCheck, true);
        delay(4000);
        menu.clearMenu(menuNonCOD, menuMain, menu.end());
        resiBarcode = "";
        menu.freeMenu(menuNonCODCheck);
        return;
      }
    }
    menu.showMenu(menuNonCOD);
  });

  menu.onSelect(menuMain, "Resi COD", []() {
    static auto menuCOD = menu.createMenu(4, "      [COD]      ", "  Silahkan Scan  ", " Resi Paket Anda ", "");
    if (!buttonOkStr.isEmpty()) {
      menu.clearMenu(menuCOD, menuMain, menu.end());
    }
    if (!resiBarcode.isEmpty()) {
      menu.formatMenu(menuCOD, 3, "[%s]", resiBarcode.c_str());
      menu.showMenu(menuCOD, true);
      delay(2000);
      menu.formatMenu(menuCOD, 3, "%s", "                 ");
      auto menuCODCheck = menu.createMenu(4, "      [COD]      ", " Pengecekan Resi ", "", "");
      menu.showMenu(menuCODCheck, true);
      delay(2000);
      bool isResiTerdaftar = false;
      int indexResiTerdaftar = -1;
      for (int i = 0; i < PAKET_MAX; i++) {
        if (resiBarcode == resiData[i].noResi) {
          isResiTerdaftar = true;
          indexResiTerdaftar = i;
          Serial.print("| resiData[" + String(i) + "].resiId: ");
          Serial.print(resiData[i].resiId);
          Serial.println();
          break;
        }
      }
      if (isResiTerdaftar) {
        if (resiData[indexResiTerdaftar].packetType != "COD") {
          menu.formatMenu(menuCODCheck, 1, "%s", "Tipe Paket Salah ");
          menu.formatMenu(menuCODCheck, 2, "%s", " Silahkan Pilih  ");
          menu.formatMenu(menuCODCheck, 3, "%s", "  Menu Non-COD   ");
          menu.showMenu(menuCODCheck, true);
          delay(4000);
          menu.clearMenu(menuCOD, menuMain, menu.end());
          resiBarcode = "";
          menu.freeMenu(menuCODCheck);
          return;
        }
        menu.formatMenu(menuCODCheck, 2, "%s", "   Nomor Resi    ");
        menu.formatMenu(menuCODCheck, 3, "%s", "    Terdaftar    ");
        menu.showMenu(menuCODCheck, true);
        delay(4000);
        auto menuCODResiTerdaftar = menu.createMenu(4, "      [COD]      ", "  Pintu Terbuka  ", "  Mohon Tunggu   ", "");
        menu.showMenu(menuCODResiTerdaftar, true);
        delay(2000);
        auto menuCODMasukanPaket = menu.createMenu(4, "      [COD]      ", "    Silahkan     ", " Memasukan Paket ", "");
        menu.showMenu(menuCODMasukanPaket, true);
        delay(4000);
        auto menuCODAmbilCash = menu.createMenu(4, "      [COD]      ", " Silahkan Ambil  ", " Uang Anda Pada  ", "  Laci Samping   ");
        menu.showMenu(menuCODAmbilCash, true);
        delay(4000);
        auto menuCODTerimaKasih = menu.createMenu(4, "      [COD]      ", "     Terima      ", "      Kasih      ", "");
        menu.showMenu(menuCODTerimaKasih, true);
        delay(2000);
        menu.clearMenu(menuCOD, menuMain, menu.end());
        resiBarcode = "";
        menu.freeMenu(menuCODCheck);
        menu.freeMenu(menuCODResiTerdaftar);
        menu.freeMenu(menuCODMasukanPaket);
        menu.freeMenu(menuCODAmbilCash);
        menu.freeMenu(menuCODTerimaKasih);
        return;
      } else {
        menu.formatMenu(menuCODCheck, 2, "%s", "   Nomor Resi    ");
        menu.formatMenu(menuCODCheck, 3, "%s", " Tidak Terdaftar ");
        menu.showMenu(menuCODCheck, true);
        delay(4000);
        menu.clearMenu(menuCOD, menuMain, menu.end());
        resiBarcode = "";
        menu.freeMenu(menuCODCheck);
        return;
      }
    }
    menu.showMenu(menuCOD);
  });

  menu.onSelect(menuMain, "Ambil Paket", []() {
    static auto menuAmbilPaket = menu.createMenu(4, "  [AMBIL PAKET]  ", "  Silahkan Scan  ", "  QR Code Anda   ", "");
    if (!ambilPaketState) {
      if (!buttonOkStr.isEmpty()) {
        menu.clearMenu(menuAmbilPaket, menuMain, menu.end());
      }
      if (!userQRCode.isEmpty()) {
        menu.formatMenu(menuAmbilPaket, 3, "[%s]", userQRCode.c_str());
        menu.showMenu(menuAmbilPaket, true);
        delay(2000);
        bool qrCodeTerdaftar = false;
        for (int i = 0; i < USER_MAX; i++) {
          if (userQRCode == userData[i].name) {
            qrCodeTerdaftar = true;
            break;
          }
        }
        if (qrCodeTerdaftar) {
          auto menuAmbilPaketBerhasil = menu.createMenu(4, "  [AMBIL PAKET]  ", "  QR Code Anda   ", "Terdaftar, Pintu ", "  Akan Terbuka   ");
          menu.showMenu(menuAmbilPaketBerhasil, true);
          delay(4000);
          menu.freeMenu(menuAmbilPaketBerhasil);
          // menu.clearMenu(menuMain, menu.end());
          menu.formatMenu(menuAmbilPaket, 3, "[%s]", "                 ");
          userQRCode = "";
          ambilPaketState = true;
          return;
        } else {
          auto menuAmbilPaketGagal = menu.createMenu(4, "  [AMBIL PAKET]  ", "  QR Code Anda   ", " Tidak Terdafar  ", "");
          menu.showMenu(menuAmbilPaketGagal, true);
          delay(2000);
          menu.freeMenu(menuAmbilPaketGagal);
          menu.clearMenu(menuMain, menu.end());
          menu.formatMenu(menuAmbilPaket, 3, "[%s]", "                 ");
          userQRCode = "";
          return;
        }
      }
    } else {
      static auto menuAmbilPaketOK = menu.createMenu(4, "  [AMBIL PAKET]  ", "  Tekan Tombol   ", "    OK Untuk     ", " Mengunci Pintu  ");
      menu.showMenu(menuAmbilPaketOK);
      if (!buttonOkStr.isEmpty()) {
        menu.clearMenu(menuAmbilPaket, menuMain, menu.end());
        userQRCode = "";
        ambilPaketState = false;
      }
      return;
    }
    menu.showMenu(menuAmbilPaket);
  });

  menu.showMenu(menuMain);
}