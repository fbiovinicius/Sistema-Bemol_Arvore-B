#ifndef PRODUTO_H
#define PRODUTO_H

#define MAX_CHAVE 10
#define MAX_NOME 100
#define MAX_PRECO 15
#define MAX_ESTOQUE 5
#define MAX_CATEGORIA 30
#define MAX_PRODUTOS 100
#define GRAU 3

typedef struct {
    char codigo[MAX_CHAVE + 1];
    char nome[MAX_NOME + 1];
    char preco[MAX_PRECO + 1];
    char estoque[MAX_ESTOQUE + 1];
    char categoria[MAX_CATEGORIA + 1];
    int ativo;
} Produto;

typedef struct BTreeNode {
    char *chaves[2 * GRAU - 1];
    int indices[2 * GRAU - 1];
    int num_chaves;
    struct BTreeNode *filhos[2 * GRAU];
    int folha;
} BTreeNode;


void inicializar_sistema();
void executar_menu();

#endif
