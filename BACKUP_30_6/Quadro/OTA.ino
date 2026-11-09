// OTA - Aggiornamento firmware via HTTP
// Scarica il binario da FIRMWARE_URL e lo flasha sulla partizione inattiva.
// Triggerato da MQTT settando FW_UPDATE = 1.



// Controlla se è stato richiesto un aggiornamento.
// Ritorna true solo se la flag è settata E gli slave sono fermi.
// Se gli slave sono in movimento, l'aggiornamento viene posticipato (la flag resta a 1).
bool shouldUpdateFirmware() {
  if (!FW_UPDATE) return false;

  if (slavesInMovimento()) {
    // Posticipa: non resetta la flag, ritenterà al prossimo giro
    Serial.println("Aggiornamento posticipato: slave in movimento");
    return false;
  }

  FW_UPDATE = 0;  // resetta la flag
  return true;
}

void updateFirmware() {
  HTTPClient http;

  // Messaggio pre-aggiornamento
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Aggiornamento FW");
  lcd.setCursor(0, 1); lcd.print("in corso...");
  lcd.setCursor(0, 2); lcd.print("NON SPEGNERE");
  Serial.println("Inizio aggiornamento firmware");
  Serial.print("URL: ");
  Serial.println(FIRMWARE_URL);

  http.begin(FIRMWARE_URL);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Errore HTTP: %d (%s)\n", httpCode, http.errorToString(httpCode).c_str());
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Errore HTTP:");
    lcd.setCursor(0, 1); lcd.print(httpCode);
    delay(3000);
    http.end();
    return;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Serial.println("Lunghezza contenuto non valida");
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Errore: dimensione");
    lcd.setCursor(0, 1); lcd.print("non valida");
    delay(3000);
    http.end();
    return;
  }

  Serial.printf("Dimensione firmware: %d bytes\n", contentLength);

  if (!Update.begin(contentLength)) {
    Update.printError(Serial);
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Errore Update.begin");
    delay(3000);
    http.end();
    return;
  }

  WiFiClient* stream = http.getStreamPtr();
  uint8_t buf[512];
  int received = 0;
  int lastPercent = -1;
  uint32_t lastDataTime = millis();

  while (received < contentLength) {
    // Timeout di sicurezza: se non arrivano dati per 10 secondi, abortisci
    if (millis() - lastDataTime > 10000) {
      Serial.println("Timeout download");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Timeout download");
      delay(3000);
      Update.abort();
      http.end();
      return;
    }

    if (!http.connected()) {
      Serial.println("Connessione persa");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Connessione persa");
      delay(3000);
      Update.abort();
      http.end();
      return;
    }

    size_t available = stream->available();
    if (available == 0) {
      delay(1);  // cedi tempo allo stack TCP
      continue;
    }

    size_t toRead = (available > sizeof(buf)) ? sizeof(buf) : available;
    int c = stream->readBytes(buf, toRead);
    if (c <= 0) continue;

    if (Update.write(buf, c) != (size_t)c) {
      Update.printError(Serial);
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Errore scrittura");
      delay(3000);
      Update.abort();
      http.end();
      return;
    }

    received += c;
    lastDataTime = millis();

    // Aggiorna LCD ogni cambio di percentuale (non ad ogni byte)
    int percent = (received * 100) / contentLength;
    if (percent != lastPercent) {
      lastPercent = percent;
      lcd.setCursor(0, 3);
      lcd.print("Download: ");
      lcd.print(percent);
      lcd.print("%   ");
      Serial.printf("Ricevuti %d / %d bytes (%d%%)\n", received, contentLength, percent);
    }
  }

  if (!Update.end(true)) {
    Update.printError(Serial);
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Errore Update.end");
    delay(3000);
    http.end();
    return;
  }

  Serial.println("Aggiornamento completato. Riavvio...");
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Aggiornamento OK");
  lcd.setCursor(0, 1); lcd.print("Riavvio...");
  delay(1500);

  http.end();
  ESP.restart();
}