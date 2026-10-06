/*
 * ============================================================================
 * PROYECTO: Control de Motor N20 con Driver HW-095 (L298N)
 * CURSO: Desafíos de la Ingeniería y Ciencias
 * ============================================================================
 * 
 * GUÍA DE CONEXIÓN RÁPIDA (¡Modo 2 Cables - No toques los jumpers!):
 * ----------------------------------------------------------------------------
 * 1. Deja el JUMPER NEGRO puesto en ENA (así el motor siempre está habilitado).
 * 2. Ignora por completo IN3, IN4 y ENB (son para un segundo motor que no tienen).
 * 
 * CABLES DE CONTROL:
 * HW-095 IN1  ----->  Arduino Pin 9  (PWM)
 * HW-095 IN2  ----->  Arduino Pin 10 (PWM)
 * 
 * MOTOR N20:
 * OUT1        ----->  Terminal 1 de Motor N20
 * OUT2        ----->  Terminal 2 de Motor N20
 * 
 * ALIMENTACIÓN (Modo prueba con cable USB de Arduino):
 * HW-095 +12V ----->  Arduino Pin 5V
 * HW-095 GND  ----->  Arduino GND (¡Masa común!)
 * ============================================================================
 */

// --- Pines PWM de Control (Timer1 de Arduino Uno) ---
const int PIN_IN1 = 9;   // Entrada 1 HW-095 (Control de velocidad y giro)
const int PIN_IN2 = 10;  // Entrada 2 HW-095 (Control de velocidad y giro)

// --- Parámetros de Operación ---
const int VELOCIDAD_MINIMA = 70;  // PWM mínimo para vencer la fricción del reductor
const int VELOCIDAD_MAXIMA = 255; // Potencia máxima (100% PWM)

bool modoAutomatico = true;
int velocidadActual = 200;

void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);

  detenerMotor();

  Serial.begin(9600);
  while (!Serial && millis() < 1000);

  imprimirEncabezado();
}

void loop() {
  // 1. Escuchar comandos desde el Monitor Serial
  if (Serial.available() > 0) {
    char comando = Serial.read();
    procesarComandoSerial(comando);
  }

  // 2. Ejecutar rutina automática para demostración
  if (modoAutomatico) {
    ejecutarSecuenciaDemo();
  }
}

// ============================================================================
// FUNCIONES DE CONTROL
// ============================================================================

/**
 * Gira hacia adelante con velocidad controlada por PWM.
 */
void motorAdelante(int velocidad) {
  velocidad = constrain(velocidad, 0, 255);
  analogWrite(PIN_IN1, velocidad);
  digitalWrite(PIN_IN2, LOW);
}

/**
 * Gira en reversa con velocidad controlada por PWM.
 */
void motorAtras(int velocidad) {
  velocidad = constrain(velocidad, 0, 255);
  digitalWrite(PIN_IN1, LOW);
  analogWrite(PIN_IN2, velocidad);
}

/**
 * Detiene el motor (Freno suave / Corte de señal).
 */
void detenerMotor() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
}

/**
 * Freno activo (Ambos terminales a HIGH).
 */
void frenoActivo() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, HIGH);
}

// ============================================================================
// SECUENCIA DE DEMOSTRACIÓN AUTOMÁTICA
// ============================================================================
void ejecutarSecuenciaDemo() {
  Serial.println(F("\n>>> [DEMO] INICIANDO CICLO DE DEMOSTRACION <<<"));

  // 1. Aceleración progresiva adelante
  Serial.println(F("[1/6] ADELANTE: Rampa de aceleracion progresiva (PWM 70 -> 255)..."));
  for (int pwm = VELOCIDAD_MINIMA; pwm <= VELOCIDAD_MAXIMA; pwm += 5) {
    if (verificarInterrupcion()) return;
    motorAdelante(pwm);
    delay(35);
  }

  // 2. Velocidad crucero adelante
  Serial.println(F("[2/6] Manteniendo velocidad maxima adelante (2 segundos)..."));
  for (int i = 0; i < 20; i++) {
    if (verificarInterrupcion()) return;
    delay(100);
  }

  // 3. Desaceleración
  Serial.println(F("[3/6] Desacelerando a reposo..."));
  for (int pwm = VELOCIDAD_MAXIMA; pwm >= VELOCIDAD_MINIMA; pwm -= 5) {
    if (verificarInterrupcion()) return;
    motorAdelante(pwm);
    delay(25);
  }
  detenerMotor();
  Serial.println(F("[PAUSA] Motor en reposo por 1.5s..."));
  for (int i = 0; i < 15; i++) {
    if (verificarInterrupcion()) return;
    delay(100);
  }

  // 4. Aceleración en reversa
  Serial.println(F("[4/6] REVERSA: Rampa de aceleracion progresiva (PWM 70 -> 255)..."));
  for (int pwm = VELOCIDAD_MINIMA; pwm <= VELOCIDAD_MAXIMA; pwm += 5) {
    if (verificarInterrupcion()) return;
    motorAtras(pwm);
    delay(35);
  }

  // 5. Velocidad crucero en reversa
  Serial.println(F("[5/6] Manteniendo velocidad maxima reversa (2 segundos)..."));
  for (int i = 0; i < 20; i++) {
    if (verificarInterrupcion()) return;
    delay(100);
  }

  // 6. Desaceleración y freno
  Serial.println(F("[6/6] Desacelerando y aplicando FRENO ACTIVO..."));
  for (int pwm = VELOCIDAD_MAXIMA; pwm >= VELOCIDAD_MINIMA; pwm -= 5) {
    if (verificarInterrupcion()) return;
    motorAtras(pwm);
    delay(25);
  }
  frenoActivo();
  delay(120);
  detenerMotor();

  Serial.println(F(">>> [DEMO] CICLO COMPLETADO. Reiniciando en 3s... <<<"));
  for (int i = 0; i < 30; i++) {
    if (verificarInterrupcion()) return;
    delay(100);
  }
}

bool verificarInterrupcion() {
  return (Serial.available() > 0);
}

// ============================================================================
// CONSOLA SERIAL INTERACTIVA
// ============================================================================
void procesarComandoSerial(char c) {
  if (c == '\r' || c == '\n') return;

  switch (c) {
    case 'a': case 'A':
      modoAutomatico = true;
      Serial.println(F("\n[MODO] Secuencia Automatica ACTIVADA."));
      break;

    case 'f': case 'F':
      modoAutomatico = false;
      motorAdelante(velocidadActual);
      Serial.print(F("[MANUAL] Adelante con PWM = "));
      Serial.println(velocidadActual);
      break;

    case 'r': case 'R':
      modoAutomatico = false;
      motorAtras(velocidadActual);
      Serial.print(F("[MANUAL] Reversa con PWM = "));
      Serial.println(velocidadActual);
      break;

    case 's': case 'S':
      modoAutomatico = false;
      detenerMotor();
      Serial.println(F("[MANUAL] Motor DETENIDO."));
      break;

    case '+':
      velocidadActual = constrain(velocidadActual + 25, 0, 255);
      Serial.print(F("[VELOCIDAD] Incrementada a PWM = "));
      Serial.println(velocidadActual);
      break;

    case '-':
      velocidadActual = constrain(velocidadActual - 25, 0, 255);
      Serial.print(F("[VELOCIDAD] Reducida a PWM = "));
      Serial.println(velocidadActual);
      break;

    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
      velocidadActual = map(c - '0', 1, 9, 80, 255);
      Serial.print(F("[VELOCIDAD] Nivel "));
      Serial.print(c);
      Serial.print(F(" (PWM: "));
      Serial.print(velocidadActual);
      Serial.println(F(")"));
      break;

    case 'h': case 'H':
      imprimirEncabezado();
      break;

    default:
      Serial.print(F("[AVISO] Comando '"));
      Serial.print(c);
      Serial.println(F("' no reconocido. Envia 'h' para ayuda."));
      break;
  }
}

void imprimirEncabezado() {
  Serial.println(F("\n========================================================"));
  Serial.println(F("     SISTEMA DE CONTROL DE ACTUADOR N20 - HW-095        "));
  Serial.println(F("       Desafios de la Ingenieria y Ciencias             "));
  Serial.println(F("========================================================"));
  Serial.println(F(" COMANDOS SERIALES (9600 BAUD):                         "));
  Serial.println(F("   [A] - Modo Demo Automatico Continuo                  "));
  Serial.println(F("   [F] - Giro Adelante (Manual)                         "));
  Serial.println(F("   [R] - Giro Reversa (Manual)                          "));
  Serial.println(F("   [S] - Detener Motor                                  "));
  Serial.println(F("   [+] / [-] - Ajustar Velocidad                        "));
  Serial.println(F("   [1-9]     - Nivel de Potencia Directa                "));
  Serial.println(F("   [H] - Mostrar menu de ayuda                          "));
  Serial.println(F("========================================================\n"));
}
