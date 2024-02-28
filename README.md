# Serpent_in_CBC_mode_for_microcontrollers

This repository contains code that would allow you to easily integrate the AES encryption algorithm in CBC mode to your project.

You can also get this code from SourceForge: https://sourceforge.net/projects/aes-in-cbc-mode-for-mcus/


## Compatibility

The code was successfully tested on the following boards:
- STM32F407VET6
- Teensy 4.1
- ESP32
- ESP8266
- RTL8720DN

## Usage


```
  Serial.println("\nEncryption/Decryption Test:");
  String plaintext = "This string is encrypted with AES-256 in Cipher Block Chaining mode!"; // String to be encrypted
  int iv[16]; // Initialization vector
  for (int i = 0; i < 16; i++){
    iv[i] = random(256); // Fill iv array with random numbers. I suggest you use a more secure method of random number generation!!!
  }
  encrypt_string_with_aes_in_cbc(plaintext, iv); // Function for encryption. Takes the plaintext and iv as the input.
  Serial.println("\nCiphertext");
  Serial.println(string_for_data);
  String ciphertext = string_for_data; // Back the ciphertext up
  decrypt_string_with_aes_in_cbc(ciphertext); // Decrypt data
  Serial.println("Plaintext:");
  Serial.println(string_for_data);
```

![image text](https://github.com/Northstrix/AES_in_CBC_mode_for_microcontrollers/blob/master/Pictures/Encryption%20and%20Decryption%20test.png)

When you call either "encrypt_string_with_aes_in_cbc(plaintext, iv)" or "decrypt_string_with_aes_in_cbc(ciphertext)" the string "string_for_data" is cleared.

## Visual representation of the encryption process
![image text](https://github.com/Northstrix/AES_in_CBC_mode_for_microcontrollers/blob/master/Pictures/AES%20in%20CBC.png)

## Credit

The library with the implementation of [aes](https://github.com/zhouyangchao/AES) by [zhouyangchao](https://github.com/zhouyangchao) is the property of its rightful owner.
