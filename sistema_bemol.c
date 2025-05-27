#include "produto.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <conio.h> 

#define MAX_CHAVE 10
#define MAX_NOME 100
#define MAX_PRECO 15
#define MAX_ESTOQUE 5
#define MAX_CATEGORIA 30
#define MAX_PRODUTOS 100
#define GRAU 3


BTreeNode *raiz = NULL;
const char *ARQUIVO_DADOS = "dados_bemol.txt";
const char *ARQUIVO_INDICE = "indice_bemol.dat";

Produto produtos[MAX_PRODUTOS];
int totalProdutos = 0;

const char *categorias[] = {
    "Ar e Ventilacao",
    "Celular e Smartphone",
    "Eletrodomesticos",
    "Eletroportateis", 
    "Informatica",
    "Mercado",
    "Farmacia",
    "Material de Construcao",
    "Moveis",
    "TV e Video"
};
int totalCategorias = 10;


void limpar_buffer();
void mostrar_categorias();
BTreeNode *criar_no(int folha);
void inserir_na_arvore(char *chave, int indice);
int buscar_na_arvore(char *chave);
void remover_da_arvore(BTreeNode *no, char *chave);
void liberar_arvore(BTreeNode *no);
void salvar_arquivo_dados();
void salvar_arquivo_indice();
void carregar_arquivo_dados();
void carregar_arquivo_indice();
void inserir_produto();
void buscar_produto();
void listar_produtos();
void remover_produto();
void modificar_produto();
void produtos_por_valor();
void percorrer_em_ordem(BTreeNode *no, void (*acao)(int));
void imprimir_produto(int indice);

int main() {
    carregar_arquivo_dados();
    carregar_arquivo_indice();

    int opcao;
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Inserir produto\n");
        printf("2. Buscar produto\n");
        printf("3. Listar produtos\n");
        printf("4. Remover produto\n");
        printf("5. Modificar produto\n");
        printf("6. Produtos por valor\n");
        printf("7. Sair\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            printf("Entrada invalida! Por favor, digite um numero.\n");
            continue;
        }
        limpar_buffer();

        switch(opcao) {
            case 1: 
                limpar_tela();
                inserir_produto(); 
                break;
            case 2:
                limpar_tela();
                buscar_produto();
                break;
            case 3:
                limpar_tela();
                listar_produtos();
                break;
            case 4:
                limpar_tela();
                remover_produto();
                break;
            case 5:
                limpar_tela();
                modificar_produto();
                break;
            case 6:
                limpar_tela();
                produtos_por_valor();
                break;
            case 7:
                salvar_arquivo_dados();
                salvar_arquivo_indice();
                liberar_arvore(raiz);
                printf("Sistema encerrado com sucesso!\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
        if (opcao != 7) aguardar_enter();
    } while (opcao != 7);

    return 0;
}



void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void mostrar_categorias() {
    printf("\nCategorias disponiveis:\n");
    for (int i = 0; i < totalCategorias; i++) {
        printf("%d. %s\n", i+1, categorias[i]);
    }
}

BTreeNode *criar_no(int folha) {
    BTreeNode *novo_no = (BTreeNode *)malloc(sizeof(BTreeNode));
    novo_no->folha = folha;
    novo_no->num_chaves = 0;
    for (int i = 0; i < 2 * GRAU; i++) {
        novo_no->filhos[i] = NULL;
    }
    return novo_no;
}

void remover_da_arvore(BTreeNode *no, char *chave) {
    if (no == NULL) return;
    
    int idx = 0;
    while (idx < no->num_chaves && strcmp(chave, no->chaves[idx]) > 0) {
        idx++;
    }
    
    if (idx < no->num_chaves && strcmp(chave, no->chaves[idx]) == 0) {
        free(no->chaves[idx]);
        
        for (int i = idx; i < no->num_chaves - 1; i++) {
            no->chaves[i] = no->chaves[i + 1];
            no->indices[i] = no->indices[i + 1];
        }
        no->num_chaves--;
        
        if (no->folha) return;
    } else {
        if (no->folha) return;
        remover_da_arvore(no->filhos[idx], chave);
    }
}

void liberar_arvore(BTreeNode *no) {
    if (no != NULL) {
        if (!no->folha) {
            for (int i = 0; i <= no->num_chaves; i++) {
                liberar_arvore(no->filhos[i]);
            }
        }
        for (int i = 0; i < no->num_chaves; i++) {
            free(no->chaves[i]);
        }
        free(no);
    }
}

void salvar_arquivo_dados() {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de dados!\n");
        return;
    }

    for (int i = 0; i < totalProdutos; i++) {
        if (produtos[i].ativo) {
            fprintf(arquivo, "%-10s|%s|%-15s|%-5s|%s\n", 
                   produtos[i].codigo, produtos[i].nome, 
                   produtos[i].preco, produtos[i].estoque,
                   produtos[i].categoria);
        }
    }
    fclose(arquivo);
}

void salvar_arquivo_indice() {
    FILE *arquivo = fopen(ARQUIVO_INDICE, "wb");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de indice!\n");
        return;
    }

    if (raiz != NULL) {
        BTreeNode *pilha[100];
        int topo = 0;
        pilha[topo++] = raiz;
        
        while (topo > 0) {
            BTreeNode *atual = pilha[--topo];
            
            fwrite(&atual->num_chaves, sizeof(int), 1, arquivo);
            fwrite(&atual->folha, sizeof(int), 1, arquivo);
            
            for (int i = 0; i < atual->num_chaves; i++) {
                int len = strlen(atual->chaves[i]) + 1;
                fwrite(&len, sizeof(int), 1, arquivo);
                fwrite(atual->chaves[i], sizeof(char), len, arquivo);
                fwrite(&atual->indices[i], sizeof(int), 1, arquivo);
            }
            
            if (!atual->folha) {
                for (int i = atual->num_chaves; i >= 0; i--) {
                    pilha[topo++] = atual->filhos[i];
                }
            }
        }
    }
    
    fclose(arquivo);
}

void carregar_arquivo_dados() {
    FILE *arquivo = fopen(ARQUIVO_DADOS, "r");
    if (arquivo == NULL) return;

    char linha[400];
    totalProdutos = 0;
    
    while (fgets(linha, sizeof(linha), arquivo) != NULL && totalProdutos < MAX_PRODUTOS) {
        char *token = strtok(linha, "|");
        if (!token) continue;

        Produto p = {0};
        p.ativo = 1;

        strncpy(p.codigo, token, MAX_CHAVE);
        p.codigo[MAX_CHAVE] = '\0';
        token = strtok(NULL, "|");

        if (token) {
            strncpy(p.nome, token, MAX_NOME);
            p.nome[MAX_NOME] = '\0';
            token = strtok(NULL, "|");
        }

        if (token) {
            strncpy(p.preco, token, MAX_PRECO);
            p.preco[MAX_PRECO] = '\0';
            token = strtok(NULL, "|");
        }

        if (token) {
            strncpy(p.estoque, token, MAX_ESTOQUE);
            p.estoque[MAX_ESTOQUE] = '\0';
            token = strtok(NULL, "|\n");
        }

        if (token) {
            strncpy(p.categoria, token, MAX_CATEGORIA);
            p.categoria[MAX_CATEGORIA] = '\0';
        } else {
            strcpy(p.categoria, "OUTROS");
        }

        if (strlen(p.codigo) > 0) {
            produtos[totalProdutos] = p;
            totalProdutos++;
        }
    }
    fclose(arquivo);
}

void carregar_arquivo_indice() {
    FILE *arquivo = fopen(ARQUIVO_INDICE, "rb");
    if (arquivo == NULL) {
        for (int i = 0; i < totalProdutos; i++) {
            if (produtos[i].ativo) {
                inserir_na_arvore(produtos[i].codigo, i);
            }
        }
        return;
    }

    if (raiz != NULL) {
        liberar_arvore(raiz);
        raiz = NULL;
    }
    
    BTreeNode *pilha[100];
    int topo = 0;
    
    while (!feof(arquivo)) {
        int num_chaves, folha;
        if (fread(&num_chaves, sizeof(int), 1, arquivo) != 1) break;
        if (fread(&folha, sizeof(int), 1, arquivo) != 1) break;
        
        BTreeNode *novo_no = criar_no(folha);
        novo_no->num_chaves = num_chaves;
        
        for (int i = 0; i < num_chaves; i++) {
            int len;
            if (fread(&len, sizeof(int), 1, arquivo) != 1) break;
            
            novo_no->chaves[i] = (char *)malloc(len);
            if (fread(novo_no->chaves[i], sizeof(char), len, arquivo) != len) break;
            
            if (fread(&novo_no->indices[i], sizeof(int), 1, arquivo) != 1) break;
        }
        
        if (!folha) {
            for (int i = 0; i <= num_chaves; i++) {
                pilha[topo++] = novo_no;
            }
        }
        
        if (raiz == NULL) {
            raiz = novo_no;
        } else {
            BTreeNode *pai = pilha[--topo];
            for (int i = 0; i <= pai->num_chaves; i++) {
                if (pai->filhos[i] == NULL) {
                    pai->filhos[i] = novo_no;
                    break;
                }
            }
        }
    }
    
    fclose(arquivo);
}

void inserir_na_arvore(char *chave, int indice) {
    if (raiz == NULL) {
        raiz = criar_no(1);
        raiz->chaves[0] = strdup(chave);
        raiz->indices[0] = indice;
        raiz->num_chaves = 1;
    } else {
        if (raiz->num_chaves == 2 * GRAU - 1) {
            BTreeNode *nova_raiz = criar_no(0);
            nova_raiz->filhos[0] = raiz;
            
            int i = 0;
            nova_raiz->chaves[0] = raiz->chaves[GRAU - 1];
            nova_raiz->indices[0] = raiz->indices[GRAU - 1];
            nova_raiz->num_chaves = 1;
            
            BTreeNode *novo_no = criar_no(raiz->folha);
            for (i = GRAU; i < 2 * GRAU - 1; i++) {
                novo_no->chaves[i - GRAU] = raiz->chaves[i];
                novo_no->indices[i - GRAU] = raiz->indices[i];
            }
            novo_no->num_chaves = GRAU - 1;
            
            if (!raiz->folha) {
                for (i = GRAU; i < 2 * GRAU; i++) {
                    novo_no->filhos[i - GRAU] = raiz->filhos[i];
                }
            }
            
            raiz->num_chaves = GRAU - 1;
            nova_raiz->filhos[1] = novo_no;
            raiz = nova_raiz;
        }
        
        BTreeNode *no_atual = raiz;
        while (!no_atual->folha) {
            int i = 0;
            while (i < no_atual->num_chaves && strcmp(chave, no_atual->chaves[i]) > 0) {
                i++;
            }
            
            if (no_atual->filhos[i]->num_chaves == 2 * GRAU - 1) {
                BTreeNode *filho = no_atual->filhos[i];
                BTreeNode *novo_filho = criar_no(filho->folha);
                
                for (int j = no_atual->num_chaves; j > i; j--) {
                    no_atual->chaves[j] = no_atual->chaves[j - 1];
                    no_atual->indices[j] = no_atual->indices[j - 1];
                }
                no_atual->chaves[i] = filho->chaves[GRAU - 1];
                no_atual->indices[i] = filho->indices[GRAU - 1];
                no_atual->num_chaves++;
                
                for (int j = GRAU; j < 2 * GRAU - 1; j++) {
                    novo_filho->chaves[j - GRAU] = filho->chaves[j];
                    novo_filho->indices[j - GRAU] = filho->indices[j];
                }
                novo_filho->num_chaves = GRAU - 1;
                
                if (!filho->folha) {
                    for (int j = GRAU; j < 2 * GRAU; j++) {
                        novo_filho->filhos[j - GRAU] = filho->filhos[j];
                    }
                }
                
                filho->num_chaves = GRAU - 1;
                
                for (int j = no_atual->num_chaves; j > i + 1; j--) {
                    no_atual->filhos[j] = no_atual->filhos[j - 1];
                }
                no_atual->filhos[i + 1] = novo_filho;
                
                if (strcmp(chave, no_atual->chaves[i]) < 0) {
                    no_atual = no_atual->filhos[i];
                } else {
                    no_atual = no_atual->filhos[i + 1];
                }
            } else {
                no_atual = no_atual->filhos[i];
            }
        }
        
        int i = no_atual->num_chaves - 1;
        while (i >= 0 && strcmp(chave, no_atual->chaves[i]) < 0) {
            no_atual->chaves[i + 1] = no_atual->chaves[i];
            no_atual->indices[i + 1] = no_atual->indices[i];
            i--;
        }
        no_atual->chaves[i + 1] = strdup(chave);
        no_atual->indices[i + 1] = indice;
        no_atual->num_chaves++;
    }
}

void percorrer_em_ordem(BTreeNode *no, void (*acao)(int)) {
    if (no != NULL) {
        int i;
        for (i = 0; i < no->num_chaves; i++) {
            if (!no->folha) {
                percorrer_em_ordem(no->filhos[i], acao);
            }
            acao(no->indices[i]);
        }
        if (!no->folha) {
            percorrer_em_ordem(no->filhos[i], acao);
        }
    }
}

void imprimir_produto(int indice) {
    if (produtos[indice].ativo) {
        printf("%-10s %-30s %-15s %-5s %-15s\n", 
               produtos[indice].codigo, 
               produtos[indice].nome,
               produtos[indice].preco,
               produtos[indice].estoque,
               produtos[indice].categoria);
    }
}

void listar_produtos() {
    if (totalProdutos == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    printf("\n=== LISTA DE PRODUTOS (Ordenados por Codigo) ===\n");
    printf("%-10s %-30s %-15s %-5s %-15s\n", 
           "CODIGO", "NOME", "PRECO", "ESTQ", "CATEGORIA");
    printf("------------------------------------------------------------\n");
    
    percorrer_em_ordem(raiz, imprimir_produto);  
}

void inserir_produto() {
    if (totalProdutos >= MAX_PRODUTOS) {
        printf("Limite maximo de produtos atingido!\n");
        return;
    }

    Produto novo = {0};
    novo.ativo = 1;

    printf("Codigo do produto (max %d caracteres): ", MAX_CHAVE);
    scanf("%10s", novo.codigo);
    limpar_buffer();

    if (buscar_na_arvore(novo.codigo) != -1) {
        printf("Erro: Produto com este codigo ja existe!\n");
        return;
    }

    printf("Nome do produto: ");
    fgets(novo.nome, MAX_NOME, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("Preco: ");
    scanf("%15s", novo.preco);
    limpar_buffer();

    printf("Quantidade em estoque: ");
    scanf("%5s", novo.estoque);
    limpar_buffer();

    mostrar_categorias();
    printf("Escolha a categoria (1-%d): ", totalCategorias);
    int opcaoCategoria;
    scanf("%d", &opcaoCategoria);
    limpar_buffer();
    
    if (opcaoCategoria < 1 || opcaoCategoria > totalCategorias) {
        strcpy(novo.categoria, "OUTROS");
    } else {
        strcpy(novo.categoria, categorias[opcaoCategoria-1]);
    }

    produtos[totalProdutos] = novo;
    inserir_na_arvore(novo.codigo, totalProdutos);
    totalProdutos++;
    
    salvar_arquivo_dados();
    salvar_arquivo_indice();
    printf("Produto cadastrado com sucesso!\n");
}

int buscar_na_arvore(char *chave) {
    BTreeNode *no_atual = raiz;
    
    while (no_atual != NULL) {
        int i = 0;
        while (i < no_atual->num_chaves && strcmp(chave, no_atual->chaves[i]) > 0) {
            i++;
        }
        
        if (i < no_atual->num_chaves && strcmp(chave, no_atual->chaves[i]) == 0) {
            return no_atual->indices[i];
        }
        
        if (no_atual->folha) {
            return -1;
        }
        
        no_atual = no_atual->filhos[i];
    }
    
    return -1;
}

void buscar_produto() {
    char codigo[MAX_CHAVE + 1];
    
    mostrar_cabecalho("BUSCA DE PRODUTO");
    printf("Digite o codigo do produto: ");
    scanf("%10s", codigo);
    limpar_buffer();

    int indice = buscar_na_arvore(codigo);
    
    if (indice == -1) {
        mostrar_mensagem("Produto nao encontrado!", 1);
        return;
    }

    mostrar_cabecalho("PRODUTO ENCONTRADO");
    mostrar_linha_divisoria();
    printf("Código: %s\n", produtos[indice].codigo);
    printf("Nome: %s\n", produtos[indice].nome);;
    printf("Preco: %s\n", produtos[indice].preco);
    printf("Estoque: %s\n", produtos[indice].estoque);
    printf("Categoria: %s\n", produtos[indice].categoria);
    
    mostrar_linha_divisoria();
}

void modificar_produto() {
    char codigo[MAX_CHAVE + 1];
    printf("Digite o codigo do produto a ser modificado: ");
    scanf("%10s", codigo);
    limpar_buffer();

    int indice = buscar_na_arvore(codigo);
    if (indice == -1 || !produtos[indice].ativo) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("\nModificando produto %s - %s\n", produtos[indice].codigo, produtos[indice].nome);
    printf("Deixe em branco para manter o valor atual.\n");

    char novoTitulo[MAX_NOME + 1];
    printf("Novo titulo [%s]: ", produtos[indice].nome);
    fgets(novoTitulo, MAX_NOME, stdin);
    novoTitulo[strcspn(novoTitulo, "\n")] = '\0';
    if (strlen(novoTitulo) > 0) {
        strcpy(produtos[indice].nome, novoTitulo);
    }

    char novoPreco[MAX_PRECO + 1];
    printf("Novo preco [%s]: ", produtos[indice].preco);
    fgets(novoPreco, MAX_PRECO, stdin);
    novoPreco[strcspn(novoPreco, "\n")] = '\0';
    if (strlen(novoPreco) > 0) {
        strcpy(produtos[indice].preco, novoPreco);
    }

    char novoEstoque[MAX_ESTOQUE + 1];
    printf("Novo estoque [%s]: ", produtos[indice].estoque);
    fgets(novoEstoque, MAX_ESTOQUE, stdin);
    novoEstoque[strcspn(novoEstoque, "\n")] = '\0';
    if (strlen(novoEstoque) > 0) {
        strcpy(produtos[indice].estoque, novoEstoque);
    }

    mostrar_categorias();
    printf("Nova categoria [%s] (0 para manter): ", produtos[indice].categoria);
    int opcaoCategoria;
    char entrada[10];
    fgets(entrada, sizeof(entrada), stdin);
    if (sscanf(entrada, "%d", &opcaoCategoria) == 1 && opcaoCategoria >= 1 && opcaoCategoria <= totalCategorias) {
        strcpy(produtos[indice].categoria, categorias[opcaoCategoria-1]);
    }

    salvar_arquivo_dados();
    printf("Produto modificado com sucesso!\n");
}

void remover_produto() {
    char codigo[MAX_CHAVE + 1];
    printf("Digite o codigo do produto a ser removido: ");
    scanf("%10s", codigo);
    limpar_buffer();

    int indice = buscar_na_arvore(codigo);
    if (indice == -1 || !produtos[indice].ativo) {
        printf("Produto nao encontrado.\n");
        return;
    }

    produtos[indice].ativo = 0;
    remover_da_arvore(raiz, codigo);
    
    salvar_arquivo_dados();
    salvar_arquivo_indice();
    printf("Produto removido com sucesso!\n");
}

void produtos_por_valor() {
    float valor;
    printf("Digite o valor maximo: ");
    if (scanf("%f", &valor) != 1) {
        printf("Valor invalido!\n");
        limpar_buffer();
        return;
    }
    limpar_buffer();
    
    printf("\n=== PRODUTOS COM PRECO ATE %.2f ===\n", valor);
    printf("%-10s %-30s %-15s %-5s %-15s\n", 
           "CODIGO", "NOME", "PRECO", "ESTQ", "CATEGORIA");
    
    int encontrados = 0;
    
    for (int i = 0; i < totalProdutos; i++) {
        if (produtos[i].ativo) {
            float preco = atof(produtos[i].preco);
            if (preco <= valor) {
                printf("%-10s %-30s %-15s %-5s %-15s\n", 
                       produtos[i].codigo, produtos[i].nome, 
                       produtos[i].preco, produtos[i].estoque,
                       produtos[i].categoria);
                encontrados++;
            }
        }
    }
    
    if (encontrados == 0) {
        printf("Nenhum produto encontrado dentro deste valor.\n");
    }
}
