#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <optional>
using namespace std;

// ─── WINDOW CONSTANTS ───────────────────────
static const int WIN_W = 700;
static const int WIN_H = 700;
static const int PANEL = 100;
static const int MAZE_H = WIN_H - PANEL;  // 600px for maze
static const int MAX_LEVELS = 3;

// ─── DIFFICULTY ──────────────────────────────
struct DiffConfig {
    string name;
    int rows, cols;
    float baseTime;
    sf::Color accent, wallCol, floorCol;
};
// cellSize computed dynamically from window size
const DiffConfig DIFFS[3] = {
    {"EASY",   11, 11, 120.f, sf::Color(50,230,120), sf::Color(30,22,48), sf::Color(242,239,228)},
    {"MEDIUM", 15, 15,  90.f, sf::Color(255,190,30), sf::Color(38,18,55), sf::Color(236,231,220)},
    {"HARD",   21, 21,  60.f, sf::Color(255,70,100), sf::Color(42,10,32), sf::Color(230,224,212)},
};

// ─── CELL / MAZE ────────────────────────────
struct Cell {
    bool visited=false, walls[4];
    Cell(){walls[0]=walls[1]=walls[2]=walls[3]=true;}
};

class Maze {
public:
    int R,C,CS;
    vector<vector<Cell>> g;
    const int DX[4]={0,1,0,-1};
    const int DY[4]={-1,0,1,0};

    Maze():R(11),C(11),CS(54){reset();}

    void setup(int r,int c){
        R=r; C=c;
        CS = min(WIN_W/C, MAZE_H/R);
        reset();
    }
    void reset(){g.assign(R,vector<Cell>(C));}
    bool ok(int x,int y)const{return x>=0&&x<C&&y>=0&&y<R;}

    void carve(int x1,int y1,int x2,int y2){
        int dx=x2-x1,dy=y2-y1;
        if(dx== 1){g[y1][x1].walls[1]=false;g[y2][x2].walls[3]=false;}
        if(dx==-1){g[y1][x1].walls[3]=false;g[y2][x2].walls[1]=false;}
        if(dy== 1){g[y1][x1].walls[2]=false;g[y2][x2].walls[0]=false;}
        if(dy==-1){g[y1][x1].walls[0]=false;g[y2][x2].walls[2]=false;}
    }

    void generate(){
        reset();
        mt19937 rng((unsigned)time(nullptr));
        stack<pair<int,int>> st;
        g[0][0].visited=true; st.push({0,0});
        while(!st.empty()){
            int x=st.top().first,y=st.top().second;
            vector<int> d={0,1,2,3}; shuffle(d.begin(),d.end(),rng);
            bool moved=false;
            for(int di:d){
                int nx=x+DX[di],ny=y+DY[di];
                if(ok(nx,ny)&&!g[ny][nx].visited){
                    g[ny][nx].visited=true; carve(x,y,nx,ny);
                    st.push({nx,ny}); moved=true; break;
                }
            }
            if(!moved) st.pop();
        }
    }

    bool canMove(int x,int y,int nx,int ny)const{
        if(!ok(nx,ny))return false;
        if(nx==x+1&&!g[y][x].walls[1])return true;
        if(nx==x-1&&!g[y][x].walls[3])return true;
        if(ny==y+1&&!g[y][x].walls[2])return true;
        if(ny==y-1&&!g[y][x].walls[0])return true;
        return false;
    }

    // offsets to center maze in window
    int offX()const{return (WIN_W - C*CS)/2;}
    int offY()const{return (MAZE_H - R*CS)/2;}

    void draw(sf::RenderWindow& win, sf::Color fc, sf::Color wc){
        int ox=offX(), oy=offY();
        for(int y=0;y<R;y++) for(int x=0;x<C;x++){
            float px=(float)(ox+x*CS), py=(float)(oy+y*CS);
            sf::RectangleShape cell(sf::Vector2f((float)CS,(float)CS));
            cell.setPosition(sf::Vector2f(px,py)); cell.setFillColor(fc); win.draw(cell);
            sf::RectangleShape w; w.setFillColor(wc);
            if(g[y][x].walls[0]){w.setSize(sf::Vector2f((float)CS+2,3));  w.setPosition(sf::Vector2f(px-1,py));      win.draw(w);}
            if(g[y][x].walls[1]){w.setSize(sf::Vector2f(3,(float)CS+2));  w.setPosition(sf::Vector2f(px+CS-1,py-1)); win.draw(w);}
            if(g[y][x].walls[2]){w.setSize(sf::Vector2f((float)CS+2,3));  w.setPosition(sf::Vector2f(px-1,py+CS-1));win.draw(w);}
            if(g[y][x].walls[3]){w.setSize(sf::Vector2f(3,(float)CS+2));  w.setPosition(sf::Vector2f(px-1,py-1));   win.draw(w);}
        }
    }
};

// ─── BFS ────────────────────────────────────
vector<pair<int,int>> runBFS(const Maze& m,int sx,int sy,int ex,int ey){
    vector<vector<bool>> vis(m.R,vector<bool>(m.C,false));
    vector<vector<pair<int,int>>> par(m.R,vector<pair<int,int>>(m.C,{-1,-1}));
    queue<pair<int,int>> q; q.push({sx,sy}); vis[sy][sx]=true;
    while(!q.empty()){
        auto[x,y]=q.front();q.pop();
        if(x==ex&&y==ey)break;
        for(int d=0;d<4;d++){
            int nx=x+m.DX[d],ny=y+m.DY[d];
            if(m.canMove(x,y,nx,ny)&&!vis[ny][nx]){vis[ny][nx]=true;par[ny][nx]={x,y};q.push({nx,ny});}
        }
    }
    vector<pair<int,int>> path;
    if(!vis[ey][ex])return path;
    pair<int,int> cur={ex,ey};
    while(cur!=make_pair(sx,sy)){path.push_back(cur);cur=par[cur.second][cur.first];}
    reverse(path.begin(),path.end()); return path;
}

struct BTSolver{
    vector<pair<int,int>> result;
    vector<vector<bool>> vis;
    bool solve(const Maze& m,int x,int y,int ex,int ey,vector<pair<int,int>>& p){
        if(x==ex&&y==ey){result=p;result.push_back({x,y});return true;}
        vis[y][x]=true; p.push_back({x,y});
        for(int d=0;d<4;d++){int nx=x+m.DX[d],ny=y+m.DY[d];if(m.canMove(x,y,nx,ny)&&!vis[ny][nx])if(solve(m,nx,ny,ex,ey,p))return true;}
        p.pop_back(); return false;
    }
    vector<pair<int,int>> run(const Maze& m,int sx,int sy,int ex,int ey){
        result.clear();vis.assign(m.R,vector<bool>(m.C,false));
        vector<pair<int,int>> p;solve(m,sx,sy,ex,ey,p);return result;
    }
};

// ─── PARTICLES ──────────────────────────────
struct Particle{float x,y,vx,vy,life,maxLife,size; sf::Color color;};

// ─── GAME STATE ─────────────────────────────
enum class GS { MENU, PLAYING, LEVEL_WIN, GAME_WIN, GAME_OVER };

// ─── GAME ────────────────────────────────────
class Game {
    sf::RenderWindow win;
    sf::Font font; bool hasFont=false;

    int diffIdx=0;   // <-- starts on EASY
    Maze maze;
    GS state=GS::MENU;

    int px,py,gx,gy;
    float drawX,drawY;
    float timeLeft,totalTime;
    int score,levelScore,currentLevel,moves,totalScore;

    bool hintOn=false; vector<pair<int,int>> hintPath;
    bool autoOn=false; vector<pair<int,int>> autoPath; int autoIdx=0; float autoTick=0;

    sf::Clock sceneClock,loopClock,animClock;
    float stateTimer=0,shakeAmt=0;
    vector<Particle> particles;
    mt19937 rng;

public:
    Game():win(sf::VideoMode({(unsigned)WIN_W,(unsigned)WIN_H}),"Rat in a Maze | DAA Lab | JIIT"),
           rng((unsigned)time(nullptr))
    {
        win.setFramerateLimit(60);
        if(font.openFromFile("ARIAL.TTF")) hasFont=true;
        else if(font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) hasFont=true;
        else cout<<"Font not found\n";
        currentLevel=1; totalScore=0; score=0; levelScore=0;
    }

    const DiffConfig& D(){return DIFFS[diffIdx];}

    // ── text ──
    void lbl(const string& s,float x,float y,int sz,sf::Color c,bool center=false){
        if(!hasFont)return;
        sf::Text t(font,s,(unsigned)sz); t.setFillColor(c);
        if(center){auto b=t.getLocalBounds();t.setOrigin(sf::Vector2f(b.size.x/2.f,b.size.y/2.f));}
        t.setPosition(sf::Vector2f(x,y)); win.draw(t);
    }
    void lblS(const string& s,float x,float y,int sz,sf::Color c,bool center=false){
        lbl(s,x+2,y+2,sz,sf::Color(0,0,0,140),center);
        lbl(s,x,y,sz,c,center);
    }

    // ── rect helper ──
    void rect(float x,float y,float w,float h,sf::Color c){
        sf::RectangleShape r(sf::Vector2f(w,h)); r.setPosition(sf::Vector2f(x,y)); r.setFillColor(c); win.draw(r);
    }

    // ── particles ──
    void spawn(float x,float y,sf::Color c,int n=16){
        for(int i=0;i<n;i++){
            Particle p; p.x=x;p.y=y;
            float a=((float)(rng()%1000)/1000.f)*6.28f;
            float sp=50.f+(float)(rng()%100);
            p.vx=cosf(a)*sp;p.vy=sinf(a)*sp;
            p.life=p.maxLife=0.5f+(float)(rng()%60)/100.f;
            p.size=3.f+(float)(rng()%8);p.color=c;
            particles.push_back(p);
        }
    }
    void confetti(int n=90){
        sf::Color cols[]={sf::Color(255,80,100),sf::Color(255,200,30),sf::Color(50,230,120),sf::Color(80,160,255),sf::Color(230,80,255)};
        for(int i=0;i<n;i++){
            Particle p; p.x=(float)(rng()%WIN_W);p.y=-20.f;
            p.vx=(float)((int)(rng()%200)-100);p.vy=80+(float)(rng()%120);
            p.life=p.maxLife=1.5f+(float)(rng()%100)/100.f;
            p.size=4+(float)(rng()%7);p.color=cols[rng()%5];
            particles.push_back(p);
        }
    }
    void tickParticles(float dt){
        for(auto& p:particles){p.x+=p.vx*dt;p.y+=p.vy*dt;p.vy+=110*dt;p.life-=dt;}
        particles.erase(remove_if(particles.begin(),particles.end(),[](const Particle& p){return p.life<=0;}),particles.end());
    }
    void drawParticles(){
        for(auto& p:particles){
            float a=p.life/p.maxLife;
            sf::CircleShape c(p.size*a); c.setFillColor(sf::Color(p.color.r,p.color.g,p.color.b,(uint8_t)(a*220)));
            c.setPosition(sf::Vector2f(p.x,p.y)); win.draw(c);
        }
    }

    // ════════════════════════════════════════
    //  MENU
    // ════════════════════════════════════════
    void menuInput(){
        while(auto ev=win.pollEvent()){
            if(ev->is<sf::Event::Closed>())win.close();
            if(const auto* kp=ev->getIf<sf::Event::KeyPressed>()){
                if(kp->code==sf::Keyboard::Key::Escape)win.close();
                // Number keys to select difficulty
                if(kp->code==sf::Keyboard::Key::Num1){diffIdx=0;}
                if(kp->code==sf::Keyboard::Key::Num2){diffIdx=1;}
                if(kp->code==sf::Keyboard::Key::Num3){diffIdx=2;}
                if(kp->code==sf::Keyboard::Key::Up)  diffIdx=(diffIdx+2)%3;
                if(kp->code==sf::Keyboard::Key::Down)diffIdx=(diffIdx+1)%3;
                if(kp->code==sf::Keyboard::Key::E)   diffIdx=0;
                if(kp->code==sf::Keyboard::Key::M)   diffIdx=1;
                if(kp->code==sf::Keyboard::Key::H)   diffIdx=2;
                // Enter / Space starts game
                if(kp->code==sf::Keyboard::Key::Enter||kp->code==sf::Keyboard::Key::Space){
                    startGame(); return;
                }
            }
            // Mouse click on buttons
            if(const auto* mb=ev->getIf<sf::Event::MouseButtonPressed>()){
                if(mb->button==sf::Mouse::Button::Left){
                    float mx=(float)mb->position.x, my=(float)mb->position.y;
                    // Button positions: centered at x=350, y=245,340,435
                    float bys[3]={245.f,340.f,435.f};
                    for(int i=0;i<3;i++){
                        if(mx>=210&&mx<=490&&my>=bys[i]-35&&my<=bys[i]+35){
                            diffIdx=i;
                            startGame(); return;
                        }
                    }
                }
            }
        }
    }

    void menuDraw(){
        float t=animClock.getElapsedTime().asSeconds();
        win.clear(sf::Color(8,6,20));

        // Animated background grid
        for(int i=0;i<25;i++){
            float fy=fmod(i*28.f+t*16.f,(float)WIN_H);
            rect(0,fy,(float)WIN_W,1,sf::Color(255,255,255,6));
        }
        for(int i=0;i<25;i++){
            rect((float)(i*28),0,1,(float)WIN_H,sf::Color(255,255,255,6));
        }

        // Glow behind title
        float g=0.5f+0.5f*sinf(t*2.f);
        rect(80,50,540,80,sf::Color(60,40,160,(uint8_t)(22*g)));

        // Title
        lblS("RAT  IN  A  MAZE",350,62,42,sf::Color::White,true);
        lbl("D A A   L A B   P R O J E C T   |   J I I T",350,116,13,sf::Color(130,120,175),true);

        // Divider
        rect(180,146,340,2,sf::Color(80,60,180,200));

        lbl("CHOOSE  YOUR  DIFFICULTY",350,162,16,sf::Color(180,170,220),true);

        // Difficulty buttons
        sf::Color aCol[]={sf::Color(50,230,120),sf::Color(255,190,30),sf::Color(255,70,100)};
        sf::Color dkCol[]={sf::Color(12,60,30),sf::Color(80,60,8),sf::Color(80,18,30)};
        const char* names[]={"EASY","MEDIUM","HARD"};
        const char* descs[]={"11x11 maze  |  120 seconds  |  Beginner",
                              "15x15 maze  |   90 seconds  |  Balanced",
                              "21x21 maze  |   60 seconds  |  Expert"};
        float bys[]={245.f,340.f,435.f};

        for(int i=0;i<3;i++){
            bool sel=(diffIdx==i);
            float by=bys[i];
            float pulse=sel?(0.5f+0.5f*sinf(t*4.f)):0.f;

            // Shadow
            rect(213,by-33,284,68,sf::Color(0,0,0,80));

            // Button body
            sf::Color bodyCol=sel?sf::Color(aCol[i].r/2,aCol[i].g/2,aCol[i].b/2,220):dkCol[i];
            rect(210,by-35,280,70,bodyCol);

            // Left accent stripe
            rect(210,by-35,6,70,sel?aCol[i]:sf::Color(aCol[i].r/4,aCol[i].g/4,aCol[i].b/4));

            // Border glow if selected
            if(sel){
                uint8_t ba=(uint8_t)(160+95*pulse);
                rect(210,by-35,280,2,sf::Color(aCol[i].r,aCol[i].g,aCol[i].b,ba));
                rect(210,by+33,280,2,sf::Color(aCol[i].r,aCol[i].g,aCol[i].b,ba));
                rect(488,by-35,2,70,sf::Color(aCol[i].r,aCol[i].g,aCol[i].b,ba));
            }

            // Text
            lblS(names[i],310,by-20,26,sel?aCol[i]:sf::Color(200,195,225));
            lbl(descs[i],310,by+8,12,sel?sf::Color(220,220,220):sf::Color(110,105,140));

            // Selected indicator
            if(sel) lblS(">> SELECTED",390,by-20,13,aCol[i]);
        }

        // Instructions
        lbl("Click a button  OR  use [Up/Down] then press [ENTER]",350,510,13,sf::Color(160,150,200),true);

        float pa=0.5f+0.5f*sinf(t*3.f);
        lbl("[ ENTER ] to start game",350,542,15,sf::Color(220,210,255,(uint8_t)(200*pa)),true);
        lbl("[1] Easy   [2] Medium   [3] Hard",350,570,11,sf::Color(90,85,120),true);

        drawParticles();
        win.display();
    }

    // ════════════════════════════════════════
    //  GAME START / LEVEL
    // ════════════════════════════════════════
    void startGame(){
        currentLevel=1; totalScore=0; score=0; levelScore=0;
        particles.clear();
        maze.setup(D().rows, D().cols);
        startLevel();
        state=GS::PLAYING;
    }

    void startLevel(){
        maze.setup(D().rows, D().cols);
        maze.generate();
        px=py=0; gx=D().cols-1; gy=D().rows-1;
        int ox=maze.offX(), oy=maze.offY(), CS=maze.CS;
        drawX=(float)(ox+CS/2); drawY=(float)(oy+CS/2);
        totalTime=timeLeft=max(20.f, D().baseTime-(currentLevel-1)*15.f);
        moves=0; levelScore=0;
        hintOn=false; hintPath.clear();
        autoOn=false; autoPath.clear(); autoIdx=0; autoTick=0;
        particles.clear(); shakeAmt=0; stateTimer=0;
        sceneClock.restart();
    }

    // ════════════════════════════════════════
    //  PLAY INPUT
    // ════════════════════════════════════════
    void playInput(){
        while(auto ev=win.pollEvent()){
            if(ev->is<sf::Event::Closed>())win.close();
            if(const auto* kp=ev->getIf<sf::Event::KeyPressed>()){
                if(kp->code==sf::Keyboard::Key::Escape){state=GS::MENU;particles.clear();return;}

                // Level win — any key advances
                if(state==GS::LEVEL_WIN){
                    currentLevel++;
                    totalScore+=levelScore;
                    if(currentLevel>MAX_LEVELS){
                        state=GS::GAME_WIN; confetti(140); stateTimer=0;
                    } else {
                        startLevel(); state=GS::PLAYING;
                    }
                    return;
                }
                // Game over
                if(state==GS::GAME_OVER){
                    if(kp->code==sf::Keyboard::Key::R)startGame();
                    else if(kp->code==sf::Keyboard::Key::Escape){state=GS::MENU;particles.clear();}
                    return;
                }
                // Game win
                if(state==GS::GAME_WIN){
                    if(kp->code==sf::Keyboard::Key::R)startGame();
                    else if(kp->code==sf::Keyboard::Key::Escape){state=GS::MENU;particles.clear();}
                    return;
                }

                if(kp->code==sf::Keyboard::Key::R){startLevel();return;}
                if(kp->code==sf::Keyboard::Key::H){
                    hintOn=!hintOn; autoOn=false;
                    hintPath=hintOn?runBFS(maze,px,py,gx,gy):vector<pair<int,int>>{};
                    return;
                }
                if(kp->code==sf::Keyboard::Key::B){
                    autoOn=!autoOn; hintOn=false; hintPath.clear();
                    if(autoOn){BTSolver bt;autoPath=bt.run(maze,px,py,gx,gy);autoIdx=0;}
                    return;
                }

                if(autoOn||state!=GS::PLAYING)return;

                int nx=px,ny=py;
                if(kp->code==sf::Keyboard::Key::W||kp->code==sf::Keyboard::Key::Up)   ny--;
                if(kp->code==sf::Keyboard::Key::S||kp->code==sf::Keyboard::Key::Down) ny++;
                if(kp->code==sf::Keyboard::Key::A||kp->code==sf::Keyboard::Key::Left) nx--;
                if(kp->code==sf::Keyboard::Key::D||kp->code==sf::Keyboard::Key::Right)nx++;

                if(nx!=px||ny!=py){
                    if(maze.canMove(px,py,nx,ny)){
                        px=nx;py=ny;moves++;
                        int CS=maze.CS,ox=maze.offX(),oy2=maze.offY();
                        spawn((float)(ox+px*CS+CS/2),(float)(oy2+py*CS+CS/2),D().accent,3);
                        if(hintOn)hintPath=runBFS(maze,px,py,gx,gy);
                        if(px==gx&&py==gy){
                            levelScore=max(0,(int)(timeLeft*10)-moves*2+300);
                            spawn((float)(ox+px*CS+CS/2),(float)(oy2+py*CS+CS/2),D().accent,70);
                            state=GS::LEVEL_WIN; stateTimer=0;
                        }
                    } else { shakeAmt=5.f; }
                }
            }
        }
    }

    // ════════════════════════════════════════
    //  UPDATE
    // ════════════════════════════════════════
    void update(float dt){
        stateTimer+=dt; shakeAmt*=0.80f;
        if(state==GS::PLAYING){
            timeLeft-=dt;
            if(timeLeft<=0){
                timeLeft=0; shakeAmt=10; state=GS::GAME_OVER; stateTimer=0;
                spawn((float)WIN_W/2,(float)WIN_H/2,sf::Color(255,60,60),40);
            }
            if(autoOn&&!autoPath.empty()&&autoIdx<(int)autoPath.size()){
                autoTick+=dt;
                if(autoTick>=0.12f){
                    autoTick=0;
                    int nx=autoPath[autoIdx].first,ny=autoPath[autoIdx].second;autoIdx++;
                    if(maze.canMove(px,py,nx,ny)){
                        px=nx;py=ny;moves++;
                        if(px==gx&&py==gy){
                            levelScore=max(0,(int)(timeLeft*10)-moves*2+300);
                            int CS=maze.CS,ox=maze.offX(),oy2=maze.offY();
                            spawn((float)(ox+px*CS+CS/2),(float)(oy2+py*CS+CS/2),D().accent,70);
                            state=GS::LEVEL_WIN; autoOn=false; stateTimer=0;
                        }
                    }
                }
            }
        }
        if(state==GS::GAME_WIN&&(int)(stateTimer*4)%2==0)confetti(4);

        // Smooth player movement
        int CS=maze.CS,ox=maze.offX(),oy2=maze.offY();
        float tx=(float)(ox+px*CS+CS/2), ty=(float)(oy2+py*CS+CS/2);
        float k=min(1.f,dt*14);
        drawX+=(tx-drawX)*k; drawY+=(ty-drawY)*k;
        tickParticles(dt);
    }

    // ════════════════════════════════════════
    //  DRAW PANEL
    // ════════════════════════════════════════
    void drawPanel(){
        float by=(float)MAZE_H;
        // Background
        rect(0,by,(float)WIN_W,(float)PANEL,sf::Color(10,8,24));
        // Top accent line
        rect(0,by,(float)WIN_W,3,D().accent);

        // Timer bar
        rect(14,by+12,(float)WIN_W-28,8,sf::Color(35,30,55));
        float r=max(0.f,timeLeft/totalTime);
        sf::Color bc=r>0.5f?D().accent:r>0.25f?sf::Color(255,175,0):sf::Color(255,45,45);
        rect(14,by+12,(float)(WIN_W-28)*r,8,bc);

        // Level pips
        float pipX=(float)WIN_W-20.f;
        for(int i=MAX_LEVELS;i>=1;i--){
            bool done=i<currentLevel, curr=i==currentLevel;
            float rad=curr?8.f:6.f;
            sf::CircleShape pip(rad); pip.setOrigin(sf::Vector2f(rad,rad));
            pip.setPosition(sf::Vector2f(pipX,by+48));
            pip.setFillColor((done||curr)?D().accent:sf::Color(45,40,65));
            win.draw(pip);
            pipX-=26.f;
        }
        lbl("LEVEL",pipX-8,by+41,11,sf::Color(110,105,140));

        // Stats row
        stringstream ss;
        ss<<"LVL "<<currentLevel<<"/"<<MAX_LEVELS
          <<"   SCORE: "<<totalScore+levelScore
          <<"   MOVES: "<<moves
          <<"   TIME: "<<fixed<<setprecision(1)<<timeLeft<<"s"
          <<"   ["<<D().name<<"]";
        lbl(ss.str(),14,by+28,14,sf::Color(210,205,235));

        // Controls
        lbl("[WASD/Arrows] Move  [H] BFS Hint  [B] Auto-Solve  [R] Restart  [ESC] Menu",
            14,by+60,10,sf::Color(80,75,112));

        if(hintOn){string s="BFS: "+to_string(hintPath.size())+" steps";lbl(s,14,by+76,10,sf::Color(50,215,100));}
        if(autoOn)lbl("AUTO-SOLVING...",14,by+76,10,sf::Color(80,165,255));
    }

    // ════════════════════════════════════════
    //  BIG OVERLAY (win/lose/level)
    // ════════════════════════════════════════
    void bigOverlay(sf::Color dimCol,
                    const string& tag, sf::Color tagCol,
                    const string& bigText, sf::Color bigCol,
                    const string& line1,
                    const string& line2,
                    const string& hint){
        float t=stateTimer;
        float W=(float)WIN_W, H=(float)MAZE_H;

        // Dim
        rect(0,0,W,H,dimCol);

        // Card slide in
        float slide=max(0.f,(1.f-t*6.f))*80.f;
        float cw=min(W-60.f,440.f), ch=240.f;
        float cx=W/2.f, cy=H/2.f+slide;

        // Card shadow
        rect(cx-cw/2+8,cy-ch/2+8,cw,ch,sf::Color(0,0,0,100));
        // Card body
        rect(cx-cw/2,cy-ch/2,cw,ch,sf::Color(12,9,26,245));
        // Top bar
        rect(cx-cw/2,cy-ch/2,cw,4,bigCol);

        // Tag
        lbl(tag,cx,cy-ch/2+12,12,sf::Color(tagCol.r,tagCol.g,tagCol.b,200),true);
        // Big text
        lblS(bigText,cx,cy-ch/2+36,34,bigCol,true);
        // Line 1
        lbl(line1,cx,cy-ch/2+90,15,sf::Color(210,205,230),true);
        // Line 2
        lbl(line2,cx,cy-ch/2+116,14,D().accent,true);

        // Divider
        rect(cx-cw/2+30,cy-ch/2+148,cw-60,1,sf::Color(60,55,90));

        // Pulsing hint
        float pa=0.5f+0.5f*sinf(t*3.5f);
        lbl(hint,cx,cy+ch/2-45,13,sf::Color(190,180,225,(uint8_t)(210*pa)),true);
    }

    // ════════════════════════════════════════
    //  RENDER PLAY
    // ════════════════════════════════════════
    void renderPlay(){
        float shX=(shakeAmt>0.4f)?((float)((int)(rng()%100)-50)/50.f*shakeAmt):0;
        float shY=(shakeAmt>0.4f)?((float)((int)(rng()%100)-50)/50.f*shakeAmt):0;

        win.clear(sf::Color(8,6,20));

        sf::View v=win.getDefaultView();
        v.move(sf::Vector2f(shX,shY)); win.setView(v);

        // Dark background for maze area
        rect(0,0,(float)WIN_W,(float)MAZE_H,sf::Color(18,14,30));

        maze.draw(win, D().floorCol, D().wallCol);

        int CS=maze.CS, ox=maze.offX(), oy2=maze.offY();
        float t=sceneClock.getElapsedTime().asSeconds();

        // Start tile
        rect((float)(ox+3),(float)(oy2+3),(float)(CS-6),(float)(CS-6),sf::Color(40,190,90,85));
        lbl("S",(float)(ox+CS/2),(float)(oy2+CS/2-8),12,sf::Color(30,170,70),true);

        // Goal tile (pulsing)
        uint8_t pu=(uint8_t)(140+115*sinf(t*5.f));
        rect((float)(ox+gx*CS+3),(float)(oy2+gy*CS+3),(float)(CS-6),(float)(CS-6),
             sf::Color(D().accent.r,D().accent.g,D().accent.b,pu));
        lbl("G",(float)(ox+gx*CS+CS/2),(float)(oy2+gy*CS+CS/2-8),12,sf::Color::White,true);

        // BFS hint trail
        for(int i=0;i<(int)hintPath.size();i++){
            float fade=1.f-(float)i/max(1,(int)hintPath.size());
            sf::CircleShape d((float)CS/5.5f);
            d.setFillColor(sf::Color(50,220,100,(uint8_t)(200*fade)));
            d.setPosition(sf::Vector2f((float)(ox+hintPath[i].first*CS+CS/2)-(float)CS/5.5f,
                                       (float)(oy2+hintPath[i].second*CS+CS/2)-(float)CS/5.5f));
            win.draw(d);
        }

        // Auto trail
        if(autoOn) for(int i=autoIdx;i<(int)autoPath.size();i++){
            sf::CircleShape d((float)CS/7.f);
            d.setFillColor(sf::Color(80,160,255,110));
            d.setPosition(sf::Vector2f((float)(ox+autoPath[i].first*CS+CS/2)-(float)CS/7.f,
                                       (float)(oy2+autoPath[i].second*CS+CS/2)-(float)CS/7.f));
            win.draw(d);
        }

        // Player
        float rad=(float)CS/2.f-3;
        sf::CircleShape glow(rad+9); glow.setFillColor(sf::Color(D().accent.r,D().accent.g,D().accent.b,25));
        glow.setPosition(sf::Vector2f(drawX-rad-9,drawY-rad-9)); win.draw(glow);
        sf::CircleShape body(rad); body.setFillColor(D().accent);
        body.setPosition(sf::Vector2f(drawX-rad,drawY-rad)); win.draw(body);
        sf::CircleShape shine(rad/3.f); shine.setFillColor(sf::Color(255,255,255,145));
        shine.setPosition(sf::Vector2f(drawX-rad+rad/2,drawY-rad+rad/4)); win.draw(shine);

        drawParticles();
        win.setView(win.getDefaultView());
        drawPanel();

        // ── Overlays ──
        if(state==GS::LEVEL_WIN){
            bool last=(currentLevel==MAX_LEVELS);
            string sc="Level score: "+to_string(levelScore)+"  |  Time bonus: "+to_string((int)(timeLeft*5));
            bigOverlay(sf::Color(0,25,0,165),
                last?"FINAL LEVEL!":"LEVEL "+to_string(currentLevel)+" COMPLETE",
                D().accent,
                "WELL DONE!",D().accent,
                last?"You cleared the last level! Press any key...":"Press any key for the next level",
                sc,"Press any key to continue");
        }
        if(state==GS::GAME_OVER){
            bigOverlay(sf::Color(55,0,0,175),
                "LEVEL "+to_string(currentLevel)+" — FAILED",sf::Color(255,80,80),
                "SORRY!  TIME'S UP",sf::Color(255,70,80),
                "You ran out of time. Better luck next time!",
                "Difficulty: "+D().name,
                "[R] Play Again   |   [ESC] Main Menu");
        }
        if(state==GS::GAME_WIN){
            string sc="TOTAL SCORE: "+to_string(totalScore+levelScore);
            bigOverlay(sf::Color(0,35,10,175),
                "ALL "+to_string(MAX_LEVELS)+" LEVELS CLEARED!",sf::Color(50,230,120),
                "HURRAY!  YOU WIN!",sf::Color(50,230,120),
                "You are a maze master! Incredible run!",
                sc,"[R] Play Again   |   [ESC] Main Menu");
        }

        win.display();
    }

    // ════════════════════════════════════════
    //  MAIN LOOP
    // ════════════════════════════════════════
    void run(){
        while(win.isOpen()){
            float dt=min(loopClock.restart().asSeconds(),0.05f);
            if(state==GS::MENU){
                menuInput(); tickParticles(dt); menuDraw();
            } else {
                playInput(); update(dt); renderPlay();
            }
        }
    }
};

int main(){
    Game g; g.run(); return 0;
}