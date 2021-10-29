/*
Puente h para los motores,
Sensores de color TCS3200,
MicroSevo Pendiente,
Sensor Utrasonido,
Pantalla LCD
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LCD.h>
#include <QMC5883LCompass.h>

//0:verde 1:rojo 2:azul 3: morado 4:amarillo 5:blanco 6:negro 7:rosa
String ordenColores[4] = {"R", "G", "B", "SC"};
String coloresLista[8] = {"verde", "rojo", "azul", "morado", "amarillo", "blanco", "negro", "rosa"};
int rangosColores[8][4][2] = {
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}},
    {{0, 1000},
     {0, 1000},
     {0, 1000},
     {0, 1000}}};
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
//Clase para nuestra pantalla LCD
class PantallaLCD
{
public:
    //Declaración de pines e inicializacion de la pantalla
    void setup()
    {
        lcd.setBacklightPin(3, POSITIVE);
        lcd.setBacklight(HIGH);
        lcd.begin(16, 2);
        lcd.clear();
    }
    //Funcion para escribir en la pantalla arriba
    void imprimirArriba(String texto)
    {
        lcd.setCursor(0, 0);
        lcd.print(texto);
    }
    //Funcion para escribir en la pantalla abajo
    void imprimirAbajo(String texto)
    {
        lcd.setCursor(0, 1);
        lcd.print(texto);
    }
    void limpiar()
    {
        lcd.clear();
    }
    void imprimirColores(String ColorIzquierda, String ColorDerecha)
    {
        lcd.setCursor(0, 0);
        lcd.print("Color Izquierda: ");
        lcd.print(ColorIzquierda);
        lcd.setCursor(0, 1);
        lcd.print("Color Derecha: ");
        lcd.print(ColorDerecha);
    }
    void ImprimirValoresArriba(int rojo, int verde, int azul, int sc)
    {
        imprimirArriba("R: " + String(rojo) + " V: " + String(verde) + " A: " + String(azul) + " SC: " + String(sc));
        Serial.println("IZq: R: " + String(rojo) + " V: " + String(verde) + " A: " + String(azul) + " SC: " + String(sc));
    }
    void ImprimirValoresAbajo(int rojo, int verde, int azul, int sc)
    {
        imprimirAbajo("R: " + String(rojo) + " G: " + String(verde) + " B: " + String(azul) + " Sc: " + String(sc));
        Serial.println("DER: R: " + String(rojo) + " G: " + String(verde) + " B: " + String(azul) + " Sc: " + String(sc));
    }
};
//Clase para nuestro sensor de color
class SensorColor
{
public:
    //Declaración de pines
    int pines[4];
    int out;
    //int output[4];

    //salida de colores
    //              R  G  B  SC
    int color[4] = {0, 0, 0, 0};
    int maximo[4] = {0, 0, 0, 0};
    int minimo[4] = {1000, 1000, 1000, 1000};
    String colorActual;
    String colorAnterior;

    // 4 pines un pin de output y 4 pines de entrada
    //Constructor
    //s0, s1, s2, s3, out
    SensorColor(int s0, int s1, int s2, int s3, int ou)
    {
        this->pines[0] = s0;
        this->pines[1] = s1;
        this->pines[2] = s2;
        this->pines[3] = s3;
        this->out = ou;
    };
    //Funcion para inicializar los pines
    void setup()
    {
        for (int i = 0; i < 4; i++)
        {
            pinMode(pines[i], OUTPUT);
        }
        pinMode(out, INPUT);
        digitalWrite(pines[0], HIGH);
        digitalWrite(pines[1], HIGH);
    }
    //Funcion para leer los colores
    void LecturaColor()
    {
        digitalWrite(pines[2], LOW);
        digitalWrite(pines[3], LOW);
        color[0] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
        digitalWrite(pines[3], HIGH);
        color[1] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
        digitalWrite(pines[2], HIGH);
        color[2] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
        digitalWrite(pines[3], LOW);
        color[3] = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
    }
    void Rangos()
    {
        for (int i = 0; i < 4; i++)
        {
            maximo[i] = max(maximo[i], color[i]);
            minimo[i] = min(minimo[i], color[i]);
        }
    }
    void AnexMatriz(int color)
    {
        for (int i = 0; i < 4; i++)
        {
            rangosColores[color][i][0] = max(minimo[i], rangosColores[color][i][0]);
            rangosColores[color][i][1] = min(maximo[i], rangosColores[color][i][1]);
        }
    }
    void Calibracion(PantallaLCD pantallaLCD)
    {
        pantallaLCD.imprimirArriba("Calibrando");
        delay(100);
        for (int i = 0; i < 8; i++)
        {
            pantallaLCD.limpiar();
            pantallaLCD.imprimirArriba("Color: " + coloresLista[i]);
            pantallaLCD.imprimirAbajo("5");
            delay(3000);
            pantallaLCD.imprimirAbajo("4");
            delay(3000);
            pantallaLCD.imprimirAbajo("3");
            delay(3000);
            pantallaLCD.imprimirAbajo("2");
            delay(3000);
            pantallaLCD.imprimirAbajo("1");
            delay(3000);
            pantallaLCD.imprimirAbajo("Calibrando");
            for (int j = 0; j < 4; j++)
            {
                maximo[j] = 0;
                minimo[j] = 1000;
            }
            for (int j = 0; j < 100; j++)
            {
                LecturaColor();
                Rangos();
            }
            AnexMatriz(i);
            Serial.println(i);
        }
    }
    //Funcion para leer el color FALTA REVISAR LOS VALORES Y AJUSTAR LOS COLORES
    void AnalisisColor()
    {
        if (color[0] < rangosColores[0][0][0] && color[0] > rangosColores[0][0][1] &&
            color[1] < rangosColores[0][1][0] && color[1] > rangosColores[0][1][1] &&
            color[2] < rangosColores[0][2][0] && color[2] > rangosColores[0][2][1] &&
            color[3] < rangosColores[0][3][0] && color[3] > rangosColores[0][3][1])
        {
            colorActual = "Verde";
        }
        else if (color[0] < rangosColores[1][0][0] && color[0] > rangosColores[1][0][1] &&
                 color[1] < rangosColores[1][1][0] && color[1] > rangosColores[1][1][1] &&
                 color[2] < rangosColores[1][2][0] && color[2] > rangosColores[1][2][1] &&
                 color[3] < rangosColores[1][3][0] && color[3] > rangosColores[1][3][1])
        {
            colorActual = "Rojo";
        }
        else if (color[0] < rangosColores[2][0][0] && color[0] > rangosColores[2][0][1] &&
                 color[1] < rangosColores[2][1][0] && color[1] > rangosColores[2][1][1] &&
                 color[2] < rangosColores[2][2][0] && color[2] > rangosColores[2][2][1] &&
                 color[3] < rangosColores[2][3][0] && color[3] > rangosColores[2][3][1])
        {
            colorActual = "Azul";
        }
        else if (color[0] < rangosColores[3][0][0] && color[0] > rangosColores[3][0][1] &&
                 color[1] < rangosColores[3][1][0] && color[1] > rangosColores[3][1][1] &&
                 color[2] < rangosColores[3][2][0] && color[2] > rangosColores[3][2][1] &&
                 color[3] < rangosColores[3][3][0] && color[3] > rangosColores[3][3][1])
        {
            colorActual = "Morado";
        }
        else if (color[0] < rangosColores[4][0][0] && color[0] > rangosColores[4][0][1] &&
                 color[1] < rangosColores[4][1][0] && color[1] > rangosColores[4][1][1] &&
                 color[2] < rangosColores[4][2][0] && color[2] > rangosColores[4][2][1] &&
                 color[3] < rangosColores[4][3][0] && color[3] > rangosColores[4][3][1])
        {
            colorActual = "Amarillo";
        }
        else if (color[0] < rangosColores[5][0][0] && color[0] > rangosColores[5][0][1] &&
                 color[1] < rangosColores[5][1][0] && color[1] > rangosColores[5][1][1] &&
                 color[2] < rangosColores[5][2][0] && color[2] > rangosColores[5][2][1] &&
                 color[3] < rangosColores[5][3][0] && color[3] > rangosColores[5][3][1])
        {
            colorActual = "Blanco";
        }
        else if (color[0] < rangosColores[6][0][0] && color[0] > rangosColores[6][0][1] &&
                 color[1] < rangosColores[6][1][0] && color[1] > rangosColores[6][1][1] &&
                 color[2] < rangosColores[6][2][0] && color[2] > rangosColores[6][2][1] &&
                 color[3] < rangosColores[6][3][0] && color[3] > rangosColores[6][3][1])
        {
            colorActual = "Negro";
        }
        else if (color[0] < rangosColores[7][0][0] && color[0] > rangosColores[7][0][1] &&
                 color[1] < rangosColores[7][1][0] && color[1] > rangosColores[7][1][1] &&
                 color[2] < rangosColores[7][2][0] && color[2] > rangosColores[7][2][1] &&
                 color[3] < rangosColores[7][3][0] && color[3] > rangosColores[7][3][1])
        {
            colorActual = "Rosa";
        }
        else
        {
            colorActual = "NR";
        }
    }
    //Funcion para evaluar el cambio de color
    bool CambioColor()
    {
        if (colorActual != colorAnterior)
        {
            colorAnterior = colorActual;
            return true;
        }
        else
        {
            return false;
        }
    }
};
//Clase para nuestro puenteH
class PuenteH
{
public:
    //Declaración de pines
    int motorD[2] = {23, 22};
    int motorI[2] = {25, 24};
    int VelocidadesD = 6;
    int VelocidadesI = 5;
    //Funcion para inicializar los pines
    void setup()
    {
        for (int i = 0; i < 2; i++)
        {
            pinMode(motorD[i], OUTPUT);
            pinMode(motorI[i], OUTPUT);
        }
        pinMode(VelocidadesD, OUTPUT);
        pinMode(VelocidadesI, OUTPUT);
    }
    //Metodos de movimiento
    void Avanzar(int velocidad)
    {
        digitalWrite(motorD[0], HIGH);
        digitalWrite(motorD[1], LOW);
        digitalWrite(motorI[0], HIGH);
        digitalWrite(motorI[1], LOW);
        analogWrite(VelocidadesD, velocidad);
        analogWrite(VelocidadesI, velocidad);
    }
    void Retroceder(int velocidad)
    {
        digitalWrite(motorD[0], LOW);
        digitalWrite(motorD[1], HIGH);
        digitalWrite(motorI[0], LOW);
        digitalWrite(motorI[1], HIGH);
        analogWrite(VelocidadesD, velocidad);
        analogWrite(VelocidadesI, velocidad);
    }
    void Detener()
    {
        digitalWrite(motorD[0], LOW);
        digitalWrite(motorD[1], LOW);
        digitalWrite(motorI[0], LOW);
        digitalWrite(motorI[1], LOW);
        analogWrite(VelocidadesD, 0);
        analogWrite(VelocidadesI, 0);
    }
    void Derecha(int velocidad)
    {
        digitalWrite(motorD[0], LOW);
        digitalWrite(motorD[1], HIGH);
        digitalWrite(motorI[0], HIGH);
        digitalWrite(motorI[1], LOW);
        analogWrite(VelocidadesD, velocidad);
        analogWrite(VelocidadesI, velocidad);
    }
    void Izquierda(int velocidad)
    {
        digitalWrite(motorD[0], HIGH);
        digitalWrite(motorD[1], LOW);
        digitalWrite(motorI[0], LOW);
        digitalWrite(motorI[1], HIGH);
        analogWrite(VelocidadesD, velocidad);
        analogWrite(VelocidadesI, velocidad);
    }
};
//Clase para nuestro sensor de distancia
class SensorUtrasonido
{
public:
    //Declaración de pines
    int trigger;
    int echo;
    float distancia;
    //Constructor
    //trigger, echo
    SensorUtrasonido(int trig, int ec)
    {
        this->trigger = trig;
        this->echo = ec;
    }
    //Funcion para inicializar los pines
    void setup()
    {
        pinMode(trigger, OUTPUT);
        pinMode(echo, INPUT);
    }
    //Funcion para leer la distancia
    void Lectura()
    {
        digitalWrite(trigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigger, LOW);
        distancia = pulseIn(echo, HIGH) / 58.0;
        Serial.println(distancia);
    }
    //Funcion para evaluar la distancia
    void Pared()
    {
        if (distancia < 10)
        {
            Serial.println("Pared");
        }
    }
};

QMC5883LCompass compass;
class Brujula
{
public:
    float angulo;
    float up = 0;
    float down = 0;
    float left = 0;
    float right = 0;
    float front = 0;
    void setup()
    {
        compass.init();
        compass.setCalibration(-682, 440, -1028, 152, -568, 0);
        Calibrar();
    }
    void Calibrar()
    {
        for (int i = 0; i != 10; i++)
        {
            compass.read();
            angulo = compass.getAzimuth();
            Serial.println(angulo);
            up = up + angulo;
            delay(50);
        }
        up = up / 10;
        front = up;
        right = round(up + 90.0) % 360;
        down = round(up + 180.0) % 360;
        left = round(up + 270.0) % 360;
    }
    void Lectura()
    {
        compass.read();
        angulo = compass.getAzimuth();
    }
};

//Declaración de componentes

SensorColor sensorColorDerecha(0, 1, 2, 3, 4);
SensorColor sensorColorIzquierda(30, 31, 44, 45, 46);
PuenteH puenteH;
SensorUtrasonido sensorUtrasonidoEnfrente(37, 36);
SensorUtrasonido sensorUtrasonidoIzquierda(7, 8);
SensorUtrasonido sensorUtrasonidoDerecha(38, 39);
Brujula brujula;
PantallaLCD pantallaLCD;
//Ejecutación del robot

//Funciones Globales
void setup()
{
    Serial.begin(9600);
    sensorColorDerecha.setup();
    sensorColorIzquierda.setup();
    puenteH.setup();
    sensorUtrasonidoEnfrente.setup();
    sensorUtrasonidoIzquierda.setup();
    sensorUtrasonidoDerecha.setup();
    pantallaLCD.setup();
    brujula.setup();
}
int cont = 0;
void CorregirAngulo(float front)
{
    brujula.Lectura();
    if (brujula.angulo < front)
    {
        while (brujula.angulo < front)
        {
            puenteH.Derecha(50);
            brujula.Lectura();
        }
    }
    else if (brujula.angulo > front)
    {
        while (brujula.angulo > front)
        {
            puenteH.Izquierda(50);
            brujula.Lectura();
        }
    }
    puenteH.Detener();
}
void AvanzarCorregido(int velocidad, float front)
{
    puenteH.Avanzar(velocidad);
    delay(100);
    CorregirAngulo(front);
}
void AvanzarHastaPared()
{
    Serial.println("AvanzarHastaPared");
    sensorUtrasonidoEnfrente.Lectura();
    Serial.println(sensorUtrasonidoEnfrente.distancia);
    float frenteMomentaneo = brujula.angulo;
    while (sensorUtrasonidoEnfrente.distancia > 5)
    {
        sensorUtrasonidoEnfrente.Lectura();
        brujula.Lectura();
        puenteH.AvanzarCorregido(150, frenteMomentaneo);
    }
    puenteH.Detener();
}
void GirarDerecha()
{
    Serial.println("GirarDerecha");
    brujula.Lectura();
    Serial.println(brujula.angulo);
    Serial.println("AAAA" + String(brujula.right));
    while (brujula.angulo <= brujula.right)
    {
        brujula.Lectura();
        Serial.println("A" + String(brujula.angulo));
        puenteH.Derecha(50);
    }
    puenteH.Detener();
}
void loop()
{

    while (cont < 1)
    {

        AvanzarHastaPared();
        brujula.Lectura();
        pantallaLCD.imprimirArriba(String(brujula.angulo));
        pantallaLCD.imprimirAbajo("YairGod");
        Serial.println("A" + String(brujula.angulo));

        GirarDerecha();
        cont++;
    }

    // puenteH.Avanzar(150);
}