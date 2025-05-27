#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terminal.h"

#ifdef _WIN32
#include <windows.h>
#define LIMPAR_TELA "cls"
#else
#define LIMPAR_TELA "clear"
#endif

void limpar_tela() {
    system(LIMPAR_TELA);
}

void mostrar_cabecalho(const char* titulo) {
    printf("\n=== %s ===\n", titulo);
}

void mostrar_linha_divisoria() {
    printf("------------------------------------------------\n");
}

void mostrar_menu_principal() {
    limpar_tela();
    mostrar_cabecalho("SISTEMA BEMOL");
    
    printf("\n1. Cadastrar novo produto\n");
    printf("2. Buscar produto\n");
    printf("3. Listar todos os produtos\n");
    printf("4. Modificar produto\n");
    printf("5. Remover produto\n");
    printf("6. Produtos por valor maximo\n");
    printf("7. Sair\n");
    
    printf("\nEscolha uma opcao: ");
}

void aguardar_enter() {
    printf("\nPressione Enter para continuar...");
    while (getchar() != '\n');
}

void mostrar_mensagem(const char* mensagem, int tipo) {
    const char* cores[] = {"\033[0m", "\033[31m", "\033[32m"}; 
    printf("%s%s%s\n", cores[tipo], mensagem, cores[0]);
}
