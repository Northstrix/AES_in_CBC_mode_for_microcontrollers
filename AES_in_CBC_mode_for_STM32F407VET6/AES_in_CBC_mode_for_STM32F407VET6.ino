/*
AES_in_CBC_mode_for_microcontrollers
Based on the work of https://github.com/zhouyangchao/AES
Parts of the code are distributed under the MIT License
© Copyright Maxim Bortnikov 2024
For more information please visit
https://github.com/Northstrix/AES_in_CBC_mode_for_microcontrollers
https://sourceforge.net/projects/aes-in-cbc-mode-for-mcus/
*/
#include "aes.h"
#include "hal_conf_extra.h"
int m;

String string_for_data;
byte tmp_st[8];
int decract;
uint8_t array_for_CBC_mode[16];
uint8_t back_aes_key[32]; 
uint32_t aes_mode[3] = {128, 192, 256};

uint8_t aes_key[32] = {
0x01,0x02,0x03,0x04,
0x10,0x11,0x12,0x13,
0x50,0x51,0x52,0x53,
0x7a,0x7b,0x7c,0x7d,
0xa0,0xa1,0xa2,0xa3,
0xbb,0xcc,0xdd,0xee,
0xfc,0xfd,0xfe,0xff,
0x00,0xff,0x00,0xff
};

RNG_HandleTypeDef hrng;

/**
* @brief RNG MSP Initialization
* This function configures the hardware resources used in this example
* @param hrng: RNG handle pointer
* @retval None
*/
extern "C" void HAL_RNG_MspInit(RNG_HandleTypeDef* hrng) {
  /* Peripheral clock enable */
  __HAL_RCC_RNG_CLK_ENABLE();
}

/**
* @brief RNG MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hrng: RNG handle pointer
* @retval None
*/
extern "C" void HAL_RNG_MspDeInit(RNG_HandleTypeDef* hrng) {
  if (hrng->Instance == RNG) {
    /* Peripheral clock disable */
    __HAL_RCC_RNG_CLK_DISABLE();

    /* Enable RNG reset state */
    __HAL_RCC_RNG_FORCE_RESET();

    /* Release RNG from reset state */
    __HAL_RCC_RNG_RELEASE_RESET();
  }
}

void back_aes_k() {
  for (int i = 0; i < 32; i++) {
    back_aes_key[i] = aes_key[i];
  }
}

void rest_aes_k() {
  for (int i = 0; i < 32; i++) {
    aes_key[i] = back_aes_key[i];
  }
}

void incr_aes_key() {
  if (aes_key[15] == 255) {
    aes_key[15] = 0;
    if (aes_key[14] == 255) {
      aes_key[14] = 0;
      if (aes_key[13] == 255) {
        aes_key[13] = 0;
        if (aes_key[12] == 255) {
          aes_key[12] = 0;
          if (aes_key[11] == 255) {
            aes_key[11] = 0;
            if (aes_key[10] == 255) {
              aes_key[10] = 0;
              if (aes_key[9] == 255) {
                aes_key[9] = 0;
                if (aes_key[8] == 255) {
                  aes_key[8] = 0;
                  if (aes_key[7] == 255) {
                    aes_key[7] = 0;
                    if (aes_key[6] == 255) {
                      aes_key[6] = 0;
                      if (aes_key[5] == 255) {
                        aes_key[5] = 0;
                        if (aes_key[4] == 255) {
                          aes_key[4] = 0;
                          if (aes_key[3] == 255) {
                            aes_key[3] = 0;
                            if (aes_key[2] == 255) {
                              aes_key[2] = 0;
                              if (aes_key[1] == 255) {
                                aes_key[1] = 0;
                                if (aes_key[0] == 255) {
                                  aes_key[0] = 0;
                                } else {
                                  aes_key[0]++;
                                }
                              } else {
                                aes_key[1]++;
                              }
                            } else {
                              aes_key[2]++;
                            }
                          } else {
                            aes_key[3]++;
                          }
                        } else {
                          aes_key[4]++;
                        }
                      } else {
                        aes_key[5]++;
                      }
                    } else {
                      aes_key[6]++;
                    }
                  } else {
                    aes_key[7]++;
                  }
                } else {
                  aes_key[8]++;
                }
              } else {
                aes_key[9]++;
              }
            } else {
              aes_key[10]++;
            }
          } else {
            aes_key[11]++;
          }
        } else {
          aes_key[12]++;
        }
      } else {
        aes_key[13]++;
      }
    } else {
      aes_key[14]++;
    }
  } else {
    aes_key[15]++;
  }
}

size_t hex2bin(void * bin) {
  size_t len, i;
  int x;
  uint8_t * p = (uint8_t * ) bin;
  for (i = 0; i < 32; i++) {
    p[i] = (uint8_t) aes_key[i];
  }
  return 32;
}

int getNum(char ch) {
  int num = 0;
  if (ch >= '0' && ch <= '9') {
    num = ch - 0x30;
  } else {
    switch (ch) {
    case 'A':
    case 'a':
      num = 10;
      break;
    case 'B':
    case 'b':
      num = 11;
      break;
    case 'C':
    case 'c':
      num = 12;
      break;
    case 'D':
    case 'd':
      num = 13;
      break;
    case 'E':
    case 'e':
      num = 14;
      break;
    case 'F':
    case 'f':
      num = 15;
      break;
    default:
      num = 0;
    }
  }
  return num;
}

char getChar(int num) {
  char ch;
  if (num >= 0 && num <= 9) {
    ch = char(num + 48);
  } else {
    switch (num) {
    case 10:
      ch = 'a';
      break;
    case 11:
      ch = 'b';
      break;
    case 12:
      ch = 'c';
      break;
    case 13:
      ch = 'd';
      break;
    case 14:
      ch = 'e';
      break;
    case 15:
      ch = 'f';
      break;
    }
  }
  return ch;
}

void back_key() {
  back_aes_k();
}

void rest_key() {
  rest_aes_k();
}

void clear_variables() {
  string_for_data = "";
  decract = 0;
}

// aes in CBC Mode(Below)

void split_by_sixteen_for_encryption(char plntxt[], int k, int str_len) {
  int res[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };

  for (int i = 0; i < 16; i++) {
    if (i + k > str_len - 1)
      break;
    res[i] = plntxt[i + k];
  }

  for (int i = 0; i < 16; i++) {
    res[i] ^= array_for_CBC_mode[i];
  }
  
  encrypt_with_aes(res);
}

void encrypt_iv_for_aes(int iv[]) {
  for (int i = 0; i < 16; i++){
    array_for_CBC_mode[i] = iv[i];
  }
  
  encrypt_with_aes(iv);
}

void encrypt_with_aes(int to_be_encr[]) {
  uint8_t text[16];
  for(int i = 0; i < 16; i++){
    text[i] = to_be_encr[i];
  }
  uint8_t cipher_text[16];
  int i = 0;
  aes_context ctx;
  set_aes_key(&ctx, aes_key, aes_mode[m]);
  aes_encrypt_block(&ctx, cipher_text, text);
    incr_aes_key();
    /*
    for (int i = 0; i < 16; i++) {
      if (cipher_text[i] < 16)
        Serial.print("0");
      Serial.print(cipher_text[i], HEX);
    }
    */
    for (int i = 0; i < 16; i++) {
     if (decract > 0){
        if (i < 16){
          array_for_CBC_mode[i] = int(cipher_text[i]);
        }  
     }
     if (cipher_text[i] < 16)
        string_for_data += "0";
      string_for_data += String(cipher_text[i], HEX);
    }
    decract++;
}

void split_for_decr(char ct[], int ct_len, int p) {
  int br = false;
  int res[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };
  byte prev_res[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };
  for (int i = 0; i < 32; i += 2) {
    if (i + p > ct_len - 1) {
      br = true;
      break;
    }
    if (i == 0) {
      if (ct[i + p] != 0 && ct[i + p + 1] != 0)
        res[i] = 16 * getNum(ct[i + p]) + getNum(ct[i + p + 1]);
      if (ct[i + p] != 0 && ct[i + p + 1] == 0)
        res[i] = 16 * getNum(ct[i + p]);
      if (ct[i + p] == 0 && ct[i + p + 1] != 0)
        res[i] = getNum(ct[i + p + 1]);
      if (ct[i + p] == 0 && ct[i + p + 1] == 0)
        res[i] = 0;
    } else {
      if (ct[i + p] != 0 && ct[i + p + 1] != 0)
        res[i / 2] = 16 * getNum(ct[i + p]) + getNum(ct[i + p + 1]);
      if (ct[i + p] != 0 && ct[i + p + 1] == 0)
        res[i / 2] = 16 * getNum(ct[i + p]);
      if (ct[i + p] == 0 && ct[i + p + 1] != 0)
        res[i / 2] = getNum(ct[i + p + 1]);
      if (ct[i + p] == 0 && ct[i + p + 1] == 0)
        res[i / 2] = 0;
    }
  }

  for (int i = 0; i < 32; i += 2) {
    if (i + p - 32 > ct_len - 1) {
      br = true;
      break;
    }
    if (i == 0) {
      if (ct[i + p - 32] != 0 && ct[i + p - 32 + 1] != 0)
        prev_res[i] = 16 * getNum(ct[i + p - 32]) + getNum(ct[i + p - 32 + 1]);
      if (ct[i + p - 32] != 0 && ct[i + p - 32 + 1] == 0)
        prev_res[i] = 16 * getNum(ct[i + p - 32]);
      if (ct[i + p - 32] == 0 && ct[i + p - 32 + 1] != 0)
        prev_res[i] = getNum(ct[i + p - 32 + 1]);
      if (ct[i + p - 32] == 0 && ct[i + p - 32 + 1] == 0)
        prev_res[i] = 0;
    } else {
      if (ct[i + p - 32] != 0 && ct[i + p - 32 + 1] != 0)
        prev_res[i / 2] = 16 * getNum(ct[i + p - 32]) + getNum(ct[i + p - 32 + 1]);
      if (ct[i + p - 32] != 0 && ct[i + p - 32 + 1] == 0)
        prev_res[i / 2] = 16 * getNum(ct[i + p - 32]);
      if (ct[i + p - 32] == 0 && ct[i + p - 32 + 1] != 0)
        prev_res[i / 2] = getNum(ct[i + p - 32 + 1]);
      if (ct[i + p - 32] == 0 && ct[i + p - 32 + 1] == 0)
        prev_res[i / 2] = 0;
    }
  }
  
  if (br == false) {
    if(decract > 16){
      for (int i = 0; i < 16; i++){
        array_for_CBC_mode[i] = prev_res[i];
      }
    }
    uint8_t ret_text[16];
    uint8_t cipher_text[16];
    for(int i = 0; i<16; i++){
      cipher_text[i] = res[i];
    }
    uint32_t aes_mode[3] = {128, 192, 256};
    int i = 0;
    aes_context ctx;
    set_aes_key(&ctx, aes_key, aes_mode[m]);
    aes_decrypt_block(&ctx, ret_text, cipher_text);
    incr_aes_key();
    if (decract > 2) {
      for (int i = 0; i < 16; i++){
        ret_text[i] ^= array_for_CBC_mode[i];
      }
      
      for (i = 0; i < 16; ++i) {
        if (ret_text[i] > 0)
          string_for_data += char(ret_text[i]);
      }
    }

    if (decract == -1){
      for (i = 0; i < 16; ++i) {
        array_for_CBC_mode[i] = int(ret_text[i]);
      }
    }
    decract++;
  }
}

void encrypt_string_with_aes_in_cbc(String input, int iv[]) {
  back_key();
  clear_variables();
  encrypt_iv_for_aes(iv);
  int str_len = input.length() + 1;
  char input_arr[str_len];
  input.toCharArray(input_arr, str_len);
  int p = 0;
  while (str_len > p + 1) {
    split_by_sixteen_for_encryption(input_arr, p, str_len);
    p += 16;
  }
  rest_key();
}

void decrypt_string_with_aes_in_cbc(String ct) { // Function for aes. Takes ciphertext as an input.
  back_key();
  clear_variables();
  int ct_len = ct.length() + 1;
  char ct_array[ct_len];
  ct.toCharArray(ct_array, ct_len);
  int ext = 0;
  decract = -1;
  while (ct_len > ext) {
    split_for_decr(ct_array, ct_len, 0 + ext);
    ext += 32;
    decract += 10;
  }
  rest_key();
}

void encrypt_aes_from_Serial() {
  bool cont_to_next = false;
  while (cont_to_next == false) {
    Serial.println("\nPaste the string you want to encrypt here:");
    while (!Serial.available()) {
    }
    String plt = Serial.readString();
    int iv[16]; // Initialization vector
    for (int i = 0; i < 16; i++){
      iv[i] = random(256); // Fill iv array with random numbers. I suggest you use a more secure method of random number generation!!!
    }
    encrypt_string_with_aes_in_cbc(plt, iv); // Function for encryption. Takes the plaintext and iv as the input.
    Serial.println("\nCiphertext");
    Serial.println(string_for_data);
    clear_variables();
    return;
  }
}

void decrypt_aes_from_Serial() {
  bool cont_to_next = false;
  while (cont_to_next == false) {
    Serial.println("\nPaste the ciphertext here:");
    while (!Serial.available()) {
    }
    String ct = Serial.readString();
    decrypt_string_with_aes_in_cbc(ct);
    Serial.println("Plaintext:");
    Serial.println(string_for_data);
    clear_variables();
    return;
  }
}

int generate_random_number() {
  bool move_frw = false;
  uint32_t aRandom32bit;
  while (move_frw == false) {
    if (HAL_RNG_GenerateRandomNumber( & hrng, & aRandom32bit) != HAL_OK) {
      /* Random number generation error */
      Error_Handler();
    } else {
      move_frw = true;
    }
  }
  return aRandom32bit % 256;
}

void setup() {
  Serial.begin(115200);
  m = 2;
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK) {
    Error_Handler();
  }
}

void loop() {
  Serial.println("\nEncryption/Decryption Test:");
  String plaintext = "This string is encrypted with AES-256 in Cipher Block Chaining mode!"; // String to be encrypted
  int iv[16]; // Initialization vector
  for (int i = 0; i < 16; i++){
    iv[i] = generate_random_number(); // Fill iv array with random numbers
  }
  encrypt_string_with_aes_in_cbc(plaintext, iv); // Function for encryption. Takes the plaintext and iv as the input.
  Serial.println("\nCiphertext");
  Serial.println(string_for_data);
  String ciphertext = string_for_data; // Back the ciphertext up
  decrypt_string_with_aes_in_cbc(ciphertext); // Decrypt data
  Serial.println("Plaintext:");
  Serial.println(string_for_data);
  delay(5000);
}
