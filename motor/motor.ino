/*
 * ============================================================================
 * PROYECTO: Control de Actuadores (Ruedas + Rodillo N20) con Pulsadores
 * CURSO: Desafíos de la Ingeniería y Ciencias
 * ============================================================================
 * 
 * DESCRIPCIÓN:
 * Control de 3 motores DC mediante 2 botones con resistencias pull-up internas:
 * - Botón 1 (Pin 8): Activa el rodillo (Motor N20).
 * - Botón 2 (Pin 7): Activa las ruedas (2 Motores DC amarillos simultáneos).
 * - Reposo / Sin pulsar: Todos los motores apagados automáticamente.
 * 
 * DIAGRAMA DE PINES:
 * ----------------------------------------------------------------------------
 * Componente               Pin Arduino    Función
 * ----------------------------------------------------------------------------
 * PUENTE H 1 (Ruedas / Motores Amarillos):
 *   - ruedasIN1            Pin 9          Motor Izquierdo Dirección A
 *   - ruedasIN2            Pin 11         Motor Izquierdo Dirección B
 *   - ruedasIN3            Pin 12         Motor Derecho Dirección A
 *   - ruedasIN4            Pin 13         Motor Derecho Dirección B (*)
 * 
 * PUENTE H 2 (Rodillo / Motor N20):
 *   - rodilloIN1           Pin 3          Motor Rodillo Dirección A
 *   - rodilloIN2           Pin 4          Motor Rodillo Dirección B
 * 
 * BOTONES (Conexión a GND, usando INPUT_PULLUP):
 *   - Botón Rodillo        Pin 8          Pulsador Rodillo (LOW = presionado)
 *   - Botón Ruedas         Pin 7          Pulsador Ruedas  (LOW = presionado)
 * 
 * (*) NOTA SOBRE PIN 13:
 * El Pin 13 comparte línea con el LED integrado de la placa. Al reiniciar el
 * Arduino, puede parpadear levemente. Si notas un pequeño tirón al encender,
 * puedes mover ruedasIN4 al Pin 5 o Pin 10 (libres).
 * ============================================================================
 */

// --- PINES PUENTE H 1 (Motores Amarillos / Ruedas) ---
const int PIN_RUEDAS_IN1 = 9;
const int PIN_RUEDAS_IN2 = 11;
const int PIN_RUEDAS_IN3 = 12;
const int PIN_RUEDAS_IN4 = 13;

// --- PINES PUENTE H 2 (Motor N20 / Rodillo) ---
const int PIN_RODILLO_IN1 = 3;
const int PIN_RODILLO_IN2 = 4;

// --- PINES BOTONES ---
const int PIN_BOTON_RODILLO = 8; // Pulsador rodillo
const int PIN_BOTON_RUEDAS  = 7; // Pulsador ruedas

// Variables para telemetría y detección de cambios de estado
enum EstadoSistema { ESTADO_REPOSO, ESTADO_RODILLO, ESTADO_RUEDAS };
EstadoSistema estadoAnterior = ESTADO_REPOSO;

void setup() {
  // 1. Configurar pines de motores como salidas
  pinMode(PIN_RUEDAS_IN1, OUTPUT);
  pinMode(PIN_RUEDAS_IN2, OUTPUT);
  pinMode(PIN_RUEDAS_IN3, OUTPUT);
  pinMode(PIN_RUEDAS_IN4, OUTPUT);

  pinMode(PIN_RODILLO_IN1, OUTPUT);
  pinMode(PIN_RODILLO_IN2, OUTPUT);

  // 2. Apagar todos los motores al inicio por seguridad
  detenerTodo();

  // 3. Activar resistencias PULL-UP internas para los botones
  // (Conexión: Pin digital -> Pulsador -> GND. Sin necesidad de resistencias externas)
  pinMode(PIN_BOTON_RODILLO, INPUT_PULLUP);
  pinMode(PIN_BOTON_RUEDAS, INPUT_PULLUP);

  // 4. Telemetría serial para diagnóstico y presentación
  Serial.begin(9600);
  Serial.println(F("=================================================="));
  Serial.println(F("  SISTEMA DE CONTROL DE MOTORES Y BOTONES LISTO   "));
  Serial.println(F("   Pin 8: Rodillo (N20) | Pin 7: Ruedas (mBot)   "));
  Serial.println(F("=================================================="));
  Serial.println(F("[ESTADO] Reposo - Todos los motores detenidos."));
}

void loop() {
  // Leer botones (INPUT_PULLUP: LOW = Presionado, HIGH = Suelto)
  bool rodilloPresionado = (digitalRead(PIN_BOTON_RODILLO) == LOW);
  bool ruedasPresionado  = (digitalRead(PIN_BOTON_RUEDAS) == LOW);

  // --------------------------------------------------------------------------
  // LÓGICA DE CONTROL DE ACCIÓN
  // --------------------------------------------------------------------------
  if (rodilloPresionado) {
    activarRodillo();
    if (estadoAnterior != ESTADO_RODILLO) {
      Serial.println(F("[ACCION] Boton Rodillo presionado -> Activando Motor N20"));
      estadoAnterior = ESTADO_RODILLO;
    }
  } 
  else if (ruedasPresionado) {
    activarRuedas();
    if (estadoAnterior != ESTADO_RUEDAS) {
      Serial.println(F("[ACCION] Boton Ruedas presionado -> Activando Motores Amarillos"));
      estadoAnterior = ESTADO_RUEDAS;
    }
  } 
  else {
    detenerTodo();
    if (estadoAnterior != ESTADO_REPOSO) {
      Serial.println(F("[ACCION] Botones liberados -> Motores en reposo"));
      estadoAnterior = ESTADO_REPOSO;
    }
  }

  // Pequeño retardo de estabilidad (antirrebote / debounce)
  delay(15);
}

// ============================================================================
// FUNCIONES AUXILIARES DE CONTROL MODULAR
// ============================================================================

/**
 * Enciende el motor N20 (Rodillo) y asegura que las ruedas estén apagadas.
 */
void activarRodillo() {
  // Asegurar ruedas apagadas
  digitalWrite(PIN_RUEDAS_IN1, LOW);
  digitalWrite(PIN_RUEDAS_IN2, LOW);
  digitalWrite(PIN_RUEDAS_IN3, LOW);
  digitalWrite(PIN_RUEDAS_IN4, LOW);

  // Encender rodillo
  digitalWrite(PIN_RODILLO_IN1, HIGH);
  digitalWrite(PIN_RODILLO_IN2, LOW);
}

/**
 * Enciende las ruedas (Motores amarillos) y asegura que el rodillo esté apagado.
 */
void activarRuedas() {
  // Asegurar rodillo apagado
  digitalWrite(PIN_RODILLO_IN1, LOW);
  digitalWrite(PIN_RODILLO_IN2, LOW);

  // Encender ambas ruedas hacia adelante
  digitalWrite(PIN_RUEDAS_IN1, HIGH);
  digitalWrite(PIN_RUEDAS_IN2, LOW);
  digitalWrite(PIN_RUEDAS_IN3, HIGH);
  digitalWrite(PIN_RUEDAS_IN4, LOW);
}

/**
 * Detiene todos los motores del sistema inmediatamente.
 */
void detenerTodo() {
  digitalWrite(PIN_RODILLO_IN1, LOW);
  digitalWrite(PIN_RODILLO_IN2, LOW);
  digitalWrite(PIN_RUEDAS_IN1, LOW);
  digitalWrite(PIN_RUEDAS_IN2, LOW);
  digitalWrite(PIN_RUEDAS_IN3, LOW);
  digitalWrite(PIN_RUEDAS_IN4, LOW);
}
