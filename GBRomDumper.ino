int wr = 22; //write pin
int rd = 23; //read pin
int cs = 24; //chip select
int reset = 25; //reset pin
int phi = 26;
int mbc = -1;

int addr[16]; //addr pins (A0-A15)
int data[8]; //data pins (D0-D7)

void setup() {
  for (int i = 28; i <= 43; i++) { //set the addr pins and init them as outputs
    addr[i - 28] = i;
    pinMode(i, OUTPUT);
  }

  for (int i = 44; i <= 51; i++) { //set the data pins and init them as inputs
    data[i - 44] = i;
    pinMode(i, INPUT);
  }

  pinMode(rd, OUTPUT);
  pinMode(wr, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(reset, OUTPUT);
  //pinMode(phi, OUTPUT);
  digitalWrite(wr, HIGH);
  digitalWrite(rd, LOW);
  digitalWrite(cs, HIGH);
  //digitalWrite(phi, LOW);
  //digitalWrite(reset, HIGH);
  //delay(10);
  //pinMode(reset, INPUT_PULLUP);
  Serial.begin(250000);
}

void loop() {
  mbc = getMBCType();
  int banks = getNumberOfBanks();

  //send info over serial to dump receiver
  Serial.println("%" + getCartTitle() + "%");
  Serial.println("%" + String(banks) + "%");
  Serial.println("START");

  for (unsigned int i = 0; i <= banks; i++) {
      readBank(i);
    }
  Serial.println("END");
  delay(1000 * 30);
}

int getByte(unsigned int address) {
  setAddr(address);
  int sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += digitalRead(data[i]) << i ;
  }

  return sum;
}

void setAddr(unsigned int address) {
  for (int i = 0; i < 16; i++) {
    digitalWrite(addr[i], (address >> i) & 0B0000000000000001);
  }
}

void setROMRead() {
  digitalWrite(wr, HIGH);
  digitalWrite(rd, LOW);
}

void setROMWrite() {
  digitalWrite(wr, LOW);
  digitalWrite(rd, HIGH);
}

String getCartTitle() {
  String name = "";
  for (int i = 0x134; i < 0x140; i++) {
    int buff = getByte(i);
    if(buff != 0) {
      name += (char)getByte(i);
    }
  }
  return name;
}

int getMBCType() {
  int mbc = getByte(0x147);
  if(mbc == 0x0) {
    return 0;
  } else if(mbc == 0x1 || mbc == 0x2 || mbc == 0x3) {
    return 1;
  } else if(mbc == 0x5 || mbc == 0x6) {
    return 2;
  } else if(mbc == 0xF || mbc == 0x10 || mbc == 0x11 || mbc == 0x12 || mbc == 0x13) {
    return 3; 
  } else if(mbc == 0x19 || mbc == 0x1A || mbc == 0x1B || mbc == 0x1C || mbc == 0x1D || mbc == 0x1E) {
    return 5;
  } else if(mbc == 0x20) { return 6;}
  else if(mbc == 0x22) {return 7;}
  else{ return -1;}
}

void switchBank(int bank) {
  setAddr(0x2100);
  setROMWrite();
  for (int i = 0; i < 7; i++) {
    pinMode(data[i], OUTPUT);
    digitalWrite(data[i], (bank >> i) & 0B00000001); //write the bank number to MBC registers
  }
  delay(5);
  for (int i = 0; i < 7; i++) {
    digitalWrite(data[i], 0);
    pinMode(data[i], INPUT);
  }
  setROMRead();
}

void readBank(int bank) {
  if (bank == 0) {
    for (unsigned int i = 0x0; i <= 0x3FFF; i++) {
      Serial.println(getByte(i));
    }
  } else {
    switchBank(bank);
    for (unsigned int i = 0x4000; i <= 0x7FFF; i++) {
      Serial.println(getByte(i));
    }
  }
}

int getNumberOfBanks() {
  int romSize = getByte(0x148);
  if (romSize == 0x00) {
    return 0;
  }
  else if (romSize == 0x01) {
    return 4;
  }
  else if (romSize == 0x02) {
    return 8;
  }
  else if (romSize == 0x03) {
    return 16;
  }
  else if (romSize == 0x04) {
    return 32;
  }
  else if (romSize == 0x05) {
    return 64;
  }
  else if (romSize == 0x06) {
    return 128;
  }
  else {
    return -1;
  }
}
