#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROSU "\x1b[31m"
#define VERDE "\x1b[32m"
#define GALBEN "\x1b[33m"
#define RESET "\x1b[0m"

#define EOL "\n"
#define NR_CUV_DICT 11454;
#define LUNGIME_CUVANT 5
#define INCERCARI 6

#define err(x)                          \
  printf(EOL "Fatal error: %s" EOL, x); \
  abort();

int toUpper(char *str) {
  int i = 0;
  while (str[i]) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      str[i] -= 32;
    }
    ++i;
  }
  return i;
}

int pickWord(char *word, int index, FILE *dict) {
  int i = 0;
  fseek(dict, 0, SEEK_SET);
  while (i <= index && fgets(word, LUNGIME_CUVANT + 1, dict) != NULL) {
    if (strlen(word) == LUNGIME_CUVANT) {
      ++i;
    }
  }
  return index;
}

void help() {
  printf("Wordle este un joc simplu:" EOL "Trebuie sa ghicesti un cuvant de  %i litere in %i incercari." EOL, LUNGIME_CUVANT, INCERCARI);
  puts(
      "Dupa fiecare incercare, fiecarui caracter ghicit i se atribuie o culoare." EOL
      "Semnificatia fiecarei culori:" EOL
          ROSU " ROSU" RESET " = Caracterul nu se afla in cuvant." EOL
              GALBEN " GALBEN" RESET " = Caracterul se afla in cuvant, dar pe alta pozitie." EOL
                  VERDE " VERDE" RESET " = Caracterul se afla in cuvant si este pe pozitia corecta." EOL
      "Restul caracterelor nefolosite au culoarea ALB" EOL);
}

int isWord(const char *word) {
  int i = -1;

  while (word[++i]) {
    if (word[i] < 'A' || word[i] > 'Z') {
      return 0;
    }
  }
  return 1;
}

int menu() {
  char buffer[21];
  int gameId = 0;
  int scan = 0;
  srand((int)time(0));
  puts(
      "Main Menu" EOL
      "=========" EOL
      "NEW: Incepe un joc nou." EOL
      "HELP: Cum se joaca." EOL
      "EXIT: Inchide executabilul.");
  while (1) {
    printf("Input: ");
    while ((scan = scanf("%20s", buffer)) != 1) {
      if (scan == EOF) {
        return -1;
      }
    }
    toUpper(buffer);
    if (strcmp(buffer, "EXIT") == 0) {
      return -1;
    } else if (strcmp(buffer, "HELP") == 0) {
      help();
    } else if (strcmp(buffer, "NEW") == 0) {
      return rand() % NR_CUV_DICT;
    } else {
      puts("Invalid input");
    }
  }
}

int strpos(const char *str, char search) {
  int i = 0;
  if (str != NULL) {
    while (str[i]) {
      if (str[i] == search) {
        return i;
      }
      ++i;
    }
  }
  return -1;
}

void printCaracCuloare(char c, int cod) {
  if (cod == 0) {
    printf("%c", c);

  } else if (cod == 1) {
    printf(VERDE "%c" RESET, c);

  } else if (cod == 2) {
    printf(GALBEN "%c" RESET, c);

  } else if (cod == 3) {
    printf(ROSU "%c" RESET, c);
  }
}

void afisAlfabet(char *alf, int *cul) {
  printf("\t");
  for (int i = 0; i < strlen(alf); i++)
    printCaracCuloare(alf[i], cul[i]);
  printf(EOL);
}

int checkWord(const char *guess, char *cuv, int *culAlfabet) {
  if (strlen(guess) == strlen(cuv)) {
    int i = 0;
    int pos = -1;
    char copy[LUNGIME_CUVANT + 1];
    char culoare[LUNGIME_CUVANT + 1];

    strcpy(copy, cuv);
    while (copy[i]) {
      if (copy[i] == guess[i]) {
        // Caracterul gasit e pe poz corecta
        copy[i] = '_';
        culoare[i] = 1;
        culAlfabet[(int)guess[i] - 65] = 1;
      } else {
        // Marcheaza restul caracterelor gresit
        culoare[i] = 3;
        culAlfabet[(int)guess[i] - 65] = 3;
      }
      ++i;
    }
    i = 0;
    while (copy[i]) {
      pos = strpos(copy, guess[i]);
      if (pos >= 0 && culoare[i] != 1) {
        // Caracter gasit dar in pozitia gresita
        copy[pos] = '_';
        culoare[i] = 2;
        culAlfabet[(int)guess[i] - 65] = 2;
      }
      ++i;
    }
    for (int i = 0; i < 6; ++i)
      printCaracCuloare(guess[i], culoare[i]);
    return 1;
  }
  return 0;
}

void gameLoop(char *cuv, char *alfabet, int *color) {
  char guess[LUNGIME_CUVANT + 1] = {0};
  int guesses = 0;
  int scan = 0;
  puts(
      "cuvant\t  alfabet " EOL
      "======\t==========");
  while (guesses < INCERCARI && strcmp(guess, cuv)) {
    printf("Incercarea %i: ", guesses + 1);
    if ((scan = scanf("%5s", guess)) == 1 && strlen(guess) == LUNGIME_CUVANT) {
      toUpper(guess);

      if (strcmp(guess, cuv)) {
        if (isWord(guess)) {
          ++guesses;
          if (checkWord(guess, cuv, color)) {
            afisAlfabet(alfabet, color);
          }
        }
      }
    } else {
      if (scan == EOF) {
        exit(-1);
      }
      printf("Cuvant invalid. Trebuie sa aiba %i caractere\n", LUNGIME_CUVANT);
    }
  }
  if (strcmp(guess, cuv)) {
    printf("Ai pierdut. Cuvantul era %s\n", cuv);
  } else {
    puts("Ai castigat!!");
  }
}

int main() {
  FILE *dictionar;
  char *cuv,*input,*word,*alfabet;
  int *culori;

  dictionar = fopen("./dictionar.txt", "r");
  cuv = calloc(6,sizeof(char));
  word = calloc(6,sizeof(char));
  alfabet = calloc(28,sizeof(char));
  strcpy(alfabet,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  input= malloc(sizeof(char)*101);
  culori  = calloc(27,sizeof(int));

  if (dictionar == NULL) {
    err("eroare la deschiderea dictionarului");
  }

  int gameId = menu();
  if (gameId >= 0) {
    pickWord(word, gameId, dictionar);
  } else {
    return 0;
  }
  printf("Jocul  #%i" EOL, gameId + 1);
  gameLoop(word, alfabet, culori);
  fclose(dictionar);
  return 0;
}