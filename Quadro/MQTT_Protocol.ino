/* MQTT_Protocol.ino
 * Gestione comunicazione MQTT per monitoraggio remoto del Quadro.
 *
 * Topics pubblicati (Quadro -> broker, retained):
 *   sintel/lavaggio/quadro/online        "true" / "false" (via LWT)
 *   sintel/lavaggio/quadro/traslo/stato  numero come stringa
 *   sintel/lavaggio/quadro/carro/stato   numero come stringa
 *
 * Topic sottoscritto (broker -> Quadro):
 *   sintel/lavaggio/quadro/cmd
 *     "firmware_update"  -> setta FW_UPDATE = 1 per OTA
 *     "status_request"   -> ripubblica online + stato traslo + stato carro
 *     "stop"             -> manda 02003 a entrambi gli slave
 */

// Callback chiamata da PubSubClient quando arriva un messaggio sui topic sottoscritti.
// Dispatching dei comandi sul topic /cmd.
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Copia payload in buffer terminato da null
  char buf[32];
  unsigned int len = (length < sizeof(buf) - 1) ? length : sizeof(buf) - 1;
  memcpy(buf, payload, len);
  buf[len] = '\0';

  Serial.print("MQTT ricevuto su ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(buf);

  // Dispatch comandi (solo per topic /cmd)
  if (strcmp(topic, MQTT_TOPIC_CMD) != 0) return;

  if (strcmp(buf, "firmware_update") == 0) {
    Serial.println("Comando MQTT: firmware_update");
    FW_UPDATE = 1;  // shouldUpdateFirmware() lo raccoglierà al prossimo loop
  }
  else if (strcmp(buf, "status_request") == 0) {
    Serial.println("Comando MQTT: status_request");
    pubblicaStatoCompleto();
  }
  else if (strcmp(buf, "stop") == 0) { // TODO non mostra niente su LCD al momento
    Serial.println("Comando MQTT: stop");
    // Mirror di controllaTastoStop(): manda 02003 a entrambi
    spedisciStringa("02003", TRASLO_ADD);
    spedisciStringa("02003", CARRO_ADD);
    Attesa_Risposta = false;
    Tentativi_Invio = 0;
    Messaggio_Buffered = "";
  }
  else {
    Serial.print("Comando MQTT sconosciuto: ");
    Serial.println(buf);
  }
}

// Tenta connessione al broker MQTT.
// Imposta LWT (Last Will and Testament) per pubblicare "false" su /online se la connessione cade.
// Su connessione riuscita: sottoscrive /cmd e pubblica stato iniziale.
// Ritorna true se connesso, false altrimenti.
bool connettiMQTT() {
  if (WiFi.status() != WL_CONNECTED) return false;  // niente WiFi, niente MQTT

  Serial.print("Tentativo connessione MQTT a ");
  Serial.print(MQTT_BROKER);
  Serial.print(":");
  Serial.println(MQTT_PORT);

  // connect() con LWT: se il Quadro si disconnette in modo anomalo,
  // il broker pubblica "false" su /online (retained, QoS 0)
  bool connesso = mqttClient.connect(
    MQTT_CLIENT_ID,
    MQTT_USER,
    MQTT_PASSWORD,
    MQTT_TOPIC_ONLINE,  // willTopic
    0,                  // willQos
    true,               // willRetain
    "false"             // willMessage
  );

  if (!connesso) {
    Serial.print("Connessione MQTT fallita, rc=");
    Serial.println(mqttClient.state());
    return false;
  }

  Serial.println("MQTT connesso");

  // Sottoscrizione al topic comandi
  if (mqttClient.subscribe(MQTT_TOPIC_CMD)) {
    Serial.print("Sottoscritto a ");
    Serial.println(MQTT_TOPIC_CMD);
  } else {
    Serial.println("Sottoscrizione fallita");
  }

  // Pubblica online=true (retained) e stato corrente
  mqttClient.publish(MQTT_TOPIC_ONLINE, "true", true);
  pubblicaStatoCompleto();

  return true;
}

// Pubblica lo stato corrente di entrambi gli slave + online.
// Chiamata su connessione e su comando "status_request".
// Aggiorna anche le variabili "ultimoPubblicato" per coerenza con la logica on-change.
void pubblicaStatoCompleto() {
  if (!mqttClient.connected()) return;

  char buf[8];

  mqttClient.publish(MQTT_TOPIC_ONLINE, "true", true);

  snprintf(buf, sizeof(buf), "%u", STATO_CORRENTE_TRASLO);
  mqttClient.publish(MQTT_TOPIC_TRASLO, buf, true);
  ultimoStatoTrasloPubblicato = STATO_CORRENTE_TRASLO;

  snprintf(buf, sizeof(buf), "%u", STATO_CORRENTE_CARRO);
  mqttClient.publish(MQTT_TOPIC_CARRO, buf, true);
  ultimoStatoCarroPubblicato = STATO_CORRENTE_CARRO;
}

// Pubblica stato traslo se cambiato dall'ultima pubblicazione.
// Da chiamare in esaminaMessaggio() dopo aver aggiornato STATO_CORRENTE_TRASLO.
void pubblicaStatoTrasloSeCambiato() {
  if (!mqttClient.connected()) return;
  if (STATO_CORRENTE_TRASLO == ultimoStatoTrasloPubblicato) return;

  char buf[8];
  snprintf(buf, sizeof(buf), "%u", STATO_CORRENTE_TRASLO);
  if (mqttClient.publish(MQTT_TOPIC_TRASLO, buf, true)) {
    ultimoStatoTrasloPubblicato = STATO_CORRENTE_TRASLO;
  }
}

// Analoga per il carro.
void pubblicaStatoCarroSeCambiato() {
  if (!mqttClient.connected()) return;
  if (STATO_CORRENTE_CARRO == ultimoStatoCarroPubblicato) return;

  char buf[8];
  snprintf(buf, sizeof(buf), "%u", STATO_CORRENTE_CARRO);
  if (mqttClient.publish(MQTT_TOPIC_CARRO, buf, true)) {
    ultimoStatoCarroPubblicato = STATO_CORRENTE_CARRO;
  }
}

// Da chiamare nel loop() ad ogni iterazione.
// Gestisce riconnessione non bloccante e processa messaggi in arrivo.
void gestisciMQTT() {
  if (!mqttClient.connected()) {
    uint32_t ora = millis();
    if (ora - lastReconnectAttemptMQTT >= MQTT_RECONNECT_INTERVAL) {
      lastReconnectAttemptMQTT = ora;
      connettiMQTT();  // se fallisce, ritenterà tra 5s
    }
  } else {
    mqttClient.loop();  // processa messaggi in arrivo
  }
}