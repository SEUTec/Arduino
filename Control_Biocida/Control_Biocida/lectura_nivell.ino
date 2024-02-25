/* Lectura Promijada de la Distancia mesurada pel sensor de distancia
*/

// Parametres correció lectura del ADS en volts.
//#define ADS_OFFSET -0.07  // per al ADS del duplicat
//#define ADS_OFFSET 0.0    // per al ADS instal lat
//#define ADS_SCALE  1.0

#define num_Lects 5          // Nombre de lectures per treure un valor mig

// Relació correcta 0 - 20 mA a 0 - 5V
// Segons adjust del convertidor de 0,4 - 20mA a 0 - 5 V
#define VLTS_4mA  0    // 0 Volts amb 4  mA, Nivell 0   %, 0    m, 0    L.
#define VLTS_20mA 5    // 5 Volts amb 20 mA, Nivell 100 %, 1410 m, 1250 L.

#define mA_20mA  20     // Corrent a 20 mA, Ja, Ja, Ja.
#define p100_20mA 100   // Percentarge a 20 mA.
#define MM_20mA   1310 // Direfencia de Metros confgirats al sensor a 20 m.
                       // Fondo Depòsit a 1410 mm del Radardona 4 mA
                       // Nivel allt del depòsit a 100 mm.
                       // Diferencia 4-20 mA es de 1410 - 100 = 1310 mm, segons configuració del Radar.
#define MTRS_20mA 1.310 // Metros al sensor a 20 mA.
// 1310 mm * 0.8888 L/mm = 1164,4 Litros que corresponen a 1310 mm, dels 20 mA.
//#define LTRS_20mA 1460  // Litros tank quan sensor dona 20 mA.
#define LTRS_20mA 1164  // Litros tank quan sensor dona 20 mA, amb 1310 mm de nivell.
//#define LTRS_Zero  300  // 300 L Que queden al tank quan el flotador ja toca al fondo.

// Paramatres del Tanc
#define RADI_INT_mm  532     // Radi Interior del Tanc 1064 mm / 2 = 532 mm
#define ALT_TANC_mm  1310    // altura zona de mesura, = 1410 - 100.

#define RADI_INT_m  0.532    // Radi Interior del Tanc 1.064 mm / 2 = 0.532 m
#define ALT_TANC_m  1.310    // altura zona de mesura = 1410 - 100

void lec_Nivell(){
    sns_Nivell_Volts = 0;

    for (int i=0; i < num_Lects; i=i+1){
      adc0 = ads.readADC_SingleEnded(0);         // Lectura sensor 
      float sns_Nivell_Volts_tmp = ads.computeVolts(adc0);  // Lectura sensor en Volts
      // sns_Nivell_Volts_tmp = ( sns_Nivell_Volts_tmp - ADS_OFFSET ) * ADS_SCALE;  // En principi no es necessari

      sns_Nivell_Volts = sns_Nivell_Volts + sns_Nivell_Volts_tmp;

      delay(100);
    }
    sns_Nivell_Volts = sns_Nivell_Volts / num_Lects;   // Distancia promijada en m o mm, segons
                                              // ALT_TANC_m o ALT_TANC_mm a Altura_Nivell =.....

    // fmap( x, in_min, in_max, out_min, out_max){
    // Si el sensor ens proporciona directamente la altura de líquid del tanc en mm
    // amb una senyal de 4-20mA, equivalent a 0 - ??? Litros
    //sns_Nivell_mA = fmap(sns_Nivell_Volts, VLTS_4mA, VLTS_20mA, 0, mA_20mA);   // Nivell Tanc de 0 a 20mA
    sns_Nivell_mA = fmap(sns_Nivell_Volts, VLTS_4mA, VLTS_20mA, 4, mA_20mA);   // Nivell Tanc de 4 a 20mA
    sns_Nivell_100 = fmap(sns_Nivell_Volts, VLTS_4mA, VLTS_20mA, 0, p100_20mA);   // Nivell Tanc de 0 a 100 %
    sns_Nivell_MM = fmap(sns_Nivell_Volts, VLTS_4mA, VLTS_20mA, 0, MM_20mA);   // Nivell Tanc de 0 a 1.410 m.
    sns_Nivell_Mtrs = fmap(sns_Nivell_Volts, VLTS_4mA, VLTS_20mA, 0, MTRS_20mA);   // Nivell Tanc de 0 a 1.410 m.
    sns_Nivell_Ltrs = fmap(sns_Nivell_Volts, VLTS_4mA, VLTS_20mA, 0, LTRS_20mA);   // Nivell Tanc de 0 a 1.250 L.
}

// cambi d escala entre floats
float fmap(float x, float in_min, float in_max, float out_min, float out_max){
   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
