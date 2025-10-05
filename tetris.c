/*
  Gerenciador de Peças – Fila Circular + Pilha
  --------------------------------------------
  - Fila circular: capacidade 5, sempre cheia quando possível.
  - Pilha (reserva): capacidade 3.
  - Peça: { nome: 'I'|'O'|'T'|'L', id: sequencial }.
  - Ações:
      1) Jogar peça da frente (dequeue)  -> gera peça nova e enfileira.
      2) Reservar (frente da fila -> topo da pilha) se houver espaço -> repõe na fila.
      3) Usar reservada (pop da pilha)  -> não repõe.
      4) Trocar frente da fila com topo da pilha.
      5) Troca múltipla 3x3 (se fila>=3 e pilha>=3).
      6) Visualizar estado.
      0) Sair.
  - Peças removidas não retornam ao jogo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* -------------------- Definições e estruturas -------------------- */
typedef struct {
    char nome;  /* 'I', 'O', 'T', 'L' */
    int  id;    /* sequencial */
} Peca;

#define Q_CAP 5 /* capacidade da fila */
#define S_CAP 3 /* capacidade da pilha */

typedef struct {
    Peca dados[Q_CAP];
    int ini;   /* índice do primeiro (frente) */
    int tam;   /* quantidade atual */
} Fila;

typedef struct {
    Peca dados[S_CAP];
    int topo;  /* quantidade atual; topo é topo-1 */
} Pilha;

/* -------------------- Utilidades de exibição -------------------- */
static void printPeca(Peca p) {
    printf("[%c %d] ", p.nome, p.id);
}

static void exibirEstado(const Fila *f, const Pilha *s) {
    printf("\nEstado atual:\n\n");
    printf("Fila de peças\t");
    if (f->tam == 0) {
        printf("(vazia)");
    } else {
        for (int i = 0; i < f->tam; i++) {
            int idx = (f->ini + i) % Q_CAP;
            printPeca(f->dados[idx]);
        }
    }
    printf("\n");

    printf("Pilha de reserva\t(Topo -> base): ");
    if (s->topo == 0) {
        printf("(vazia)");
    } else {
        for (int i = s->topo - 1; i >= 0; i--) {
            printPeca(s->dados[i]);
        }
    }
    printf("\n");
}

/* -------------------- Geração de peças -------------------- */
static int proximoId = 0;

static Peca gerarPeca(void) {
    /* Gera nome aleatório entre {I,O,T,L} e id sequencial */
    const char tipos[4] = {'I','O','T','L'};
    Peca p;
    p.nome = tipos[rand() % 4];
    p.id   = proximoId++;
    return p;
}

/* -------------------- Fila circular -------------------- */
static void fila_init(Fila *f) { f->ini = 0; f->tam = 0; }

/* Enfileira no fim (se houver espaço) */
static int fila_enqueue(Fila *f, Peca p) {
    if (f->tam == Q_CAP) return 0; /* cheia */
    int fim = (f->ini + f->tam) % Q_CAP;
    f->dados[fim] = p;
    f->tam++;
    return 1;
}

/* Desenfileira da frente (se houver elemento) */
static int fila_dequeue(Fila *f, Peca *out) {
    if (f->tam == 0) return 0;
    *out = f->dados[f->ini];
    f->ini = (f->ini + 1) % Q_CAP;
    f->tam--;
    return 1;
}

/* Acesso direto à frente (ponteiro) – útil para trocas */
static Peca* fila_frontPtr(Fila *f) {
    if (f->tam == 0) return NULL;
    return &f->dados[f->ini];
}

/* -------------------- Pilha -------------------- */
static void pilha_init(Pilha *s) { s->topo = 0; }

static int pilha_push(Pilha *s, Peca p) {
    if (s->topo == S_CAP) return 0;
    s->dados[s->topo++] = p;
    return 1;
}

static int pilha_pop(Pilha *s, Peca *out) {
    if (s->topo == 0) return 0;
    *out = s->dados[--s->topo];
    return 1;
}

/* Acesso ao topo (ponteiro) para troca */
static Peca* pilha_topPtr(Pilha *s) {
    if (s->topo == 0) return NULL;
    return &s->dados[s->topo - 1];
}

/* -------------------- Ações de jogo -------------------- */

/* 1) Jogar peça: remove da fila e repõe com nova (se couber) */
static void acao_jogar(Fila *f) {
    Peca jogada;
    if (!fila_dequeue(f, &jogada)) {
        printf("Fila vazia. Nao ha peca para jogar.\n");
        return;
    }
    printf("Jogada: "); printPeca(jogada); printf("\n");

    /* repor automatico para manter fila cheia quando possível */
    Peca nova = gerarPeca();
    if (!fila_enqueue(f, nova)) {
        /* pela lógica, sempre deve caber, mas mantemos robusto */
        printf("Atencao: fila cheia, nao foi possivel repor automaticamente.\n");
    } else {
        printf("Reposicao automatica: "); printPeca(nova); printf("\n");
    }
}

/* 2) Reservar: frente da fila -> topo da pilha, se houver espaço; repõe a fila */
static void acao_reservar(Fila *f, Pilha *s) {
    if (s->topo == S_CAP) {
        printf("Pilha cheia. Nao foi possivel reservar.\n");
        return;
    }
    Peca p;
    if (!fila_dequeue(f, &p)) {
        printf("Fila vazia. Nao ha peca para reservar.\n");
        return;
    }
    pilha_push(s, p);
    printf("Reservada no topo: "); printPeca(p); printf("\n");

    /* repor a fila */
    Peca nova = gerarPeca();
    fila_enqueue(f, nova);
    printf("Reposicao automatica: "); printPeca(nova); printf("\n");
}

/* 3) Usar reservada: pop da pilha (nao repoe fila) */
static void acao_usarReservada(Pilha *s) {
    Peca p;
    if (!pilha_pop(s, &p)) {
        printf("Pilha vazia. Nao ha peca reservada para usar.\n");
        return;
    }
    printf("Usada (removida da pilha): "); printPeca(p); printf("\n");
}

/* 4) Trocar frente da fila com topo da pilha */
static void acao_trocarAtual(Fila *f, Pilha *s) {
    Peca *pf = fila_frontPtr(f);
    Peca *ps = pilha_topPtr(s);
    if (!pf) { printf("Fila vazia. Nao ha frente para trocar.\n"); return; }
    if (!ps) { printf("Pilha vazia. Nao ha topo para trocar.\n"); return; }
    Peca tmp = *pf;
    *pf = *ps;
    *ps = tmp;
    printf("Troca realizada entre a frente da fila e o topo da pilha.\n");
}

/* 5) Troca múltipla: 3 primeiros da fila <-> 3 da pilha (se ambas tiverem >=3) */
static void acao_trocaMultipla(Fila *f, Pilha *s) {
    if (f->tam < 3 || s->topo < 3) {
        printf("Operacao invalida: e preciso ter pelo menos 3 na fila e 3 na pilha.\n");
        return;
    }
    for (int i = 0; i < 3; i++) {
        int idxFila = (f->ini + i) % Q_CAP;         /* i-esimo a partir da frente */
        int idxPilha = s->topo - 1 - i;             /* i-esimo a partir do topo */
        Peca tmp = f->dados[idxFila];
        f->dados[idxFila] = s->dados[idxPilha];
        s->dados[idxPilha] = tmp;
    }
    printf("Troca multipla realizada entre os 3 primeiros da fila e os 3 da pilha.\n");
}

/* -------------------- Inicialização do cenário -------------------- */
static void inicializar(Fila *f, Pilha *s) {
    fila_init(f);
    pilha_init(s);
    /* preencher a fila com 5 peças iniciais */
    for (int i = 0; i < Q_CAP; i++) {
        fila_enqueue(f, gerarPeca());
    }
}

/* -------------------- Menu / main -------------------- */
static int lerOpcao(void) {
    int op;
    printf("\nOpcoes disponiveis:\n");
    printf("1) Jogar peca da frente da fila\n");
    printf("2) Enviar peca da fila para a pilha de reserva\n");
    printf("3) Usar peca da pilha de reserva\n");
    printf("4) Trocar a peca da frente da fila com o topo da pilha\n");
    printf("5) Trocar os 3 primeiros da fila com as 3 pecas da pilha\n");
    printf("6) Visualizar estado\n");
    printf("0) Sair\n");
    printf("Escolha: ");
    if (scanf("%d", &op) != 1) {
        /* limpar e retornar opcao invalida */
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return -1;
    }
    int c; while ((c = getchar()) != '\n' && c != EOF) {} /* limpa linha */
    return op;
}

int main(void) {
    srand((unsigned)time(NULL));

    Fila fila;
    Pilha pilha;
    inicializar(&fila, &pilha);

    printf("=== Gerenciador de Pecas (Fila Circular + Pilha) ===\n");
    exibirEstado(&fila, &pilha);

    for (;;) {
        int op = lerOpcao();
        switch (op) {
            case 1:
                acao_jogar(&fila);
                exibirEstado(&fila, &pilha);
                break;
            case 2:
                acao_reservar(&fila, &pilha);
                exibirEstado(&fila, &pilha);
                break;
            case 3:
                acao_usarReservada(&pilha);
                exibirEstado(&fila, &pilha);
                break;
            case 4:
                acao_trocarAtual(&fila, &pilha);
                exibirEstado(&fila, &pilha);
                break;
            case 5:
                acao_trocaMultipla(&fila, &pilha);
                exibirEstado(&fila, &pilha);
                break;
            case 6:
                exibirEstado(&fila, &pilha);
                break;
            case 0:
                printf("Encerrando. Obrigado por jogar!\n");
                return 0;
            default:
                printf("Opcao invalida.\n");
        }
    }
}
