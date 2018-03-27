/*H**********************************************************************
* FILENAME :        caesar_code.c             DESIGN REF: FMCM00
*
* DESCRIPTION :
*       Encrypt and decrypt text files using the simple Caesar Code.
*
*
* NOTES :
*       the .cdc means "Codice di Cesare" which is the italian translation of
*       "Caesar's Code"
*
* AUTHOR :    Nicholas Sollazzo (@Sal0hc1n)   START DATE : 25 Mar 18
*
* VERSION : 1.0                               REALEASE DATE : 27 Mar 18
*
*
*H*/

/*Libraries*/
#include<stdio.h>
#include<string.h>

/*Prototypes*/
int codec(char * file_name, char * s_key);
char check(char chr, int key, int mode);
int caesar_code(int code, int key, int lower_limit, int upper_limit, int mode);
void strplc(char * str, char * substr, int i);
unsigned long int strtoul(const char * str, char * * endptr, int base);

/**
 * Main
 * ----
 * function with three operations:
 * 1) Coding: if it receive 'file_name.txt key' from the user
 * 2) Decoding: if it receive 'file_name.cdc' from the user
 * 3) Showing the right syntax to the user
 */
int main(int argc, char * argv[]) {
  int err = 0;

  if (argc == 4) {
    if(argv[1][1] == 'c'){
      codec(argv[2], argv[3]);
    } else {
      err = 1;
    }
  } else if (argc == 3){
    if (argv[1][1] == 'd'){
      codec(argv[2], NULL);
    } else {
      err = 1;
    }
  } else {
    err = 1;
  }

  if(err){
      printf("Syntax:\n* caesar -c file_name.txt key\n* caesar -d file_name.cdc\n");
      return 1;
    }
  return 0;
}

/**
 * COder-DECoder
 * -------------
 * Function that code and decode text in a given file.
 * Coder:
 *   - check that the key is between 1 and 26
 *   - open the .txt file in read-only mode
 *   - make a .cdc file and open it in write-only mode
 *   - write the key at the beginning of the file as a letter
 *   - encode the text into the .txt file and put it in the .cdc
 *
 * Decoder:
 *   - open the .cdc file in read-only
 *   - get the key from the first character of the file
 *   - make a .txt file and open it in write-only mode
 *   - decode the text into the .cdc file and put it in the .txt
 */
int codec(char * file_name, char * s_key) {
  FILE * plain_file, * encrypted_file;
  int i, i_key;
  char chr, c_key;
  char * endptr; // next character in str after the numerical value

  if(s_key){

    i_key = strtoul(s_key, & endptr, 10); // cast the string key to an integer
    if (i_key < 1 || i_key > 26) {
      printf("The key must be a number between 1 and 26\n");
      return 1;
    }

    if ((plain_file = fopen(file_name, "r")) == NULL) {
      printf("Cannot read the file %s\n", file_name);
      return 3;
    }

    strplc(file_name, "cdc", strlen(file_name) - 3);
    if ((encrypted_file = fopen(file_name, "w")) == NULL) {
      printf("Cannot create .cdc file\n");
      return 2;
    }

    c_key = i_key + 96; // add 96 to write the integer key as a char
    fprintf(encrypted_file, "%c", c_key);
    while ((chr = fgetc(plain_file)) != EOF) {
      chr = check(chr, i_key, 0);
      fputc(chr, encrypted_file);
    }

  } else {

    if ((encrypted_file = fopen(file_name, "r")) == NULL) {
      printf("Cannot read the file %s\n", file_name);
      return 3;
    }

    c_key = fgetc(encrypted_file);
    i_key = c_key - 96; // remove 96 to get the char key as an integer

    strplc(file_name, "txt", strlen(file_name) - 3);
    if ((plain_file = fopen(file_name, "w")) == NULL) {
      printf("Cannot create the .txt file\n");
      fclose(encrypted_file);
      return 2;
    }

    while ((chr = fgetc(encrypted_file)) != EOF) {
      chr = check(chr, i_key, 1);
      fputc(chr, plain_file);
    }

  }

  fclose(plain_file);
  fclose(encrypted_file);
}


/**
 * Check
 * -----
 * function that check if the character give is an Uppercase, lowercase,
 * symbols, space, new line or numbers.
 * The mode variabile is used to choose what to do:
 *   - mode = 0 -> encrypt
 *   - mode = 1 -> decrypt
 */
char check(char chr, int key, int mode) {
  int code;
  int lower_limit, upper_limit;
  code = chr;
  if (chr != 32 && chr != '\n') {
    if (code > 32 && code < 65) { // numbers and symbols
      lower_limit = 33;
      upper_limit = 64;
      code = caesar_code(code, key, lower_limit, upper_limit, mode);
    } else {
      if (code > 64 && code < 91) { // uppercase letters
        lower_limit = 65;
        upper_limit = 90;
        code = caesar_code(code, key, lower_limit, upper_limit, mode);
      } else {
        if (code > 96 && code < 123) { // lowercase letters
          lower_limit = 97;
          upper_limit = 122;
          code = caesar_code(code, key, lower_limit, upper_limit, mode);
        }
      }
    }
  }
  return code;
}

/**
 * Caesar Code
 * -----------
 * Function that encrypt and decrypt a character.
 * It takes:
 *   - the character to crypt or encrypt
 *   - the key used to encrypt or decrypt
 *   - the ASCII lower limit
 *   - the ASCII upper limit
 *   - the mode:
 *     -- mode = 0 -> encrypt
 *     -- mode = 1 -> decrypt
 */
int caesar_code(int code, int key, int lower_limit, int upper_limit, int mode){
  if(mode){ // encrypt
    code += key;
    if (code > upper_limit) {
      // the minus one is because the lower limit already includes a letter
      code = (code - upper_limit) + lower_limit - 1;
    }
  } else {
    if ((code - key) < lower_limit) {
      key = (code - key) - lower_limit;
      code = upper_limit + key + 1;
      // the plus one is because the upper limit already includes a letter

    } else {
      code -= key;
    }

  }
  return code;
}


/**
 * STRing rePLaCe
 * --------------
 * Function that sobstitute a substring into a string.
 * The function takes:
 * - the main string
 * - the substring
 * - the index from which the function starts to substitute in the main string
 */
void strplc(char * str, char * substr, int i) {
  int j;
  for (j = 0; j < strlen(substr); j++) {
    str[i] = substr[j];
    i++;
  }
}
