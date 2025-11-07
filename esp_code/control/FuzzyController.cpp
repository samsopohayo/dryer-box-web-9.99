#include "FuzzyController.h"

void FuzzyController::setup() {
  setupInputsOutputs();
  setupRules();
  Serial.println("✅ Fuzzy logic system initialized");
}

void FuzzyController::setupInputsOutputs() {
  // Input 1: Suhu
  FuzzyInput *suhu = new FuzzyInput(1);
  suhu->addFuzzySet(dinginSuhu);
  suhu->addFuzzySet(normalSuhu);
  suhu->addFuzzySet(panasSuhu);
  fuzzy->addFuzzyInput(suhu);
  
  // Input 2: Kadar Air
  FuzzyInput *moisture = new FuzzyInput(2);
  moisture->addFuzzySet(keringKadar);
  moisture->addFuzzySet(normalKadar);
  moisture->addFuzzySet(lembabKadar);
  fuzzy->addFuzzyInput(moisture);
  
  // Input 3: Humidity
  FuzzyInput *humidity = new FuzzyInput(3);
  humidity->addFuzzySet(rendahHumidity);
  humidity->addFuzzySet(idealHumidity);
  humidity->addFuzzySet(tinggiHumidity);
  fuzzy->addFuzzyInput(humidity);
  
  // Output 1: Pemanas
  FuzzyOutput *pemanas = new FuzzyOutput(1);
  pemanas->addFuzzySet(matiPemanas);
  pemanas->addFuzzySet(hangatPemanas);
  pemanas->addFuzzySet(panasPemanas);
  fuzzy->addFuzzyOutput(pemanas);
  
  // Output 2: Kipas
  FuzzyOutput *kipas = new FuzzyOutput(2);
  kipas->addFuzzySet(matiKipas);
  kipas->addFuzzySet(lambatKipas);
  kipas->addFuzzySet(cepatKipas);
  fuzzy->addFuzzyOutput(kipas);
  
  // Output 3: Exhaust
  FuzzyOutput *exhaust = new FuzzyOutput(3);
  exhaust->addFuzzySet(tutupExhaust);
  exhaust->addFuzzySet(sedangExhaust);
  exhaust->addFuzzySet(bukaExhaust);
  fuzzy->addFuzzyOutput(exhaust);
}

void FuzzyController::setupRules() {
  // R1: IF Dingin AND Kering THEN Mati AND Mati
  FuzzyRuleAntecedent *r1_antecedent = new FuzzyRuleAntecedent();
  r1_antecedent->joinWithAND(dinginSuhu, keringKadar);
  FuzzyRuleConsequent *r1_consequent = new FuzzyRuleConsequent();
  r1_consequent->addOutput(matiPemanas);
  r1_consequent->addOutput(matiKipas);
  FuzzyRule *rule1 = new FuzzyRule(1, r1_antecedent, r1_consequent);
  fuzzy->addFuzzyRule(rule1);
  
  // R2: IF Dingin AND Normal THEN Panas AND Cepat
  FuzzyRuleAntecedent *r2_antecedent = new FuzzyRuleAntecedent();
  r2_antecedent->joinWithAND(dinginSuhu, normalKadar);
  FuzzyRuleConsequent *r2_consequent = new FuzzyRuleConsequent();
  r2_consequent->addOutput(panasPemanas);
  r2_consequent->addOutput(cepatKipas);
  FuzzyRule *rule2 = new FuzzyRule(2, r2_antecedent, r2_consequent);
  fuzzy->addFuzzyRule(rule2);
  
  // R3: IF Dingin AND Lembab THEN Panas AND Cepat
  FuzzyRuleAntecedent *r3_antecedent = new FuzzyRuleAntecedent();
  r3_antecedent->joinWithAND(dinginSuhu, lembabKadar);
  FuzzyRuleConsequent *r3_consequent = new FuzzyRuleConsequent();
  r3_consequent->addOutput(panasPemanas);
  r3_consequent->addOutput(cepatKipas);
  FuzzyRule *rule3 = new FuzzyRule(3, r3_antecedent, r3_consequent);
  fuzzy->addFuzzyRule(rule3);
  
  // R4: IF Normal AND Kering THEN Mati AND Mati
  FuzzyRuleAntecedent *r4_antecedent = new FuzzyRuleAntecedent();
  r4_antecedent->joinWithAND(normalSuhu, keringKadar);
  FuzzyRuleConsequent *r4_consequent = new FuzzyRuleConsequent();
  r4_consequent->addOutput(matiPemanas);
  r4_consequent->addOutput(matiKipas);
  FuzzyRule *rule4 = new FuzzyRule(4, r4_antecedent, r4_consequent);
  fuzzy->addFuzzyRule(rule4);
  
  // R5: IF Normal AND Normal THEN Hangat AND Lambat
  FuzzyRuleAntecedent *r5_antecedent = new FuzzyRuleAntecedent();
  r5_antecedent->joinWithAND(normalSuhu, normalKadar);
  FuzzyRuleConsequent *r5_consequent = new FuzzyRuleConsequent();
  r5_consequent->addOutput(hangatPemanas);
  r5_consequent->addOutput(lambatKipas);
  FuzzyRule *rule5 = new FuzzyRule(5, r5_antecedent, r5_consequent);
  fuzzy->addFuzzyRule(rule5);
  
  // R6: IF Normal AND Lembab THEN Hangat AND Lambat
  FuzzyRuleAntecedent *r6_antecedent = new FuzzyRuleAntecedent();
  r6_antecedent->joinWithAND(normalSuhu, lembabKadar);
  FuzzyRuleConsequent *r6_consequent = new FuzzyRuleConsequent();
  r6_consequent->addOutput(hangatPemanas);
  r6_consequent->addOutput(lambatKipas);
  FuzzyRule *rule6 = new FuzzyRule(6, r6_antecedent, r6_consequent);
  fuzzy->addFuzzyRule(rule6);
  
  // R7: IF Panas AND Kering THEN Mati AND Mati
  FuzzyRuleAntecedent *r7_antecedent = new FuzzyRuleAntecedent();
  r7_antecedent->joinWithAND(panasSuhu, keringKadar);
  FuzzyRuleConsequent *r7_consequent = new FuzzyRuleConsequent();
  r7_consequent->addOutput(matiPemanas);
  r7_consequent->addOutput(matiKipas);
  FuzzyRule *rule7 = new FuzzyRule(7, r7_antecedent, r7_consequent);
  fuzzy->addFuzzyRule(rule7);
  
  // R8: IF Panas AND Normal THEN Mati AND Lambat
  FuzzyRuleAntecedent *r8_antecedent = new FuzzyRuleAntecedent();
  r8_antecedent->joinWithAND(panasSuhu, normalKadar);
  FuzzyRuleConsequent *r8_consequent = new FuzzyRuleConsequent();
  r8_consequent->addOutput(matiPemanas);
  r8_consequent->addOutput(lambatKipas);
  FuzzyRule *rule8 = new FuzzyRule(8, r8_antecedent, r8_consequent);
  fuzzy->addFuzzyRule(rule8);
  
  // R9: IF Panas AND Lembab THEN Mati AND Cepat
  FuzzyRuleAntecedent *r9_antecedent = new FuzzyRuleAntecedent();
  r9_antecedent->joinWithAND(panasSuhu, lembabKadar);
  FuzzyRuleConsequent *r9_consequent = new FuzzyRuleConsequent();
  r9_consequent->addOutput(matiPemanas);
  r9_consequent->addOutput(cepatKipas);
  FuzzyRule *rule9 = new FuzzyRule(9, r9_antecedent, r9_consequent);
  fuzzy->addFuzzyRule(rule9);
  
  Serial.println("✅ Fuzzy logic rules configured");
}

void FuzzyController::apply() {
  Serial.println("\n🧠 === FUZZY LOGIC CONTROL MODE ===");
  
  fuzzy->setInput(1, suhuSensor);
  fuzzy->setInput(2, kadarAir);
  fuzzy->setInput(3, kelembapanSensor);
  
  fuzzy->fuzzify();
  
  float outputPemanas = fuzzy->defuzzify(1);
  float outputKipas = fuzzy->defuzzify(2);
  
  Serial.println("   Input - Suhu: " + String(suhuSensor, 1) + "°C");
  Serial.println("   Input - Kadar Air: " + String(kadarAir, 1) + "%");
  Serial.println("   Input - Humidity: " + String(kelembapanSensor, 1) + "%");
  Serial.println("   Output - Pemanas: " + String(outputPemanas, 1));
  Serial.println("   Output - Kipas: " + String(outputKipas, 1));
  
  if (!manualHeaterControl) controlHeater(outputPemanas);
  if (!manualFanColControl && !manualFanPanControl) controlFan(outputKipas);
}

void FuzzyController::controlHeater(float output) {
  if (output <= 84) {
    digitalWrite(RELAY1_PIN, RELAY_OFF);
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    pemanasStatus = "Mati";
  } else if (output >= 85 && output <= 126) {
    digitalWrite(RELAY1_PIN, RELAY_ON);
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    pemanasStatus = "Hangat";
  } else {
    digitalWrite(RELAY1_PIN, RELAY_ON);
    digitalWrite(RELAY2_PIN, RELAY_ON);
    pemanasStatus = "Panas";
  }
  Serial.println("🔥 Heater: " + pemanasStatus);
}

void FuzzyController::controlFan(float output) {
  int duty = constrain((int)round(output), 0, 255);
  int persen = map(duty, 0, 255, 0, 100);
  
  ledcWrite(FANHEATPAN1, duty);
  
  bool kondisiCerah = (weatherMain == "cerah" || weatherMain == "berawan");
  time_t now = time(nullptr);
  struct tm* ptm = localtime(&now);
  int jamSekarang = ptm->tm_hour;
  bool jamKolektor = (jamSekarang >= 6 && jamSekarang < 17);
  
  if (duty == 0) {
    ledcWrite(FANHEATCOL2, 0);
    kipasStatus = "Mati";
    
  } else if (kondisiCerah && jamKolektor) {
    ledcWrite(FANHEATCOL2, duty);
    kipasStatus = "Pan & Col aktif (pwr: " + String(persen) + "%, cuaca: " + weatherMain + ")";
    
  } else {
    ledcWrite(FANHEATCOL2, 0);
    
    if (kondisiCerah) {
      kipasStatus = "Hanya Pan (pwr: " + String(persen) + "%, malam hari)";
    } else {
      kipasStatus = "Hanya Pan (pwr: " + String(persen) + "%, cuaca: " + weatherMain + ")";
    }
  }
  
  Serial.println("💨 Fan status: " + kipasStatus);
}

void FuzzyController::controlExhaust(float output) {
  if (output <= 30) {
    digitalWrite(MOSFET1_PIN, LOW);
    exhaustServo.write(0);
    exhaustStatus = "Tutup";
  } else if (output >= 30 && output <= 70) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(45);
    exhaustStatus = "Sedang";
  } else {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Buka";
  }
  Serial.println("🌬  Exhaust: " + exhaustStatus);
}