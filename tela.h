#ifndef __WINDOWS_H
#include <windows.h>
#endif

// Cores disponíveis para as fuções textcolor e textbackground

#define PRETO              0
#define AZUL_ESCURO        1
#define VERDE_ESCURO       2
#define VERMELHO_ESCURO    4
#define CLARO       8

#define AZUL        AZUL_ESCURO | CLARO
#define VERDE       VERDE_ESCURO | CLARO
#define VERMELHO    VERMELHO_ESCURO | CLARO

#define CIANO       AZUL_ESCURO | VERDE_ESCURO | CLARO
#define ROSA        AZUL_ESCURO | VERMELHO_ESCURO | CLARO
#define AMARELO     VERDE_ESCURO | VERMELHO_ESCURO | CLARO
#define BRANCO      AZUL_ESCURO | VERDE_ESCURO | VERMELHO_ESCURO | CLARO

#define CINZA            AZUL_ESCURO | VERDE_ESCURO | VERMELHO_ESCURO
#define CIANO_ESCURO     AZUL_ESCURO | VERDE_ESCURO
#define ROSA_ESCURO      AZUL_ESCURO | VERMELHO_ESCURO
#define AMARELO_ESCURO   VERDE_ESCURO | VERMELHO_ESCURO

#define FUNDO 4


// Variáveis Globais

int cor_frente = BRANCO;
int cor_fundo = PRETO;


////////////////////////////////////////////////////////////////////////////////
/*  gotoxy funciona somente no ambiente Windows. */
   
//   #include <windows.h>

// Move o cursor para uma coordenada (linha, coluna)
// Ex.: gotoxy(5,10) => move o cursor para linha 5, coluna 10.


void gotoxy(int lin, int col)
{
    HANDLE hOut;
    COORD Position;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    Position.X = col; 
    Position.Y = lin; 
    SetConsoleCursorPosition(hOut,Position);
}

// Cria um tempo de espera em milisegundos
// Ex.: delay(1000) => espera 1s (1000s).

void delay(DWORD miliseconds)
{
     Sleep(miliseconds);
}

// Muda a cor do texto 
// Ex.: textcolor(AZUL); printf("Alo"); => Imprime a mensagem Alo em azul
// Veja as cores disponíveis no início do arquivo

void textcolor(int cor)
{    
    cor_frente = cor; 
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, cor | (cor_fundo << FUNDO) );
}

// Muda a cor de fundo do texto
// Ex.: textbackground(VERDE); printf("Alo"); => Imprime a mensagem Alo com fundo azul
// Veja as cores disponíveis no início do arquivo

void textbackground(int cor)
{
    cor_fundo = cor;   
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut, cor_frente | (cor << FUNDO) );
}

// Limpa a tela, caso a cor de fundo for diferente de PRETO, então a tela é colorida pela
// cor de fundo especificada em textbackground
// Ex.: clrscr();

void clrscr()
{
  COORD coordScreen = { 0, 0 };
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD dwConSize;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  GetConsoleScreenBufferInfo(hConsole, &csbi);
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter(hConsole, TEXT(' '),
                             dwConSize,
                             coordScreen,
                             &cCharsWritten);
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  FillConsoleOutputAttribute(hConsole,
                             csbi.wAttributes,
                             dwConSize,
                             coordScreen,
                             &cCharsWritten);
  SetConsoleCursorPosition(hConsole, coordScreen);
}
