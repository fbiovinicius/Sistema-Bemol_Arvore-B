#ifndef TERMINAL_H
#define TERMINAL_H

#include "produto.h" 

void mostrar_menu_principal();
void mostrar_cabecalho(const char* titulo);
void mostrar_linha_divisoria();
void limpar_tela();
void aguardar_enter();
void mostrar_mensagem(const char* mensagem, int tipo); 

#endif
