#include <Arduino.h>

// ================================================================
// ==================== KRAL AYARLAR (V11) ========================
// ================================================================

// 1. HIZ VE MOTOR
const int HIZ = 160;            // Temel Hız
const int SAG_MOTOR_OFSET = 15; // Düz giderken sağa çekiyorsa bunu azalt

// 2. DÖNÜŞ SÜRELERİ (Senin 300/600 ayarın)
int SURE_90 = 450;       
int SURE_180 = 800;      

// 3. MESAFE SINIRLARI
const int BOSLUK_SINIRI = 30;   // 30 cm üzeri "Yol Var" demektir
const int DUVAR_DURMA = 13;     // Ön duvara 13 cm kala dur

// 4. İLERLEME SÜRELERİ
const int KAVSAK_ORTALA = 250;  // Dönmeden önce azıcık öne gitme
const int YENI_YOL_GIRIS = 400; // Döndükten sonra koridora girme süresi

// ==================== PINLER ====================================
const int MA1 = 10; const int MA2 = 11; // Sol Motor
const int MB1 = 3;  const int MB2 = 9;  // Sağ Motor

const int T_ON=4, E_ON=12;
const int T_SAG=5, E_SAG=6;
const int T_SOL=2, E_SOL=13; 

long distOn, distSag, distSol;

// Fonksiyonlar
void duzGitVeDengele(int sure); // Duvar takipçisi
void manevra(int solHiz, int sagHiz);
void solaDon();
void sagaDon();
void geriDon();
void dur();
void hafifGeri();
long mesafeOlc(int trig, int echo);

void setup() {
  Serial.begin(9600);
  
  pinMode(MA1, OUTPUT); pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT); pinMode(MB2, OUTPUT);
  
  pinMode(T_ON, OUTPUT); pinMode(E_ON, INPUT);
  pinMode(T_SAG, OUTPUT); pinMode(E_SAG, INPUT);
  pinMode(T_SOL, OUTPUT); pinMode(E_SOL, INPUT);

  pinMode(7, INPUT); pinMode(8, INPUT); // Enkoder iptal

  delay(3000); 
}

void loop() {
  // 1. DUR VE ANALİZ ET
  dur();
  delay(300); // Net okuma için bekle
  
  distSol = mesafeOlc(T_SOL, E_SOL);
  distSag = mesafeOlc(T_SAG, E_SAG);
  distOn  = mesafeOlc(T_ON, E_ON);
  
  Serial.print("L:"); Serial.print(distSol);
  Serial.print(" F:"); Serial.print(distOn);
  Serial.print(" R:"); Serial.println(distSag);

  // === GÜVENLİK: SIKIŞMA KONTROLÜ ===
  if (distOn > 0 && distOn < 5) {
    hafifGeri();
    return;
  }

  // === MANTIK: BASİT VE NET SOL EL KURALI ===
  // Karşılaştırma yok. Sırayla kontrol var.

  // 1. ÖNCELİK: SOL AÇIK MI? -> DÖN
  if (distSol > BOSLUK_SINIRI) {
    Serial.println("KARAR: SOL BOS -> SOLA DON");
    
    // Kavşağa hizalan
    duzGitVeDengele(KAVSAK_ORTALA);
    dur();
    delay(100);
    
    solaDon();
    
    // Yeni yola gir
    duzGitVeDengele(YENI_YOL_GIRIS);
  }
  
  // 2. ÖNCELİK: ÖNÜM AÇIK MI? -> DÜZ GİT
  // (Sol doluysa buraya bakar)
  else if (distOn > DUVAR_DURMA) {
    Serial.println("KARAR: ON BOS -> DUZ GIT");
    // Düz git ama kör gitme, duvarları takip et!
    duzGitVeDengele(400); 
  }
  
  // 3. ÖNCELİK: SAĞ AÇIK MI? -> DÖN
  // (Sol ve Ön doluysa buraya bakar)
  else if (distSag > BOSLUK_SINIRI) {
    Serial.println("KARAR: SAG BOS -> SAGA DON");
    
    duzGitVeDengele(KAVSAK_ORTALA);
    dur();
    delay(100);
    
    sagaDon();
    
    duzGitVeDengele(YENI_YOL_GIRIS);
  }
  
  // 4. HİÇBİR YER YOK -> GERİ DÖN
  else {
    Serial.println("KARAR: CIKMAZ -> GERI");
    geriDon();
  }
}

// ==================== FONKSİYONLAR ====================

// Bu fonksiyon robotun koridorun ortasından gitmesini sağlar
void duzGitVeDengele(int sure) {
  unsigned long baslangic = millis();
  
  while(millis() - baslangic < (unsigned long)sure) {
    long anlikSol = mesafeOlc(T_SOL, E_SOL);
    long anlikSag = mesafeOlc(T_SAG, E_SAG);
    
    // GÜVENLİK: Önüme duvar çıktı mı?
    if (mesafeOlc(T_ON, E_ON) < DUVAR_DURMA) {
      dur();
      break;
    }

    // --- ORTALAMA MANTIĞI ---
    // Sola çok yaklaştım (Sol < 8cm) -> SAĞA KIR
    if (anlikSol < 8 && anlikSol > 0) {
      manevra(HIZ + 20, HIZ - 10); 
    }
    // Sağa çok yaklaştım (Sağ < 8cm) -> SOLA KIR
    else if (anlikSag < 8 && anlikSag > 0) {
      manevra(HIZ - 20, HIZ + 30); 
    }
    // Ortadaysam -> Düz devam
    else {
      manevra(HIZ, HIZ + SAG_MOTOR_OFSET);
    }
    
    delay(20); 
  }
  dur();
}

void manevra(int solHiz, int sagHiz) {
  if (solHiz > 255) solHiz = 255; if (solHiz < 0) solHiz = 0;
  if (sagHiz > 255) sagHiz = 255; if (sagHiz < 0) sagHiz = 0;
  
  analogWrite(MA1, solHiz); analogWrite(MA2, 0);
  analogWrite(MB1, sagHiz); analogWrite(MB2, 0);
}

void solaDon() {
  analogWrite(MA1, 0);   analogWrite(MA2, HIZ);
  analogWrite(MB1, HIZ); analogWrite(MB2, 0);
  delay(SURE_90);
  dur();
  delay(200);
}

void sagaDon() {
  analogWrite(MA1, HIZ); analogWrite(MA2, 0);
  analogWrite(MB1, 0);   analogWrite(MB2, HIZ);
  delay(SURE_90);
  dur();
  delay(200);
}

void geriDon() {
  analogWrite(MA1, HIZ); analogWrite(MA2, 0);
  analogWrite(MB1, 0);   analogWrite(MB2, HIZ);
  delay(SURE_180);
  dur();
  delay(200);
}

void hafifGeri() {
  analogWrite(MA1, 0); analogWrite(MA2, HIZ);
  analogWrite(MB1, 0); analogWrite(MB2, HIZ + SAG_MOTOR_OFSET);
  delay(350);
  dur();
}

void dur() {
  digitalWrite(MA1, LOW); digitalWrite(MA2, LOW);
  digitalWrite(MB1, LOW); digitalWrite(MB2, LOW);
}

long mesafeOlc(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long sure = pulseIn(echo, HIGH, 6000); 
  
  if (sure == 0) return 999; 
  return sure * 0.0343 / 2;
}