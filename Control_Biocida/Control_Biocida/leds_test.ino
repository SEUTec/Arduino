  /* Funció de prova de LEDs Indicadors
   * Activa tots els LEDs Indicador durant uns segons
  */
  
  void leds_test(){
    // LEDS Roijos inidicadors d'alarma
    digitalWrite(LED_DOSIFIACIO, HIGH);
    digitalWrite(LED_PA_LO_QUE_CONVINGUE, HIGH);
    digitalWrite(LED_TANC_FORADAD, HIGH);

    // LEDS Verds indicadors
    digitalWrite(led_ERCROS, HIGH);
    digitalWrite(led_BMB1, HIGH);
    digitalWrite(led_BMB2, HIGH);

    delay(TEMPS_TEST_s * 1000);  // Espera en ms

    digitalWrite(LED_DOSIFIACIO, LOW);
    digitalWrite(LED_PA_LO_QUE_CONVINGUE, LOW);
    digitalWrite(LED_TANC_FORADAD, LOW);
    digitalWrite(led_ERCROS, LOW);
    digitalWrite(led_BMB1, LOW);
    digitalWrite(led_BMB2, LOW);
  }
