#include <ncurses.h>
#include <stdbool.h> // para usar variaveis tipo bool
#include <stdlib.h> // rand() srand()
#include <time.h> // time ()

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6

// variaveis globais
int py, px; // @ cords
int attack = 1;
int hp = 10;

bool p_placed = 0; //player
bool t_placed = 0; //goblin

int c_placed = 0; //caverna
int p_ouro = 0; //ouro do player
int dlvl = 1;

struct monstros {
    int y;
    int x;
    int lvl;
    int type;
    bool acordado;
};

struct monstros monstro[10];

int desenho_caverna (int linhas, int colunas, char (*mapa)[colunas]) 
{
    // desenhar no ecra
    for (int y = 0; y <= linhas ; y++)
    {
        for (int x = 0; x <= colunas ; x++)
        {  
            // empty floor
            if (mapa[y][x] == ' ')
                mvaddch(y,x,' ');
            // stairs
            else if (mapa[y][x] == '>')
            {
                attron(A_BOLD);
                mvaddch(y,x,'>'); 
                standend();
            }
            //ultima linha vazia para colocar dados
            else if (y == 0 || y == linhas)
                mvaddch(y,x,' ');
            // paredes permanentes
            else if (mapa[y][x] == '%')
                mvaddch(y,x,'%');
            // paredes normais
            else if (mapa[y][x] == '#')
                mvaddch(y,x,'#');
            // monstros
            else if (mapa[y][x] == 't')
            {
                for(int m = 0; m < 10; m++)
                {
                    if(monstro[m].y == y && monstro[m].x == x) 
                    { 
                    if(monstro[m].lvl < dlvl / 2 + 2)
                    {
                        //pouca vida
                        attron(COLOR_PAIR(RED));
                        mvaddch(y,x,monstro[m].type);
                    }
                    else if(monstro[m].lvl < dlvl + 2)
                    {
                        // menos do que a vida toda
                        attron(COLOR_PAIR(YELLOW));
                        mvaddch(y,x,monstro[m].type);
                    }
                    else 
                        mvaddch(y,x,monstro[m].type);

                    standend();

                    }

                }
            }
        }
    }

    mvprintw(linhas, 0, "HP: %d \t Attack: %d \t Ouro: %d \t Dlvl: %d", hp, attack, p_ouro, dlvl);

    return 0;
}

int monstro_acorda(int colunas, char (*mapa)[colunas]) 
{
    int dist_y , dist_x;

    for(int m = 0; m < 10; m++)
    {
        if(monstro[m].lvl < 1)
        continue;

        dist_y = abs(monstro[m].y - py); // caso tenha valores negativos
        dist_x = abs(monstro[m].x - px);

        if(dist_y < 4 && dist_x < 4)
            monstro[m].acordado = 1;

        if(monstro[m].acordado == 0)
            continue;

        int dir_y = monstro[m].y;
        int dir_x = monstro[m].x;

        // movimento simples
        if(dist_y > dist_x)
        {
            if (dir_y < py)
                dir_y++;
            else
                dir_y--;
        }
        else 
        {
            if(dir_x < px)
                dir_x++;
            else
                dir_x--;
        }

    if (mapa[dir_y][dir_x] == '#' || mapa[dir_y][dir_x] == '%')
    {
        // movimento na diagonal(contornar cantos)

        dir_y = monstro[m].y;
        dir_x = monstro[m].x;

        if(dir_y < py)
            dir_y++;
        else 
            dir_y--;

        if(dir_x < px)
            dir_x++;
        else
            dir_x--;
    }

    // caso fiquem presos na parede
    if (mapa[dir_y][dir_x] == '#' || mapa[dir_y][dir_x] == '%')
    {
        if(dist_y > dist_x)
        {
            if (dir_y > py)
                dir_y++;
            else
                dir_y--;
        }
        else 
        {
            if(dir_x > px)
                dir_x++;
            else
                dir_x--;
        }
    }


        if (dist_y <= 1 && dist_x <= 1)
        {
            hp-= dlvl / 2 + 1;
        }
        else if (mapa[dir_y][dir_x] == ' ')
        {
            mapa[monstro[m].y][monstro[m].x] = ' ';
            monstro[m].y = dir_y;
            monstro[m].x = dir_x;
            mapa[monstro[m].y][monstro[m].x] = 't';
        }

    }


return 0;
}

int battle (int colunas, char (*mapa)[colunas], int dir_y, int dir_x) 
{

        for(int m = 0; m < 10; m++)
        {
            if(monstro[m].y == dir_y && monstro[m].x == dir_x) 
            {
                if(monstro[m].lvl <= 0) 
                {
                    mapa[monstro[m].y][monstro[m].x] = ' ';
                    p_ouro += rand() % 10 + 1; //random ouro de 1 a 10
                }
                else 
                    monstro[m].lvl-= attack;
            }

        }
        return 0;
}

int p_action(int c,int colunas, char (*mapa)[colunas]) 
{
    int dir_y = py, dir_x = px;

    if      (c == KEY_UP)
        dir_y = py - 1;
    else if (c == KEY_DOWN)
        dir_y = py + 1;
    else if (c == KEY_LEFT)
        dir_x = px - 1 ;
    else if (c == KEY_RIGHT) 
        dir_x = px + 1;
    else if (c == '>')
    {
        p_placed = 0; 
        t_placed = 0; 
        c_placed = 0;
        return 1;
    }
    

    if (mapa[dir_y][dir_x] == ' ' || mapa[dir_y][dir_x] == '>')
    {
        py = dir_y;
        px = dir_x;
    }
    else if (mapa[dir_y][dir_x] == 't')
        battle(colunas, mapa, dir_y, dir_x);


        
return 0;
}

int spawn_creatures(int linhas, int colunas, char (*mapa)[colunas]) 
{
    // spawn random player
    if(p_placed == 0) {
        do {
            py = rand() % linhas;
            px = rand () % colunas;
        }
        while(mapa[py][px] != ' ');

        mapa[py][px] = ' ';

        p_placed = 1;
    }

    // spawn monstros
    if (t_placed == 0) 
    {
        int y, x;

        for(int m = 0; m < 10; m++)
        {
            do 
            {
                y = rand() % linhas;
                x = rand () % colunas;
            }
            while(mapa[y][x] != ' ');

            monstro[m].y = y;
            monstro[m].x = x;

            monstro[m].lvl = rand () % dlvl + 2; // lvl 1 : from 2 to 3

            if (dlvl == 1 && !(rand() % 3))
                monstro[m].lvl = 1;

            else if (rand() % 2)
                    monstro[m].lvl = dlvl + 2;



            
                

            monstro[m].type = rand () % dlvl + 97; // 96 é onde começa a,b,c
            if(rand() % 5)
                monstro[m].type += 1;

            monstro[m].acordado = 0; // no inicio vao estar todos a dormir


            mapa[monstro[m].y][monstro[m].x] = 't';
        }

            t_placed = 1;
    }
    return 0;
}

int geracao_caverna (int linhas, int colunas, char (*mapa)[colunas]) 
{
    // preencher database
    if (c_placed == 0) 
    {
        for (int y = 0; y <= linhas ; y++)
        {
            for (int x = 0; x <= colunas ; x++)
            {
                // Primeira e ultima borda
                if (y == 0 || y == 1 || y == linhas - 1)
                    mapa[y][x] = '%';
                // Borda da esquerda e borda da direita
                else if ((x == 0 || x == colunas) && y != linhas)
                    mapa[y][x] = '%';
                // Player Interface line
                else if (y == linhas)
                    mapa[y][x] = '%';
                // Paredes internas
                else 
                    mapa[y][x] = '#';
            }
        }
    }

    // geracao de caverna
    if(c_placed == 0) 
    {

        int cy,cx; //cordenadas cavernas
        int c_tamanho_y;
        int c_tamanho_x; //tamanho cavernas

        int c_centro_y = 0;
        int c_centro_x = 0;
        int c_antigo_centro_y;
        int c_antigo_centro_x;


        int num_cavernas = rand() % 5 + 5; //gerar num cavernas

        while (c_placed < num_cavernas) {

            bool colisao;
            int contagem_loop =  0;

            do
            {
                colisao = 0;

                do {
                    // gerar caverna cords
                    cy = rand() % linhas - 3; //nao queremos cavernas nas bordas
                    cx = rand () % colunas - 3;


                    // gerar tamanho caverna
                    c_tamanho_y = rand() % 5 + 4;
                    c_tamanho_x = rand() % 10 + 8;
                }
                while(cy < 1 || cx < 1);

                // check de colisoes
                for (int y = cy; y <= cy + c_tamanho_y ; y++)
                {
                    for (int x = cx; x <= cx + c_tamanho_x ; x++)
                    {  
                        if (mapa[y][x] == ' ' || mapa[y][x] == '%' || mapa[y + 2][x] == ' ' || mapa[y - 2][x] == ' ' 
                        || mapa[y][x + 2 ] == ' ' || mapa[y][x - 2] == ' ')
                        {  
                            colisao = 1;
                            y = cy + c_tamanho_y; // sair do loop de cima ...
                            break; // ... agora sair do loop atual
                        }
                    }
                }

                //prevencao de erro (sem espaco para cavernas)
                contagem_loop++;
                if (contagem_loop > 100) 
                {   
                    cy = 11; cx = 11;
                    c_tamanho_y = 2; c_tamanho_x = 2;
                    break;
                }


        }
        while (colisao);

            // preencher database do mapa com cordenadas das cavernas
            for (int y = cy; y <= cy + c_tamanho_y ; y++)
            {
                for (int x = cx; x <= cx + c_tamanho_x ; x++)
                {  
                    if (mapa[y][x] != '%' && 0 < y && y < linhas - 1 && 0 < x && x < colunas)
                    {
                    mapa[y][x] = ' ';
                    }
                }
            }

            c_placed++;

            // centro da caverna anterior
            if (c_placed > 0) 
            {
                c_antigo_centro_y = c_centro_y;
                c_antigo_centro_x = c_centro_x;
            }

            c_centro_y = cy + (c_tamanho_y / 2);
            c_centro_x = cx + (c_tamanho_x / 2);

            //corredores
            if(c_placed > 1) 
            {
                int antigo_y;

                for (antigo_y = c_antigo_centro_y; antigo_y != c_centro_y ;)
                {
                    mapa[antigo_y][c_antigo_centro_x] = ' ';

                    if (c_antigo_centro_y < c_centro_y)
                    antigo_y++;
                    else if (c_antigo_centro_y > c_centro_y)
                    antigo_y--;
                }

                for (int x = c_antigo_centro_x; x != c_centro_x ;)
                {
                    mapa[antigo_y][x] = ' ';

                    if (c_antigo_centro_x < c_centro_x)
                    x++;
                    else if (c_antigo_centro_x > c_centro_x)
                    x--;
                }

            }

        }

    //gen stairs
    int sy,sx;
    do 
    {
        sy = rand() % linhas - 3; //nao queremos stairs nas bordas
        sx = rand () % colunas - 3;
    }
        while(mapa[sy][sx] != ' ');
        mapa[sy][sx] = '>';

    }


return 0;
}

int game_loop (int c, int linhas, int colunas, char (*mapa)[colunas]) {

bool new_lvl = 0;

    srand(time(NULL));

    geracao_caverna(linhas,colunas, mapa);

    spawn_creatures(linhas,colunas, mapa);

    if(c != 0) 
        new_lvl = p_action(c,colunas, mapa); //include battle()

    monstro_acorda(colunas, mapa);

    desenho_caverna (linhas,colunas, mapa);

    attron(A_BOLD);
    mvaddch(py,px,'@'); // desenhar jogador
    standend();


    if(new_lvl)
    {
        mvprintw(0, 0, "Bem-vindo ao nível %d! (Pressione ENTER para continuar)", ++dlvl);
        attack += rand() % 4 + 2;
    }

    if(hp < 1)
    {
        clear();
        mvprintw(linhas/2, colunas/2 -10 , "RIP. Ganhaste %d ouro", p_ouro);
    }

    c = getch();

    return c; // return p input
}



int main () {
    int c = 0; // input
    int linhas, colunas;

    initscr(); // inicializar modo de tela

    start_color();
    use_default_colors();

    init_pair(RED, COLOR_RED , COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN , COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW , COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE , COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA , COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN , COLOR_BLACK);

    keypad(stdscr, 1); // permitir SETAS, F1-F12

    noecho(); // esconder user input
    curs_set(0); // esconder cursor a piscar

    getmaxyx(stdscr,linhas,colunas);

    char mapa[linhas][colunas];

do
    c = game_loop(c, linhas - 1,colunas - 1, mapa);
while (c != 27); //27 = 'ESC'
    

    refresh();
    getch();
    endwin();

    return 0;
}