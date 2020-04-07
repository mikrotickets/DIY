/*  *Copyright (c) 2020 MikroTickets

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*-----( Declare Libraries )-----*/
#include <SPI.h>
#include <SD.h> //Libreria del SD shield card reader
#include <LiquidCrystal_I2C.h> //Libreria para LCD 16x2 

LiquidCrystal_I2C lcd(0x27 , 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//Modified these  lines according to you language NO MORE THAN 16 Characters per line:
const char string_0[] PROGMEM = "    Wellcome";   //"   Bienvenido";
const char string_1[] PROGMEM = " Internet  Sale";   //" Venta Internet";
const char string_2[] PROGMEM = " You can insert";   //"  Para comprar";
const char string_3[] PROGMEM = "   coins  now"; //"deposita monedas";
const char string_4[] PROGMEM = " Coin"; //"Monedas";
const char string_5[] PROGMEM = "    Inserted"; //"  introducidas";
const char string_6[] PROGMEM = "User:    "; //"Usuario:  ";
const char string_7[] PROGMEM = "Password: ";//"Contrasena: ";
const char string_8[] PROGMEM ="Access with this"; // "Accede con este:";
const char string_9[] PROGMEM = "PIN:";
const char string_10[] PROGMEM = "Out or Service"; //"Fuera/servicio";
const char string_11[] PROGMEM = "Error: ";

const char *const string_table[] PROGMEM = {string_0, string_1, string_2, string_3, string_4, string_5 , string_6, string_7, string_8, string_9, string_10, string_11};
char buffer[16];  //Handle the above  strings on progmen, this is large enough for the 16 character LCD space

//Global Variables
#define coinPulse 2 //Pin conected to COIN SIGNAL from the coin aceptor (white wire)
#define pushButton 6 //Pin conected to push Buttom
byte button = 0; // variable to change state detection.
byte old_button = 0; // variable to change state detection
byte Coun = 0; //Global counter for inserted coins and to biuld file names
unsigned int timer = 0; //Global tiner to trigger  lcd mesagges and wait times
String xuser; //String to store the user or first space before comma.
String ypassword; //String to strore  the password from csv after comma.
long indx = 0; // store the las index position to search  the next line of users on csvs
long btotal = 0;// strore the total of profile avalible. 
String filename;
String indexname;
byte idCode = 1; //Error code to show on lcd, example: ERROR: 21 (meand all the user from profiles 1.csv has been sold) ERROR 30: wrong files on sd.
byte filecount = 0; //Number of files founs must be un pairs, #.csv and INDEX#.csv
byte lastProfile = 0; //store the highest numeric value of the file name found to trigger automatic sale since there is no higer user file to sell
byte val[4];// Array to store the profile numbers, keep it 4 if you what to see the list of remaining user avalible to sell on LCD at Boot
byte j = 0;
boolean OKcomprar = false; //flag to indicate there not error and ready to sell
byte diagnostic = 1; //flag to run specific read file function only at boot


File file;
// This next 3 function reads the csv files values

bool readLine(File &f, char* line, size_t maxLen) {
  for (size_t n = 0; n < maxLen; n++) {
    int c = f.read();
    if ( c < 0 && n == 0) return false;  // EOF
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // Line to long
}


bool readVals(long* v1, long* v2) {
  char line[20], *ptr, *str;
  if (!readLine(file, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // wrong number if equal
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v2 = strtol(ptr, &str, 10);
  return str != ptr;  // true si es numero
}

bool readValsSTR(String* v1, String* v2) {
  char line[20];
  int comma = 0;
  String values;
  if (!readLine(file, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  for (byte i = 0; i < sizeof(line); i++)
  {
    values += (char)(line[i]);
  }
  comma = values.indexOf(',');
  *v1 = values.substring(0, comma);
  *v2 = values.substring(comma + 1, sizeof(line));
  return true;
}

//funciones de indexado
void index1() {
  String number = String(Coun);
  indexname = String("Index" + number + ".csv");
  // Serial.print("IndxNa:");
  // Serial.println(indexname);
  delay (10);
  file = SD.open(indexname, FILE_READ);
  while (readVals(&indx, &btotal)) {
    delay(1);
  }
  if (diagnostic) {
    //Serial.print("a: ");
    //Serial.println(indx);
    //Serial.print("b: ");
    //Serial.println(btotal);
    //Serial.println();
    if (indx >= btotal) {
      idCode = 2;
      errorCode();
      Serial.println("erroindex1");
      return;
    }
  }
  file.close();
  if (!diagnostic && OKcomprar && (Coun <= lastProfile) && (Coun > 0)) //
  {
    SD.remove(indexname);
    delay(10);
    file = SD.open(indexname, FILE_WRITE);
    long indexa = ++indx;
    file.print(indexa);
    file.print(",");
    file.println(btotal);
    file.close();
  }
}

bool CSVcheck(String filename)
{
  int len = filename.length();
  bool result;
  filename.toLowerCase();
  if (filename.endsWith(".csv")) //solo tomar en cuenta archivos CSV
  {
    result = false;
    filename.remove(len - 4); //quitamos extension ".csv" del nombre
    if (isDigit(filename[0])) //checamos si el nombre del archivo empieza con un numero y asumimos que todo es numerico, para mayor certeza se debe checar el nombre completo del archivo
    {
      int num = filename.toInt(); //convertimos el nombre del archivo de un string a un int
      val[j] = num; //lo guardamos en el array
      //lastProfile = num;
      j++; //incrementamos el indice del array
      result = true;
    }
  }
  else
  {
    result = false;
  }
  return result;
}

// Scan only csv files to count them, store them into the array val[4] and sort them
void printDirectory()
{
  filecount = 0;
  file = SD.open("/");
  while (true)
  {
    File entry =  file.openNextFile();
    if (!entry)
    {
      //no more files
      break;
    }
    if (CSVcheck(entry.name()))
    {
      filecount++;
    }
    entry.close();
  }
  //Serial.print("Total desired files found: ");Serial.println(filecount);
  sort(val, j); //ordenamos el arreglo val y usamos count para saber su tamaño
  for (unsigned char i = 0; i < j; i++) //imprimimos el arreglo ya ordenado de mayor a menor
  { if (i == 0) {
      lastProfile = val[i];
    }

    //Serial.println(val[i]);
  }

  file.close();
  delay (10);
  return;
}

// sort the numeric files names to detect the last profice withthe higest number to trigger automatic sell since there not more profiles to sell.
void sort(unsigned char values[], unsigned char length)
{
  int i, j, flag = 1;
  int temp;
  for (i = 1; (i <= length) && flag; i++)
  {
    flag = 0;
    for (j = 0; j < (length - 1); j++)
    {
      if (values[j + 1] > values[j])
      {
        temp = values[j];
        values[j] = values[j + 1];
        values[j + 1] = temp;
        flag = 1;
      }
    }
  }
}

void remainUsers() {
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < 4; i++)
  {
    if (val[i] > 0) {
      Coun = val[i];
      index1(); ;
      lcd.print F(("P"));
      lcd.print(Coun);
      lcd.print F((":"));
      lcd.print((btotal - indx));
      lcd.print F((" "));
    }
    if (i == 1) {
      lcd.setCursor(0, 1);
    }
  }
  delay (5000);
  return;
}

void setup()
{
  Serial.begin(9600);
  pinMode(pushButton, INPUT_PULLUP); // Pin conected to push button  to trigger buy and clear screen.
  //digitalWrite(pushButton, HIGH);//Add pull up resistor function to pin
  pinMode(coinPulse, INPUT_PULLUP);//Pin conected to COIN SIGNAL wire (use a diode to improve response)
  //digitalWrite(coinPulse, HIGH); //Add pull up resistor function to pin
  pinMode(5, OUTPUT); //Pin to trigger GDN signal to pin 4
  digitalWrite(5, LOW); //Pin to trigger GDN signal to pin 4
  lcd.begin();        // initialize the lcd for 20 chars 4 lines, turn on backlight
  lcd.backlight(); // turn led on  some i2c adapter
  if (!SD.begin(4)) { //init the SD card
    Serial.println F(("Sd error"));
    errorCode();
    return;
  }
  else
    idCode = 0;
  Serial.println F(("SD OK"));
  Serial.println();
  printDirectory();
  Serial.print F(("# CSV files found: "));
  Serial.println(filecount);
  Serial.print F(("# last file: "));
  Serial.println(lastProfile);
  Serial.println();
  if (filecount % 2) {
    idCode = 3;  //verifica que no vengan archivos ocultos por virus en la sd si arroja un numero non
    errorCode();
    return;
  }

  for (int i = 0; i < 9; i++)
  {
    if (val[i] > 0) {
      // Serial.print F(("F"));
      // Serial.println(val[i]);
      Coun = val[i];
      index1();
    }
  }
  diagnostic = 0; //cambia la timeriable de diagnostico en el indexador de profiles
  if (!idCode) {
    Registro();
    remainUsers();
    buyMsg();
  }
  delay (10);
}

int getButton()
{
  byte buttonState = digitalRead(pushButton);
  byte buttonStateCoin = digitalRead(coinPulse);
  if (!buttonState) button = 1;
  else if (!buttonStateCoin) button = 2;
  else button = 0;
  return button;
}

//funciones de lectura de usuario
void profile1()
{
  String number = String(Coun);
  filename = String( number + ".csv");
  delay (10);
  Serial.print F("Profile filename: ");
  Serial.println (filename);
  file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.println F("No profile file");
    return;
  }
  int count = 0;
  while (readValsSTR(&xuser, &ypassword)) {
    count++;
    delay(1);
    if (count == indx) {
      break;
    }
  }
  Serial.print F("User: ");
  Serial.println(xuser);
  Serial.print F("Password: ");
  Serial.println(ypassword);
  Serial.println F(("Done"));
  Serial.println F(("--------------------------"));
  file.close();
}


//funciones para la impresion de usuario y passwor en impresora termica

void errorCode()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print F(("Fuera de servicio"));
  lcd.setCursor(0, 1);
  lcd.print F(("Error: "));
  lcd.print(idCode);
  lcd.print(Coun);
  delay (10);
  return;
}


void Registro()
{
  //digitalWrite(pulseLED, LOW);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print F(("MikroTickets"));
  lcd.setCursor(6, 1);
  lcd.print F(("DIY"));
  Serial.println F("MikroTickets DIY");
  delay (2000);
  return;
}


void WellcomeMsg()
{
  clearMem();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[0]))));
  lcd.setCursor(0, 1);
  lcd.print(strcpy_P(buffer, (char *)pgm_read_word(&(string_table[1]))));
}

void buyMsg()
{
  clearMem();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[2]))));
  lcd.setCursor(0, 1);
  lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[3]))));
  delay (50);
  return;
}

void clearMem() {
  if (indx >= btotal ) {
    idCode = 2;
    errorCode();
    return;
  }

  xuser = "0";
  ypassword = "0";
  Coun = 0;
}


void displayCount()
{

  if (Coun == 1 ) {
    lcd.clear();
  }
  lcd.setCursor(5, 0);
  lcd.print(Coun);
  lcd.setCursor(7, 0);
  lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[4]))));
  lcd.setCursor(0, 1);
  lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[5]))));
  return;
}

void loop()  {
  if (!idCode) { // entra solo si no hay codigo de error detectado
    if (Coun == 0) {
      timer = timer + 1;
      switch (timer) {
        case 125:
          WellcomeMsg();
          break;
        case 250:
          timer = 0;
          buyMsg();
          break;
        default:
          break;
      }
      delay (20);
    }
    button = getButton();
    if (button != old_button)
    {
      delay(10);        // debounce
      button = getButton();
      if (button != old_button) //This is a second  getButtom to avoid static from the electronic coin give  a false coin detection
      {
        //delay(50);
        old_button = button;
        switch (button) {
          case 1:
            if (Coun > 0 && OKcomprar) {
comprar:
              index1(); //funciones de indexado
              profile1(); //funciones de lectura de usuario
              //verifica que los usuarios/password no sean 0 de ser asi se desabilita la venta con el error en pantalla 2:
              if (xuser == "0") {
                idCode = 2;  // en caso de lectura de profile/index o pin
                Coun = 0;
                Serial.println F("eror xuser  0");
                errorCode();
                return;
              }
              delay (100);
              lcd.clear();
              lcd.setCursor(0, 0);
              int ypasswordint = (ypassword.toInt());
              if (!ypasswordint == 0) {
                lcd.setCursor(0, 0);
                lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[6]))));
                lcd.print(xuser);
                lcd.setCursor(0, 1);
                lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[7]))));
                ypassword = ypassword.substring(0, ypassword.length() - 1);// weird arrow appears only in this variable at the last character
                lcd.print(ypassword);
              }
              else
              {
                lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[7]))));
                lcd.setCursor(2, 1);
                lcd.print (strcpy_P(buffer, (char *)pgm_read_word(&(string_table[9]))));
                lcd.setCursor(7, 1);
                lcd.print(xuser);
                delay (500);

              }
              delay (4000);
              timer = 9000;
              while (timer > 1) { // le da tiempo a que lea el display pero puede borrar el user presionando el boton comprar ( pressed_button == 1)
                timer--;
                button = getButton();
                if (button == 1) {
                  Coun = 0;
                  timer = 0;
                  WellcomeMsg();
                  break;
                }
                if (button == 2) {
                  Coun = 1;
                  displayCount();
                  break;
                }
                delay (10);
              }
              Coun = 0;
            }
            else {
              lcd.begin();        // initialize the lcd for 20 chars 4 lines, turn on backlight
              lcd.backlight();
              buyMsg();
            }
            break;
          case 2:
            OKcomprar = false;
            Coun++;
            for (int i = 0; i < 9; i++)
            {
              if (Coun == (val[i])) {
                OKcomprar = true;
                break;
              }
            }
            displayCount();
            if (Coun == lastProfile) {
              delay (750);
              goto comprar;
              break;
            }
            break;
          default:
            break;
        }
      }
    }
  }
}

