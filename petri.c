/***************************************************************************
 *   ex7.c                                    Version 20160806.231815      *
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
 * @file ex7.c
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
 *   $gcc ex7.c -o ex7.x -Wall
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
 *      $astlye -A1 -s4 -k3 -xj -SCNeUpv ex7.c
 */

/* ---------------------------------------------------------------------- */
/* includes */

#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Miscellaneous functions (rand, malloc, srand)*/
#include <getopt.h> /* get options from system argc/argv */
#include <string.h> /* memset */
#include <math.h>   /* sqrt */
#include <time.h>   /* clock, time, srand */
#include <limits.h> /* INT_MAX */
#include "petri.h" /* To be created for this template if needed */

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
#define IFDEBUG(M) if(DEBUG) fprintf(stderr, "[DEBUG file:%s line:%d]: " M "\n", __FILE__, __LINE__); else {;}

/* limits */
#define SBUFF 256 /**< String buffer */

#define MAX_TOKENS           100
#define DEFAULT_ITERATIONS  1000
#define DEADLOCK_THRESHOLD     5
#define PREFERENCE_PERCENT    55

/* ---------------------------------------------------------------------- */
/* types */

typedef struct s_arc
{
    int place_id;
    int weight;
    struct s_arc *next;
} t_arc;

typedef struct s_transition
{
    int id;
    t_arc *inputs;
    t_arc *outputs;
    struct s_transition *next;
} t_transition;

typedef struct s_place
{
    int id;
    int tokens;
    int max_seen;
    int min_seen;
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

static const char *g_filename = NULL;
static t_petri    *g_net      = NULL;
static int         g_iterations      = 0;
static long        g_trans_checked   = 0;
static int         g_deadlock_streak = 0;

/* ---------------------------------------------------------------------- */
/* prototypes */

void help(void); /* print some help */
void copyr(void); /* print version and copyright information */
void ex7_init(void); /* global initialization function */

static t_petri *petri_read(FILE *fp, int interactive);
static void     petri_free(t_petri *net);
static t_place *petri_get_place(t_petri *net, int id);
static int      petri_enabled(t_transition *tr, t_petri *net);
static void     petri_fire(t_transition *tr, t_petri *net);
static int      petri_step(t_petri *net);
static void     petri_record_stats(t_petri *net);
static void     petri_print_stats(t_petri *net, int iters, long trans_checked, double elapsed);

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
 *    $./ex7 -h
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
     */
    opterr = 0;
    while((opt = getopt(argc, argv, "vhVf:")) != EOF)
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
            case '?':
            default:
                printf("Type\n\t$man %s\nor\n\t$%s -h\nfor help.\n\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

    if(verb)
        printf("Verbose level set at: %d\n", verb);

    ex7_init(); /* initialization function */

    t0 = clock();
    while(g_iterations < DEFAULT_ITERATIONS &&
          g_deadlock_streak < DEADLOCK_THRESHOLD)
    {
        ex7_update();
    }
    elapsed = (double)(clock() - t0) / (double)CLOCKS_PER_SEC;

    petri_print_stats(g_net, g_iterations, g_trans_checked, elapsed);

    petri_free(g_net);
    g_net = NULL;

    return EXIT_SUCCESS;
}

/* ---------------------------------------------------------------------- */
static t_petri *petri_read(FILE *fp, int interactive)
{
    t_petri *net;
    t_place *lastp, *p;
    t_transition *lastt, *tr;
    t_arc *a;
    int n, m, l, t, i, L, T, R;

    net = (t_petri*)malloc(sizeof(t_petri));
    if(!net) { perror("malloc"); exit(EXIT_FAILURE); }
    memset(net, 0, sizeof(*net));

    if(interactive) printf("Quantos lugares? (n>0)\n");
    if(fscanf(fp, "%d", &n) != 1) { fprintf(stderr, "Erro lendo n\n"); exit(EXIT_FAILURE); }

    if(interactive) printf("Quantas transicoes? (m)\n");
    if(fscanf(fp, "%d", &m) != 1) { fprintf(stderr, "Erro lendo m\n"); exit(EXIT_FAILURE); }

    if(interactive) printf("Quantos arcos consumidores do tipo LT? (l)\n");
    if(fscanf(fp, "%d", &l) != 1) { fprintf(stderr, "Erro lendo l\n"); exit(EXIT_FAILURE); }

    if(interactive) printf("Quantos arcos produtores do tipo TL? (t)\n");
    if(fscanf(fp, "%d", &t) != 1) { fprintf(stderr, "Erro lendo t\n"); exit(EXIT_FAILURE); }

    if(n <= 0 || m <= 0 || l < 0 || t < 0)
    {
        fprintf(stderr, "Parametros invalidos da rede\n");
        exit(EXIT_FAILURE);
    }

    net->nplaces = n;
    net->ntrans  = m;
    net->nlt     = l;
    net->ntl     = t;

    lastp = NULL;
    for(i = 0; i < n; i++)
    {
        p = (t_place*)malloc(sizeof(t_place));
        if(!p) { perror("malloc"); exit(EXIT_FAILURE); }
        memset(p, 0, sizeof(*p));
        p->id       = i;
        p->min_seen = INT_MAX;
        p->next     = NULL;
        if(lastp) lastp->next = p;
        else      net->places = p;
        lastp = p;
    }

    if(interactive) printf("Digite quantos tokens em cada lugar, separados por espaco:\n");
    p = net->places;
    for(i = 0; i < n; i++)
    {
        if(fscanf(fp, "%d", &p->tokens) != 1)
        {
            fprintf(stderr, "Erro lendo tokens do lugar %d\n", i);
            exit(EXIT_FAILURE);
        }
        p->max_seen = p->tokens;
        p->min_seen = p->tokens;
        p = p->next;
    }

    lastt = NULL;
    for(i = 0; i < m; i++)
    {
        tr = (t_transition*)malloc(sizeof(t_transition));
        if(!tr) { perror("malloc"); exit(EXIT_FAILURE); }
        memset(tr, 0, sizeof(*tr));
        tr->id = i;
        if(lastt) lastt->next = tr;
        else      net->transitions = tr;
        lastt = tr;
    }

    for(i = 0; i < l; i++)
    {
        if(interactive) printf("Digite a trinca ALT%d de arco consumidor LT no formato L T R:\n", i);
        if(fscanf(fp, "%d %d %d", &L, &T, &R) != 3)
        {
            fprintf(stderr, "Erro lendo arco LT %d\n", i);
            exit(EXIT_FAILURE);
        }
        a = (t_arc*)malloc(sizeof(t_arc));
        if(!a) { perror("malloc"); exit(EXIT_FAILURE); }
        a->place_id = L;
        a->weight   = R;
        a->next     = NULL;

        tr = net->transitions;
        while(tr && tr->id != T) tr = tr->next;
        if(!tr) { fprintf(stderr, "Transicao %d nao encontrada\n", T); exit(EXIT_FAILURE); }

        a->next    = tr->inputs;
        tr->inputs = a;
    }

    for(i = 0; i < t; i++)
    {
        if(interactive) printf("Digite a trinca ATL%d de arco produtor TL no formato T L R:\n", i);
        if(fscanf(fp, "%d %d %d", &T, &L, &R) != 3)
        {
            fprintf(stderr, "Erro lendo arco TL %d\n", i);
            exit(EXIT_FAILURE);
        }
        a = (t_arc*)malloc(sizeof(t_arc));
        if(!a) { perror("malloc"); exit(EXIT_FAILURE); }
        a->place_id = L;
        a->weight   = R;
        a->next     = NULL;

        tr = net->transitions;
        while(tr && tr->id != T) tr = tr->next;
        if(!tr) { fprintf(stderr, "Transicao %d nao encontrada\n", T); exit(EXIT_FAILURE); }

        a->next     = tr->outputs;
        tr->outputs = a;
    }

    return net;
}

/* ---------------------------------------------------------------------- */
static t_place *petri_get_place(t_petri *net, int id)
{
    t_place *p;
    if(!net) return NULL;
    p = net->places;
    while(p && p->id != id) p = p->next;
    return p;
}

static int petri_enabled(t_transition *tr, t_petri *net)
{
    t_arc *a;
    t_place *p;

    if(!tr) return 0;
    for(a = tr->inputs; a; a = a->next)
    {
        p = petri_get_place(net, a->place_id);
        if(!p || p->tokens < a->weight) return 0;
    }
    return 1;
}

static void petri_fire(t_transition *tr, t_petri *net)
{
    t_arc *a;
    t_place *p;

    if(!tr) return;
    for(a = tr->inputs; a; a = a->next)
    {
        p = petri_get_place(net, a->place_id);
        if(p) p->tokens -= a->weight;
    }
    for(a = tr->outputs; a; a = a->next)
    {
        p = petri_get_place(net, a->place_id);
        if(p) p->tokens += a->weight;
    }
}

static int petri_step(t_petri *net)
{
    t_transition **tarr;
    t_transition *tr, *tmp;
    int ntrans, i, j, enabled_count;

    if(!net) return 0;
    ntrans = net->ntrans;
    if(ntrans == 0) return 0;

    tarr = (t_transition**)malloc((size_t)ntrans * sizeof(t_transition*));
    if(!tarr) { perror("malloc"); exit(EXIT_FAILURE); }

    tr = net->transitions;
    for(i = 0; i < ntrans; i++)
    {
        tarr[i] = tr;
        if(tr) tr = tr->next;
    }

    for(i = ntrans - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        tmp = tarr[i]; tarr[i] = tarr[j]; tarr[j] = tmp;
    }

    enabled_count = 0;
    for(i = 0; i < ntrans; i++)
    {
        g_trans_checked++;
        if(petri_enabled(tarr[i], net))
        {
            enabled_count++;
            if((rand() % 100) < PREFERENCE_PERCENT)
            {
                petri_fire(tarr[i], net);
            }
        }
    }

    free(tarr);
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
        p->sum2  += (long long)p->tokens * (long long)p->tokens;
        p->samples++;
    }
}

static void petri_print_stats(t_petri *net, int iters, long trans_checked, double elapsed)
{
    t_place *p;
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
    if(g_deadlock_streak >= DEADLOCK_THRESHOLD)
    {
        printf("Aviso: a rede ficou %d iteracoes sem transicoes habilitadas\n",
               g_deadlock_streak);
    }

    printf("\n--- Tokens por Lugar ---\n");
    printf("%6s %6s %6s %10s %10s %10s\n",
           "Lugar", "Min", "Max", "Media", "Desvio", "Amostras");
    for(p = net->places; p; p = p->next)
    {
        if(p->samples > 0)
        {
            mean = (double)p->sum / (double)p->samples;
            var  = (double)p->sum2 / (double)p->samples - mean * mean;
            if(var < 0.0) var = 0.0;
            sd   = sqrt(var);
        }
        else { mean = 0.0; sd = 0.0; }

        printf("%6d %6d %6d %10.3f %10.3f %10lld\n",
               p->id,
               (p->min_seen == INT_MAX) ? 0 : p->min_seen,
               p->max_seen,
               mean, sd, p->samples);
    }

    printf("\nLugares inalcancaveis (nunca tiveram tokens): ");
    found = 0;
    for(p = net->places; p; p = p->next)
        if(p->max_seen == 0) { printf("%d ", p->id); found = 1; }
    if(!found) printf("nenhum");
    printf("\n");

    printf("Lugares com overflow (max_seen >= %d): ", MAX_TOKENS);
    found = 0;
    for(p = net->places; p; p = p->next)
        if(p->max_seen >= MAX_TOKENS) { printf("%d ", p->id); found = 1; }
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
    printf("\nUsage: %s [-h|-v|-V|-f <arquivo>]\n", "petri");
    printf("\nOptions:\n");
    printf("\t-h,  --help\n\t\tShow this help.\n");
    printf("\t-V,  --version\n\t\tShow version and copyright information.\n");
    printf("\t-v,  --verbose\n\t\tSet verbose level (cumulative).\n");
    printf("\t-f,  --file <nome>\n\t\tLe a rede de Petri do arquivo <nome>\n\t\t(sem esta opcao, le da entrada padrao, de forma interativa).\n");
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
        g_net = petri_read(fp, 0);
        fclose(fp);
    }
    else
    {
        g_net = petri_read(stdin, 1);
    }

    g_iterations      = 0;
    g_trans_checked   = 0;
    g_deadlock_streak = 0;

    petri_record_stats(g_net);

    return;
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief Executa uma iteracao da simulacao
 * @return Void
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
void ex7_update(void)
{
    int had_enabled;

    had_enabled = petri_step(g_net);
    g_iterations++;

    if(had_enabled) g_deadlock_streak = 0;
    else            g_deadlock_streak++;

    petri_record_stats(g_net);

    return;
}

/* ---------------------------------------------------------------------------- */
/* vi: set ai cin et ts=4 sw=4 tw=0 wm=0 fo=croqltn : C config for Vim modeline */
/* Template by Dr. Beco <rcb at beco dot cc>  Version 20160714.153029           */