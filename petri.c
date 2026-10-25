/***************************************************************************
 *   petri.c                                  Version 20160806.231815      *
 *                                                                         *
 *   Petri Net Simulator                                                   *
 *   Copyright (C) 2016         by Ruben Carlo Benante                     *
 ***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 *   To contact the author, please write to:                               *
 *   Ruben Carlo Benante                                                   *
 *   Email: rcb@beco.cc                                                    *
 *   Webpage: http://www.beco.cc                                           *
 *   Phone: +55 (81) 3184-7555                                             *
 ***************************************************************************/

/* ---------------------------------------------------------------------- */
/**
 * @file petri.c
 * @ingroup GroupUnique
 * @brief Petri Net Simulator
 * @details This program really do a nice job as a template, and template only!
 * @version 20160806.231815
 * @date 2016-08-06
 * @author Ruben Carlo Benante <<rcb@beco.cc>>
 * @par Webpage
 * <<a href="http://www.beco.cc">www.beco.cc</a>>
 * @copyright (c) 2016 GNU GPL v2
 * @note This program is free software: you can redistribute it
 * and/or modify it under the terms of the
 * GNU General Public License as published by
 * the Free Software Foundation version 2 of the License.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.
 * If not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA. 02111-1307, USA.
 * Or read it online at <<http://www.gnu.org/licenses/>>.
 *
 *
 * @todo Now that you have the template, hands on! Programme!
 * @warning Be carefull not to lose your mind in small things.
 * @bug This file right now does nothing usefull
 *
 */

/*
 * Instrucoes para compilar:
 *   $gcc petri.c -o petri.x -Wall
 *        -Wextra -ansi -pedantic-errors -g -O0 -DDEBUG=1 -DVERSION="0.1.160612.142044"
 *   ou preferencialmente inclua um makefile e use:
 *   $make
 *
 * Modelo de indentacao:
 * Estilo: --style=allman ou -A1
 *
 * Opcoes: -A1 -s4 -k3 -xj -SCNeUpv
 *
 *  * No vi use:
 *      :% !astyle -A1 -s4 -k3 -xj -SCNeUpv
 *  * No linux:
 *      $astlye -A1 -s4 -k3 -xj -SCNeUpv petri.c
 */

/* ---------------------------------------------------------------------- */
/* includes */

#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Miscellaneous functions (rand, malloc, srand)*/
#include <getopt.h> /* get options from system argc/argv */
#include <time.h>   /* clock, time, srand */
#include <unistd.h> /* getpid */
#include <limits.h> /* INT_MAX, LLONG_MAX */
#include <errno.h>  /* errno, strtol */
#include "petri.h"  /* To be created for this template if needed */

/* #include <time.h> */ /* Time and date functions */
/* #include <math.h> */ /* Mathematics functions */
/* #include <string.h> */ /* Strings functions definitions */
/* #include <dlfcn.h> */ /* Dynamic library */
/* #include <malloc.h> */ /* Dynamic memory allocation */
/* #include <unistd.h> */ /* UNIX standard function */
/* #include <limits.h> */ /* Various C limits */
/* #include <ctype.h> */ /* Character functions */
/* #include <errno.h> */ /* Error number codes errno */
/* #include <signal.h> */ /* Signal processing */
/* #include <stdarg.h> */ /* Functions with variable arguments */
/* #include <pthread.h> */ /* Parallel programming with threads */
/* #include <fcntl.h> */ /* File control definitions */
/* #include <termios.h> */ /* Terminal I/O definitions POSIX */
/* #include <sys/stat.h> */ /* File status and information */
/* #include <float.h> */ /* Float constants to help portability */
/* #include <setjmp.h> */ /* Bypass standard function calls and return */
/* #include <stddef.h> */ /* Various types and MACROS */
/* #include <SWI-Prolog.h> */ /* Prolog integration with C */
/* #include <ncurses.h> */ /* Screen handling and optimisation functions */
/* #include <allegro.h> */ /* A game library for graphics, sounds, etc. */
/* #include <libintl.h> */ /* Internationalization / translation */
/* #include <locale.h> */ /* MACROS LC_ for location specific settings */
/* #include "libeco.h" */ /* I/O, Math, Sound, Color, Portable Linux/Windows */

/* ---------------------------------------------------------------------- */
/* definitions */

#ifndef VERSION /* gcc -DVERSION="0.1.160612.142628" */
#define VERSION "20160806.231815" /**< Version Number (string) */
#endif

/* Debug */
#ifndef DEBUG /* gcc -DDEBUG=1 */
#define DEBUG 0 /**< Activate/deactivate debug mode */
#endif

#if DEBUG==0
#define NDEBUG
#endif
/* #include <assert.h> */ /* Verify assumptions with assert. Turn off with #define NDEBUG */ 

/** @brief Debug message if DEBUG on */
#ifndef IFDEBUG
#define IFDEBUG(M) if(DEBUG) fprintf(stderr, "[DEBUG file:%s line:%d]: " M "\n", __FILE__, __LINE__); else {;}
#endif

/* limits */
#define SBUFF 256 /**< String buffer */

#define MAX_TOKENS           100  /**< threshold for reporting big places */
#define DEFAULT_ITERATIONS  1000  /**< default iteration count */
#define PREFERENCE_PERCENT    55  /**< "quer?" probability */

/* ---------------------------------------------------------------------- */
/* types */

typedef struct s_place {
    int id;
    long long tokens, max_seen, min_seen, samples;
    double sum, sum2;
    struct s_place *next;
} t_place;

typedef struct s_arc {
    int place_id;
    long long weight;
    t_place *place;
    struct s_arc *next;
} t_arc;

typedef struct s_transition {
    int id;
    long long fires;
    t_arc *inputs, *outputs;
    struct s_transition *next, *order_next;
} t_transition;

typedef struct s_petri {
    t_place *places;
    t_transition *transitions, *order;
    int nplaces, ntrans, nlt, ntl;
} t_petri;

/* ---------------------------------------------------------------------- */
/* globals */

int verb = 0; /**< verbose level, global within the file */

static const char *g_filename = NULL;
static t_petri    *g_net = NULL;
static int  g_max_iterations = DEFAULT_ITERATIONS;
static int  g_iterations = 0;
static int  g_last_ok = 1;
static long long g_trans_checked = 0;

/* ---------------------------------------------------------------------- */
/* prototypes */

void help(void); /* print some help */
void copyr(void); /* print version and copyright information */
void ex7_init(void); /* global initialization function */

static void *mem(size_t);
static double my_sqrt(double);
static void petri_free(t_petri *);
static void petri_print_stats(t_petri *, int, long long, double);

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief This is the main event of the evening
 * @details Ladies and Gentleman... It's tiiiime!
 * Fightiiiiing at the blue corner,
 * he, who has compiled more C code than any other adversary in the history,
 * he, who has developed UNIX and Linux, and is an inspiration to maaany languages
 * and compilers, the GNU C Compiler, GCC!
 * Fightiiiiing at the red corner, the challenger, in his first fight, lacking of any
 * valid experience but angrily, blindly, and no doubtfully, will try to
 * compile this program without errors. He, the student, the apprentice,
 * the developer, beco!!
 *
 * @param[in] argc Argument counter
 * @param[in] argv Argument strings (argument values)
 *
 * @retval 0 If succeed (EXIT_SUCCESS).
 * @retval 1 Or another error code if failed.
 *
 * @par Example
 * @code
 *    $./petri -h
 * @endcode
 *
 * @warning   Be carefull with...
 * @bug There is a bug with...
 * @todo Need to do...
 * @note You can read more about it at <<a href="http://www.beco.cc">www.beco.cc</a>>
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
int main(int argc, char *argv[])
{
    //Bloco 1 Pessoa 1 - adicionado
    int opt; /* return from getopt() */
    clock_t t0;
    double elapsed;

    IFDEBUG("Starting optarg loop...");

    /* getopt() configured options:
     *        -h  help
     *        -V  version
     *        -v  verbose
     *        -f  arquivo de entrada
     *        -n  numero maximo de iteracoes
     */
    opterr = 0;
    while((opt = getopt(argc, argv, "vhVf:n:")) != EOF)
        switch(opt)
        {
            case 'h': help(); break;
            case 'V': copyr(); break;
            case 'v': verb++; break;
            case 'f': g_filename = optarg; break;
            case 'n':
            {
                char *end;
                long v;
                errno = 0;
                v = strtol(optarg, &end, 10);
                if(errno || end == optarg || *end || v < 0 || v > INT_MAX)
                {
                    fprintf(stderr, "Valor invalido para -n: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                g_max_iterations = (int)v;
                break;
            }
            case '?':
            default:
                printf("Type\n\t$man %s\nor\n\t$%s -h\nfor help.\n\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

    if(verb)
        printf("Verbose level set at: %d\n", verb);

    ex7_init(); /* initialization function */

    t0 = clock();
    while(g_iterations < g_max_iterations && g_last_ok)
        ex7_update();
    elapsed = (double)(clock() - t0) / (double)CLOCKS_PER_SEC;

    petri_print_stats(g_net, g_iterations, g_trans_checked, elapsed);

    petri_free(g_net);
    return EXIT_SUCCESS;
    // Fim do bloco 1
}

/* ---------------------------------------------------------------------- */
static void *mem(size_t n)
{
    //Bloco 2 Pessoa 1 - adicionado
    void *p = calloc(1, n);
    if(!p) { perror("calloc"); exit(EXIT_FAILURE); }
    return p;
    // Fim do bloco 2
}

static double my_sqrt(double x)
{
    //Bloco 3 Pessoa 1 - adicionado
    double lo = 0.0, hi = x < 1.0 ? 1.0 : x, mid;
    int i;
    if(x <= 0.0) return 0.0;
    for(i = 0; i < 80; i++) { mid = (lo + hi) / 2; if(mid * mid < x) lo = mid; else hi = mid; }
    return (lo + hi) / 2;
    // Fim do bloco 3
}

static void petri_free(t_petri *net)
{
    //Bloco 14 Pessoa 5 - adicionado
    t_place *p, *pn;
    t_transition *tr, *trn;
    t_arc *a, *an;

    if(!net) return;
    for(p = net->places; p; p = pn) { pn = p->next; free(p); }
    for(tr = net->transitions; tr; tr = trn)
    {
        trn = tr->next;
        for(a = tr->inputs; a; a = an) { an = a->next; free(a); }
        for(a = tr->outputs; a; a = an) { an = a->next; free(a); }
        free(tr);
    }
    free(net);
    // Fim do bloco 14
}

static int read_arc(FILE *fp, t_petri *net, int is_input, int idx, int interactive)
{
    //Bloco 7 Pessoa 3 - adicionado
    int L, T, R;
    t_place *p;
    t_transition *tr;
    t_arc *a, *b;

    if(interactive)
        printf("Digite a trinca %s%d de arco %s no formato %s:\n",
               is_input ? "ALT" : "ATL", idx,
               is_input ? "consumidor LT" : "produtor TL",
               is_input ? "L T R" : "T L R");

    if(is_input)
    {
        if(fscanf(fp, "%d %d %d", &L, &T, &R) != 3) { fprintf(stderr, "Erro lendo arco LT #%d\n", idx); return 1; }
    }
    else
    {
        if(fscanf(fp, "%d %d %d", &T, &L, &R) != 3) { fprintf(stderr, "Erro lendo arco TL #%d\n", idx); return 1; }
    }

    if(L < 0 || L >= net->nplaces) { fprintf(stderr, "Lugar invalido no arco #%d: %d\n", idx, L); return 1; }
    if(T < 0 || T >= net->ntrans) { fprintf(stderr, "Transicao invalida no arco #%d: %d\n", idx, T); return 1; }
    if(R <= 0) { fprintf(stderr, "Peso invalido no arco #%d: %d\n", idx, R); return 1; }

    for(p = net->places; p && p->id != L; p = p->next) ;
    for(tr = net->transitions; tr && tr->id != T; tr = tr->next) ;
    if(!p || !tr) { fprintf(stderr, "Erro interno no arco #%d\n", idx); return 1; }

    for(b = is_input ? tr->inputs : tr->outputs; b && b->place != p; b = b->next) ;
    if(b) { b->weight += R; return 0; }

    a = mem(sizeof(*a));
    a->place_id = L;
    a->weight = R;
    a->place = p;
    if(is_input) { a->next = tr->inputs;  tr->inputs  = a; }
    else         { a->next = tr->outputs; tr->outputs = a; }
    return 0;
    // Fim do bloco 7
}

static t_petri *petri_read(FILE *fp, int interactive)
{
    //Bloco 5 Pessoa 2 - adicionado
    t_petri *net = mem(sizeof(*net));
    t_place *tailp = NULL, *p;
    t_transition *tailt = NULL, *tr;
    int n, m, l, t, i;

    if(interactive) printf("Quantos lugares? (n>0)\n");
    if(fscanf(fp, "%d", &n) != 1) { fprintf(stderr, "Erro lendo lugares (n)\n"); goto fail; }
    if(interactive) printf("Quantas transicoes? (m)\n");
    if(fscanf(fp, "%d", &m) != 1) { fprintf(stderr, "Erro lendo transicoes (m)\n"); goto fail; }
    if(interactive) printf("Quantos arcos consumidores do tipo LT? (l)\n");
    if(fscanf(fp, "%d", &l) != 1) { fprintf(stderr, "Erro lendo arcos LT (l)\n"); goto fail; }
    if(interactive) printf("Quantos arcos produtores do tipo TL? (t)\n");
    if(fscanf(fp, "%d", &t) != 1) { fprintf(stderr, "Erro lendo arcos TL (t)\n"); goto fail; }

    if(n <= 0 || m < 0 || l < 0 || t < 0)
    {
        fprintf(stderr, "Parametros invalidos: n=%d m=%d l=%d t=%d\n", n, m, l, t);
        goto fail;
    }
    net->nplaces = n; net->ntrans = m; net->nlt = l; net->ntl = t;

    for(i = 0; i < n; i++)
    {
        p = mem(sizeof(*p));
        p->id = i;
        p->min_seen = LLONG_MAX;
        if(tailp) tailp->next = p; else net->places = p;
        tailp = p;
    }

    if(interactive) printf("Digite quantos tokens em cada lugar, separados por espaco:\n");
    for(p = net->places; p; p = p->next)
    {
        if(fscanf(fp, "%lld", &p->tokens) != 1) { fprintf(stderr, "Erro lendo tokens do lugar %d\n", p->id); goto fail; }
        if(p->tokens < 0) { fprintf(stderr, "Tokens negativos no lugar %d\n", p->id); goto fail; }
        p->max_seen = p->min_seen = p->tokens;
    }

    for(i = 0; i < m; i++)
    {
        tr = mem(sizeof(*tr));
        tr->id = i;
        if(tailt) tailt->next = tailt->order_next = tr; else net->transitions = tr;
        tailt = tr;
    }
    net->order = net->transitions;

    for(i = 0; i < l; i++) if(read_arc(fp, net, 1, i, interactive)) goto fail;
    for(i = 0; i < t; i++) if(read_arc(fp, net, 0, i, interactive)) goto fail;

    return net;

fail:
    petri_free(net);
    return NULL;
    // Fim do bloco 5
}

/* ---------------------------------------------------------------------- */
static int petri_enabled(t_transition *tr)
{
    //Bloco 4 Pessoa 1 - adicionado
    t_arc *a;

    for(a = tr->inputs; a; a = a->next)
        if(a->place->tokens < a->weight) return 0;
    return 1;
    // Fim do bloco 4
}

static void petri_fire(t_transition *tr)
{
    //Bloco 6 Pessoa 2 - adicionado
    t_arc *a;
    for(a = tr->inputs; a; a = a->next)  a->place->tokens -= a->weight;
    for(a = tr->outputs; a; a = a->next) a->place->tokens += a->weight;
    tr->fires++;
    // Fim do bloco 6
}

static int petri_rand4(void)
{
    //Bloco 11 Pessoa 4 - adicionado
    static int word = 0, left = 0;
    int r;

    if(left < 2) { word = rand(); left = 15; }
    r = word & 3;
    word >>= 2;
    left -= 2;
    return r;
    // Fim do bloco 11
}

static t_transition **petri_shuffle(t_transition **head)
{
    //Bloco 12 Pessoa 5 - adicionado
    t_transition *part[4] = {NULL, NULL, NULL, NULL}, **end[4], *cur, *nx;
    t_transition **tail;
    int i, c;

    if(!*head) return head;
    if(!(*head)->order_next) return &(*head)->order_next;
    for(i = 0; i < 4; i++) end[i] = &part[i];
    for(cur = *head; cur; cur = nx)
    {
        nx = cur->order_next;
        c = petri_rand4();
        *end[c] = cur;
        end[c] = &cur->order_next;
    }
    for(i = 0; i < 4; i++) *end[i] = NULL;
    tail = head;
    for(i = 0; i < 4; i++)
    {
        end[i] = petri_shuffle(&part[i]);
        if(part[i]) { *tail = part[i]; tail = end[i]; }
    }
    return tail;
    // Fim do bloco 12
}

static int petri_step(t_petri *net)
{
    //Bloco 13 Pessoa 5 - adicionado
    t_transition *tr;
    int enabled = 0;

    petri_shuffle(&net->order);
    for(tr = net->order; tr; tr = tr->order_next)
    {
        g_trans_checked++;
        if(petri_enabled(tr))
        {
            enabled++;
            if((rand() % 100) < PREFERENCE_PERCENT) petri_fire(tr);
        }
    }
    return enabled > 0;
    // Fim do bloco 13
}

static void petri_record_stats(t_petri *net)
{
    //Bloco 10 Pessoa 4 - adicionado
    t_place *p;
    for(p = net->places; p; p = p->next)
    {
        if(p->tokens > p->max_seen) p->max_seen = p->tokens;
        if(p->tokens < p->min_seen) p->min_seen = p->tokens;
        p->sum += (double)p->tokens;
        p->sum2 += (double)p->tokens * (double)p->tokens;
        p->samples++;
    }
    // Fim do bloco 10
}

static void petri_print_stats(t_petri *net, int iters, long long checked, double elapsed)
{
    //Bloco 9 Pessoa 4 - adicionado
    t_place *p;
    t_transition *tr;
    int found;
    double mean, var, sd;

    if(!net) return;

    printf("\n=== Estatisticas da Rede de Petri ===\n");
    printf("Iteracoes realizadas ....: %d\n", iters);
    printf("Transicoes analisadas ...: %lld\n", checked);
    printf("Tempo decorrido .........: %.6f s\n", elapsed);
    if(elapsed > 0.0)
        printf("Iteracoes/segundo ......: %.2f\nTransicoes/segundo .....: %.2f\n",
               iters / elapsed, (double)checked / elapsed);
    else
        printf("Iteracoes/segundo ......: n/a (tempo zero)\nTransicoes/segundo .....: n/a (tempo zero)\n");
    if(!g_last_ok)
        printf("Aviso: deadlock detectado (nenhuma transicao habilitada)\n");

    printf("\n--- Tokens por Lugar ---\n%6s %6s %6s %12s %10s %10s\n",
           "Lugar", "Min", "Max", "Media", "Desvio", "Amostras");
    for(p = net->places; p; p = p->next)
    {
        mean = p->sum / (double)p->samples;
        var = p->sum2 / (double)p->samples - mean * mean;
        sd = my_sqrt(var < 0.0 ? 0.0 : var);
        printf("%6d %6lld %6lld %12.3f %10.3f %10lld\n",
               p->id, (p->min_seen == LLONG_MAX) ? 0 : p->min_seen,
               p->max_seen, mean, sd, p->samples);
    }

    printf("\n--- Disparos por Transicao ---\n%10s %10s\n", "Transicao", "Disparos");
    for(tr = net->transitions; tr; tr = tr->next)
        printf("%10d %10lld\n", tr->id, tr->fires);

    printf("\nLugares inalcancaveis: ");
    for(found = 0, p = net->places; p; p = p->next)
        if(!p->max_seen) { printf("%d ", p->id); found = 1; }
    printf("%s\n", found ? "" : "nenhum");

    printf("Lugares no limiar (%d): ", MAX_TOKENS);
    for(found = 0, p = net->places; p; p = p->next)
        if(p->max_seen >= MAX_TOKENS) { printf("%d ", p->id); found = 1; }
    printf("%s\n", found ? "" : "nenhum");

    printf("Transicoes nunca disparadas: ");
    for(found = 0, tr = net->transitions; tr; tr = tr->next)
        if(!tr->fires) { printf("%d ", tr->id); found = 1; }
    printf("%s\n", found ? "" : "nenhum");
    // Fim do bloco 9
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief Prints help information and exit
 * @details Prints help information (usually called by opt -h)
 * @return Void
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
void help(void)
{
    //Bloco 15 Pessoa 6 - adicionado
    IFDEBUG("help()");
    printf("%s - %s\n", "petri", "Petri Net Simulator");
    printf("\nUsage: %s [-h|-v|-V|-f <arquivo>] [-n <iteracoes>]\n", "petri");
    printf("\nOptions:\n");
    printf("\t-h,  --help\n\t\tShow this help.\n");
    printf("\t-V,  --version\n\t\tShow version and copyright information.\n");
    printf("\t-v,  --verbose\n\t\tSet verbose level (cumulative).\n");
    printf("\t-f,  --file <nome>\n\t\tLe a rede de Petri do arquivo <nome>\n\t\t(sem esta opcao, le da entrada padrao, de forma interativa).\n");
    printf("\t-n,  --iter <N>\n\t\tExecuta no maximo N iteracoes (default: %d).\n", DEFAULT_ITERATIONS);
    /* add more options here */
    printf("\nExit status:\n\t0 if ok.\n\t1 some error occurred.\n");
    printf("\nTodo:\n\tLong options not implemented yet.\n");
    printf("\nAuthor:\n\tWritten by %s <%s>\n\n", "Ruben Carlo Benante", "rcb@beco.cc");
    exit(EXIT_FAILURE);
    // Fim do bloco 15
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief Prints version and copyright information and exit
 * @details Prints version and copyright information (usually called by opt -V)
 * @return Void
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
void copyr(void)
{
    //Bloco 8 Pessoa 3 - adicionado
    IFDEBUG("copyr()");
    printf("%s - Version %s\n", "petri", VERSION);
    printf("\nCopyright (C) %d %s <%s>, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>. This  is  free  software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. USE IT AS IT IS. The author takes no responsability to any damage this software may inflige in your data.\n\n", 2016, "Ruben Carlo Benante", "rcb@beco.cc");
    if(verb > 3) printf("copyr(): Verbose: %d\n", verb); /* -vvvv */
    exit(EXIT_FAILURE);
    // Fim do bloco 8
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief This function initializes some operations before start
 * @details Details to be written in
 * multiple lines
 *
 * @pre You need to call foo() before calling this function
 *
 * @param[in] i Input parameter that does bla
 * @param[out] o Parameter that outputs ble
 * @param[in,out] z The @a z variable is used as input and output
 *
 * @retval 0 Returned when bli
 * @retval 1 Error code: function returned blo
 *
 * @par Example
 * @code
 *    if(x == funcexample(i, o, z))
 *       printf("And that is it\n");
 * @endcode
 *
 * @return Void
 *
 * @warning Be carefull with blu
 * @todo Need to implement it. Its empty now. This doxygen tags are overwhelming.
 * Mandatory tags are: ingroup, brief, details, param, return, author and date.
 * The others are optional.
 *
 * @deprecated This function will be deactivated in version +11
 * @see help()
 * @see copyr()
 * @bug There is a bug with x greater than y
 * @note You can read more about it at <<a href="http://www.beco.cc">www.beco.cc</a>>
 *
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 * @copyright Use this tag only if not the same as the whole file
 *
 */
void ex7_init(void)
{
    //Bloco 16 Pessoa 6 - adicionado
    FILE *fp;
    int interactive = (g_filename == NULL);

    IFDEBUG("ex7_init()");
    srand((unsigned)time(NULL) ^ ((unsigned)getpid() << 16));

    fp = g_filename ? fopen(g_filename, "r") : stdin;
    if(!fp)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", g_filename);
        exit(EXIT_FAILURE);
    }
    g_net = petri_read(fp, interactive);
    if(g_filename) fclose(fp);
    if(!g_net) exit(EXIT_FAILURE);

    petri_record_stats(g_net);
    return;
    // Fim do bloco 16
}

/* ---------------------------------------------------------------------- */
void ex7_update(void)
{
    //Bloco 17 Pessoa 6 - adicionado
    t_place *p;

    g_last_ok = petri_step(g_net);
    if(!g_last_ok) return;

    g_iterations++;
    petri_record_stats(g_net);

    if(verb >= 2)
    {
        printf("[iter %d]", g_iterations);
        for(p = g_net->places; p; p = p->next)
            printf(" P%d=%lld", p->id, p->tokens);
        printf("\n");
    }
    // Fim do bloco 17
}

/* ---------------------------------------------------------------------------- */
/* vi: set ai cin et ts=4 sw=4 tw=0 wm=0 fo=croqltn : C config for Vim modeline */
/* Template by Dr. Beco <rcb at beco dot cc>  Version 20160714.153029           */