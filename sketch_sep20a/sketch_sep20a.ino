#include <Arduino.h>

// Definición de pines para la matriz LED
#define COL_8 2
#define COL_7 3
#define ROW_2 4
#define COL_1 5
#define ROW_4 6
#define COL_6 7
#define COL_4 8
#define ROW_1 9

#define ROW_5 10
#define ROW_7 11
#define COL_2 12
#define COL_3 13
#define ROW_8 A5
#define COL_5 A4
#define ROW_6 A3
#define ROW_3 A2

// Pin para el botón
#define BOTON_PIN 1

#define RETARDO_APAGADO_GRADUAL 10

// Definir los pines de fila y columna para tu matriz LED de ánodo común
int PIN_FILA[] = {ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8};
int PIN_COLUMNA[] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8};

void setup() {
  // Configurar pines de la matriz LED como salidas
  for (int i = 0; i < 8; i++) {
    pinMode(PIN_FILA[i], OUTPUT);
    pinMode(PIN_COLUMNA[i], OUTPUT);
  }

  // Configurar pin del botón como entrada
  pinMode(BOTON_PIN, INPUT);

  // Inicializar Serial para la comunicación con el programa C++
  Serial.begin(9600);

  // Inicializar otros aspectos de tu programa aquí
}

void loop() {
  // Leer el estado del botón
  int botonState = digitalRead(BOTON_PIN);

  if (botonState == LOW) { // El botón ha sido presionado
    // Enviar la palabra "play" a través del puerto serie
    Serial.println("play");
  }

  // Leer el número enviado a través del puerto serie
  if (Serial.available() > 0) {
    int numero = Serial.parseInt(); // Lee el número recibido

    // Enciende la matriz LED completa
    encenderMatriz();

    // Espera el tiempo especificado antes de apagar gradualmente
    delay(numero * 1000); // Multiplica por 1000 para convertir a milisegundos

    // Apagar gradualmente la matriz LED
    apagarMatriz();
  }

  // Otras acciones que desees realizar en el bucle loop
}

// Función para encender toda la matriz LED
void encenderMatriz() {
  for (int fila = 0; fila < 8; fila++) {
    digitalWrite(PIN_FILA[fila], LOW); // Enciende la fila (ánodo común, LOW)
    for (int columna = 0; columna < 8; columna++) {
      digitalWrite(PIN_COLUMNA[columna], HIGH); // Enciende la columna (cátodo común, HIGH)
    }
  }
}

// Función para apagar gradualmente la matriz LED
void apagarMatriz() {
  for (int fila = 0; fila < 8; fila++) {
    digitalWrite(PIN_FILA[fila], HIGH); // Apaga la fila (ánodo común, HIGH)
    for (int columna = 0; columna < 8; columna++) {
      digitalWrite(PIN_COLUMNA[columna], LOW); // Apaga la columna (cátodo común, LOW)
      delayMicroseconds(RETARDO_APAGADO_GRADUAL); // Retardo muy corto para apagado gradual
    }
  }
}
