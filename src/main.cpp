#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// =====================================================
// CONFIGURACIÓN SERIAL
// =====================================================

constexpr uint32_t SERIAL_BAUD = 115200;

// =====================================================
// PINES ESP32-S3 + RC522
// =====================================================

constexpr uint8_t SS_PIN   = 10;
constexpr uint8_t SCK_PIN  = 12;
constexpr uint8_t MOSI_PIN = 11;
constexpr uint8_t MISO_PIN = 13;
constexpr uint8_t RST_PIN  = 9;

// =====================================================
// CONFIGURACIÓN RFID
// =====================================================

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Bloque donde vamos a escribir.
// Usamos el bloque 4, que pertenece al sector 1.
// IMPORTANTE: no usar bloques trailer: 3, 7, 11, 15...
constexpr byte BLOCK_ADDR = 4;

// Mensaje máximo: 16 bytes por bloque.
byte messageToWrite[16] = {
  'V','I','V','A',' ',
  'E','S','P','A','N','Y','A',
  ' ',' ',' ',' '
};

// =====================================================
// FUNCIONES
// =====================================================

void initSerial() {
  Serial.begin(SERIAL_BAUD);
  delay(1500);

  Serial.println();
  Serial.println("======================================");
  Serial.println(" EJERCICIO 2 - ESCRIBIR Y LEER RFID");
  Serial.println(" ESP32-S3 + RC522 + SPI");
  Serial.println("======================================");
}

void initSPI() {
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  Serial.println("[OK] SPI inicializado");
}

void initRFID() {
  rfid.PCD_Init();
  delay(100);

  Serial.println("[OK] RC522 inicializado");

  byte version = rfid.PCD_ReadRegister(rfid.VersionReg);

  Serial.print("Version RC522: 0x");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF) {
    Serial.println("[ERROR] El RC522 no responde. Revisa cableado.");
  }

  // Clave por defecto de muchas tarjetas MIFARE Classic:
  // FF FF FF FF FF FF
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("Acerca una tarjeta RFID para escribir el mensaje...");
}

bool waitForCard() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return false;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return false;
  }

  return true;
}

void printUID() {
  Serial.print("UID detectado: ");

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
}

bool authenticateBlock(byte blockAddr) {
  MFRC522::StatusCode status;

  status = rfid.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    blockAddr,
    &key,
    &(rfid.uid)
  );

  if (status != MFRC522::STATUS_OK) {
    Serial.print("[ERROR] Fallo autenticando bloque ");
    Serial.print(blockAddr);
    Serial.print(": ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  Serial.println("[OK] Autenticacion correcta");
  return true;
}

bool writeMessageToCard() {
  MFRC522::StatusCode status;

  status = rfid.MIFARE_Write(BLOCK_ADDR, messageToWrite, 16);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("[ERROR] No se pudo escribir: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  Serial.println("[OK] Mensaje escrito correctamente");
  return true;
}

bool readMessageFromCard() {
  byte buffer[18];
  byte size = sizeof(buffer);

  MFRC522::StatusCode status;

  status = rfid.MIFARE_Read(BLOCK_ADDR, buffer, &size);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("[ERROR] No se pudo leer: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  Serial.println("[OK] Mensaje leido correctamente");

  Serial.print("Mensaje guardado: ");

  for (byte i = 0; i < 16; i++) {
    Serial.write(buffer[i]);
  }

  Serial.println();

  Serial.print("Datos en HEX: ");

  for (byte i = 0; i < 16; i++) {
    if (buffer[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }

  Serial.println();

  return true;
}

void finishCommunication() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// =====================================================
// SETUP
// =====================================================

void setup() {
  initSerial();
  initSPI();
  initRFID();
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  if (!waitForCard()) {
    return;
  }

  Serial.println();
  Serial.println("======================================");
  Serial.println(" TARJETA DETECTADA");
  Serial.println("======================================");

  printUID();

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  Serial.print("Tipo de tarjeta: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  Serial.print("Bloque usado para escritura/lectura: ");
  Serial.println(BLOCK_ADDR);

  if (authenticateBlock(BLOCK_ADDR)) {
    writeMessageToCard();
    readMessageFromCard();
  }

  finishCommunication();

  Serial.println("======================================");
  Serial.println(" FIN DE OPERACION");
  Serial.println("Retira la tarjeta y vuelve a acercarla si quieres repetir.");
  Serial.println("======================================");
  Serial.println();

  delay(3000);
}