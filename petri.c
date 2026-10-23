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
#include <string.h> /* memset */
#include <time.h>   /* clock, time, srand */
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

struct s_place;
struct s_transition;

typedef struct s_arc
{
    int place_id;
    int weight;
    struct s_place *place;
    struct s_arc *next;
} t_arc;

typedef struct s_transition
{
    int id;
    long long fires;
    t_arc *inputs;
    t_arc *outputs;
    struct s_transition *next;
} t_transition;

typedef struct s_place
{
    int id;
    long long tokens;
    long long max_seen;
    long long min_seen;
    long long sum;
    long long sum2;
    long long samples;
    struct s_place *next;
} t_place;

typedef struct s_petri
{
    t_place *places;
    t_transition *transitions;
    int nplaces;
    int ntrans;
    int nlt;
    int ntl;
} t_petri;

/* ---------------------------------------------------------------------- */
/* globals */

int verb = 0; /**< verbose level, global within the file */

static const char *g_filename        = NULL;
static t_petri    *g_net             = NULL;
static t_transition **g_order        = NULL;
static int         g_max_iterations  = DEFAULT_ITERATIONS;
static int         g_iterations      = 0;
static long        g_trans_checked   = 0;
static int         g_last_had_enabled = 1;

/* ---------------------------------------------------------------------- */
/* prototypes */

void help(void); /* print some help */
void copyr(void); /* print version and copyright information */
void ex7_init(void); /* global initialization function */

static void  *mem(size_t sz);
static double my_sqrt(double x);
static t_place *find_place(t_petri *net, int id);
static t_transition *find_trans(t_petri *net, int id);
static int   read_arc(FILE *fp, t_petri *net, int is_input, int idx, int interactive);
static t_petri *petri_read(FILE *fp, int interactive);
static void  petri_free(t_petri *net);
static int   petri_enabled(t_transition *tr);
static void  petri_fire(t_transition *tr);
static int   petri_step(t_petri *net);
static void  petri_record_stats(t_petri *net);
static void  petri_print_stats(t_petri *net, int iters, long trans_checked, double elapsed);

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
            case 'h':
                help();
                break;
            case 'V':
                copyr();
                break;
            case 'v':
                verb++;
                break;
            case 'f':
                g_filename = optarg;
                break;
            case 'n':
            {
                char *end;
                long v;
                errno = 0;
                v = strtol(optarg, &end, 10);
                if(errno != 0 || end == optarg || *end != '\0' || v < 0 || v > INT_MAX)
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
    while(g_iterations < g_max_iterations && g_last_had_enabled)
    {
        ex7_update();
    }
    elapsed = (double)(clock() - t0) / (double)CLOCKS_PER_SEC;

    petri_print_stats(g_net, g_iterations, g_trans_checked, elapsed);

    petri_free(g_net);
    g_net = NULL;
    free(g_order);
    g_order = NULL;

    return EXIT_SUCCESS;
}

/* ---------------------------------------------------------------------- */
static void *mem(size_t sz)
{
    void *p = malloc(sz);
    if(!p)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return p;
}

static double my_sqrt(double x)
{
    double lo, hi, mid;
    int i;

    if(x <= 0.0) return 0.0;
    lo = 0.0;
    hi = (x < 1.0) ? 1.0 : x;
    for(i = 0; i < 80; i++)
    {
        mid = 0.5 * (lo + hi);
        if(mid * mid < x) lo = mid;
        else              hi = mid;
    }
    return 0.5 * (lo + hi);
}

static t_place *find_place(t_petri *net, int id)
{
    t_place *p;
    if(!net) return NULL;
    for(p = net->places; p; p = p->next)
        if(p->id == id) return p;
    return NULL;
}

static t_transition *find_trans(t_petri *net, int id)
{
    t_transition *tr;
    if(!net) return NULL;
    for(tr = net->transitions; tr; tr = tr->next)
        if(tr->id == id) return tr;
    return NULL;
}

static int read_arc(FILE *fp, t_petri *net, int is_input, int idx, int interactive)
{
    int L, T, R;
    t_place *p;
    t_transition *tr;
    t_arc *a;

    if(interactive)
    {
        if(is_input) printf("Digite a trinca ALT%d de arco consumidor LT no formato L T R:\n", idx);
        else         printf("Digite a trinca ATL%d de arco produtor TL no formato T L R:\n", idx);
    }

    if(is_input)
    {
        if(fscanf(fp, "%d %d %d", &L, &T, &R) != 3)
        {
            fprintf(stderr, "Erro lendo arco LT #%d (esperado: L T R)\n", idx);
            return 1;
        }
    }
    else
    {
        if(fscanf(fp, "%d %d %d", &T, &L, &R) != 3)
        {
            fprintf(stderr, "Erro lendo arco TL #%d (esperado: T L R)\n", idx);
            return 1;
        }
    }

    if(L < 0 || L >= net->nplaces)
    {
        fprintf(stderr, "Lugar invalido no arco #%d: %d (deve estar em [0,%d])\n",
                idx, L, net->nplaces - 1);
        return 1;
    }
    if(T < 0 || T >= net->ntrans)
    {
        fprintf(stderr, "Transicao invalida no arco #%d: %d (deve estar em [0,%d])\n",
                idx, T, net->ntrans - 1);
        return 1;
    }
    if(R <= 0)
    {
        fprintf(stderr, "Peso invalido no arco #%d: %d (deve ser > 0)\n", idx, R);
        return 1;
    }

    p  = find_place(net, L);
    tr = find_trans(net, T);
    if(!p || !tr)
    {
        fprintf(stderr, "Erro interno localizando lugar/transicao no arco #%d\n", idx);
        return 1;
    }

    a = mem(sizeof(*a));
    a->place_id = L;
    a->weight   = R;
    a->place    = p;
    a->next     = NULL;

    if(is_input)
    {
        a->next    = tr->inputs;
        tr->inputs = a;
    }
    else
    {
        a->next     = tr->outputs;
        tr->outputs = a;
    }

    return 0;
}

static t_petri *petri_read(FILE *fp, int interactive)
{
    t_petri *net;
    t_place *lastp, *p;
    t_transition *lastt, *tr;
    int n, m, l, t, i;

    net = mem(sizeof(*net));
    memset(net, 0, sizeof(*net));

    if(interactive) printf("Quantos lugares? (n>0)\n");
    if(fscanf(fp, "%d", &n) != 1)
    {
        fprintf(stderr, "Erro lendo numero de lugares (n)\n");
        goto fail;
    }

    if(interactive) printf("Quantas transicoes? (m)\n");
    if(fscanf(fp, "%d", &m) != 1)
    {
        fprintf(stderr, "Erro lendo numero de transicoes (m)\n");
        goto fail;
    }

    if(interactive) printf("Quantos arcos consumidores do tipo LT? (l)\n");
    if(fscanf(fp, "%d", &l) != 1)
    {
        fprintf(stderr, "Erro lendo numero de arcos LT (l)\n");
        goto fail;
    }

    if(interactive) printf("Quantos arcos produtores do tipo TL? (t)\n");
    if(fscanf(fp, "%d", &t) != 1)
    {
        fprintf(stderr, "Erro lendo numero de arcos TL (t)\n");
        goto fail;
    }

    if(n <= 0 || m < 0 || l < 0 || t < 0)
    {
        fprintf(stderr, "Parametros invalidos da rede: n=%d m=%d l=%d t=%d\n", n, m, l, t);
        goto fail;
    }

    net->nplaces = n;
    net->ntrans  = m;
    net->nlt     = l;
    net->ntl     = t;

    lastp = NULL;
    for(i = 0; i < n; i++)
    {
        p = mem(sizeof(*p));
        memset(p, 0, sizeof(*p));
        p->id       = i;
        p->min_seen = LLONG_MAX;
        p->next     = NULL;
        if(lastp) lastp->next = p;
        else      net->places = p;
        lastp = p;
    }

    if(interactive) printf("Digite quantos tokens em cada lugar, separados por espaco:\n");
    p = net->places;
    for(i = 0; i < n; i++)
    {
        if(fscanf(fp, "%lld", &p->tokens) != 1)
        {
            fprintf(stderr, "Erro lendo tokens do lugar %d\n", i);
            goto fail;
        }
        if(p->tokens < 0)
        {
            fprintf(stderr, "Tokens iniciais negativos no lugar %d\n", i);
            goto fail;
        }
        p->max_seen = p->tokens;
        p->min_seen = p->tokens;
        p = p->next;
    }

    lastt = NULL;
    for(i = 0; i < m; i++)
    {
        tr = mem(sizeof(*tr));
        memset(tr, 0, sizeof(*tr));
        tr->id = i;
        if(lastt) lastt->next = tr;
        else      net->transitions = tr;
        lastt = tr;
    }

    for(i = 0; i < l; i++)
        if(read_arc(fp, net, 1, i, interactive)) goto fail;

    for(i = 0; i < t; i++)
        if(read_arc(fp, net, 0, i, interactive)) goto fail;

    return net;

fail:
    petri_free(net);
    return NULL;
}

/* ---------------------------------------------------------------------- */
static int petri_enabled(t_transition *tr)
{
    t_arc *a, *b;
    long long total;
    int first;

    if(!tr) return 0;
    for(a = tr->inputs; a; a = a->next)
    {
        first = 1;
        for(b = tr->inputs; b != a; b = b->next)
            if(b->place_id == a->place_id) { first = 0; break; }
        if(!first) continue;

        total = 0;
        for(b = a; b; b = b->next)
            if(b->place_id == a->place_id) total += b->weight;

        if(!a->place || a->place->tokens < total) return 0;
    }
    return 1;
}

static void petri_fire(t_transition *tr)
{
    t_arc *a;
    t_place *p;

    if(!tr) return;
    for(a = tr->inputs; a; a = a->next)
    {
        p = a->place;
        if(p) p->tokens -= a->weight;
    }
    for(a = tr->outputs; a; a = a->next)
    {
        p = a->place;
        if(p) p->tokens += a->weight;
    }
    tr->fires++;
}

static int petri_step(t_petri *net)
{
    t_transition *tmp;
    int i, j, enabled_count;

    if(!net || net->ntrans == 0 || !g_order) return 0;

    for(i = net->ntrans - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        tmp = g_order[i]; g_order[i] = g_order[j]; g_order[j] = tmp;
    }

    enabled_count = 0;
    for(i = 0; i < net->ntrans; i++)
    {
        g_trans_checked++;
        if(petri_enabled(g_order[i]))
        {
            enabled_count++;
            if((rand() % 100) < PREFERENCE_PERCENT)
                petri_fire(g_order[i]);
        }
    }

    return enabled_count > 0;
}

/* ---------------------------------------------------------------------- */
static void petri_record_stats(t_petri *net)
{
    t_place *p;
    if(!net) return;
    for(p = net->places; p; p = p->next)
    {
        if(p->tokens > p->max_seen) p->max_seen = p->tokens;
        if(p->tokens < p->min_seen) p->min_seen = p->tokens;
        p->sum   += p->tokens;
        p->sum2  += p->tokens * p->tokens;
        p->samples++;
    }
}

static void petri_print_stats(t_petri *net, int iters, long trans_checked, double elapsed)
{
    t_place *p;
    t_transition *tr;
    int found;
    double mean, var, sd;

    if(!net) return;

    printf("\n=== Estatisticas da Rede de Petri ===\n");
    printf("Iteracoes realizadas ....: %d\n", iters);
    printf("Transicoes analisadas ...: %ld\n", trans_checked);
    printf("Tempo decorrido .........: %.6f s\n", elapsed);
    if(elapsed > 0.0)
    {
        printf("Iteracoes/segundo ......: %.2f\n", (double)iters / elapsed);
        printf("Transicoes/segundo .....: %.2f\n", (double)trans_checked / elapsed);
    }
    else
    {
        printf("Iteracoes/segundo ......: n/a (tempo zero)\n");
        printf("Transicoes/segundo .....: n/a (tempo zero)\n");
    }
    if(!g_last_had_enabled)
        printf("Aviso: deadlock detectado (nenhuma transicao habilitada)\n");

    printf("\n--- Tokens por Lugar ---\n");
    printf("%6s %6s %6s %12s %10s %10s\n",
           "Lugar", "Min", "Max", "Media", "Desvio", "Amostras");
    for(p = net->places; p; p = p->next)
    {
        if(p->samples > 0)
        {
            mean = (double)p->sum / (double)p->samples;
            var  = (double)p->sum2 / (double)p->samples - mean * mean;
            if(var < 0.0) var = 0.0;
            sd   = my_sqrt(var);
        }
        else { mean = 0.0; sd = 0.0; }

        printf("%6d %6lld %6lld %12.3f %10.3f %10lld\n",
               p->id,
               (p->min_seen == LLONG_MAX) ? 0 : p->min_seen,
               p->max_seen, mean, sd, p->samples);
    }

    printf("\n--- Disparos por Transicao ---\n");
    printf("%10s %10s\n", "Transicao", "Disparos");
    for(tr = net->transitions; tr; tr = tr->next)
        printf("%10d %10lld\n", tr->id, tr->fires);

    printf("\nLugares inalcancaveis (nunca tiveram tokens): ");
    found = 0;
    for(p = net->places; p; p = p->next)
        if(p->max_seen == 0) { printf("%d ", p->id); found = 1; }
    if(!found) printf("nenhum");
    printf("\n");

    printf("Lugares que atingiram o limiar (max_seen >= %d): ", MAX_TOKENS);
    found = 0;
    for(p = net->places; p; p = p->next)
        if(p->max_seen >= MAX_TOKENS) { printf("%d ", p->id); found = 1; }
    if(!found) printf("nenhum");
    printf("\n");

    printf("Transicoes que nunca dispararam: ");
    found = 0;
    for(tr = net->transitions; tr; tr = tr->next)
        if(tr->fires == 0) { printf("%d ", tr->id); found = 1; }
    if(!found) printf("nenhum");
    printf("\n");
}

/* ---------------------------------------------------------------------- */
static void petri_free(t_petri *net)
{
    t_place *p, *pn;
    t_transition *tr, *trn;
    t_arc *a, *an;

    if(!net) return;

    for(p = net->places; p; p = pn)
    {
        pn = p->next;
        free(p);
    }
    for(tr = net->transitions; tr; tr = trn)
    {
        trn = tr->next;
        for(a = tr->inputs; a; a = an) { an = a->next; free(a); }
        for(a = tr->outputs; a; a = an){ an = a->next; free(a); }
        free(tr);
    }
    free(net);
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
    IFDEBUG("copyr()");
    printf("%s - Version %s\n", "petri", VERSION);
    printf("\nCopyright (C) %d %s <%s>, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>. This  is  free  software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. USE IT AS IT IS. The author takes no responsability to any damage this software may inflige in your data.\n\n", 2016, "Ruben Carlo Benante", "rcb@beco.cc");
    if(verb > 3) printf("copyr(): Verbose: %d\n", verb); /* -vvvv */
    exit(EXIT_FAILURE);
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
    FILE *fp;
    int interactive;
    t_transition *tr;
    int i;

    IFDEBUG("ex7_init()");

    srand((unsigned)time(NULL));

    interactive = (g_filename == NULL);
    if(g_filename)
    {
        fp = fopen(g_filename, "r");
        if(!fp)
        {
            fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", g_filename);
            exit(EXIT_FAILURE);
        }
        g_net = petri_read(fp, interactive);
        fclose(fp);
    }
    else
    {
        g_net = petri_read(stdin, interactive);
    }

    if(!g_net) exit(EXIT_FAILURE);

    g_order = NULL;
    if(g_net->ntrans > 0)
    {
        g_order = mem((size_t)g_net->ntrans * sizeof(*g_order));
        tr = g_net->transitions;
        for(i = 0; i < g_net->ntrans; i++)
        {
            g_order[i] = tr;
            tr = tr->next;
        }
    }

    g_iterations       = 0;
    g_trans_checked    = 0;
    g_last_had_enabled = 1;

    petri_record_stats(g_net);

    return;
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief Executa uma iteracao da simulacao
 * @details Roda todas as transicoes em ordem aleatoria, aplica "pode?"
 *          e "quer?" (55%). Se nada estava habilitado, a iteracao nao e
 *          contada e o estado nao e amostrado (deadlock).
 *
 * @return Void
 *
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
void ex7_update(void)
{
    t_place *p;

    g_last_had_enabled = petri_step(g_net);
    if(!g_last_had_enabled)
        return;

    g_iterations++;
    petri_record_stats(g_net);

    if(verb >= 2)
    {
        printf("[iter %d]", g_iterations);
        for(p = g_net->places; p; p = p->next)
            printf(" P%d=%lld", p->id, p->tokens);
        printf("\n");
    }

    return;
}

/* ---------------------------------------------------------------------------- */
/* vi: set ai cin et ts=4 sw=4 tw=0 wm=0 fo=croqltn : C config for Vim modeline */
/* Template by Dr. Beco <rcb at beco dot cc>  Version 20160714.153029           */