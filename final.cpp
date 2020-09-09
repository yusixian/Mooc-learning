#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <vector>
using namespace std;
const int warrior_num = 5;//武士种类数量
const int maxwarrior = 1000;//每个基地最多有多少武士 题目未说
const int rule0[warrior_num] = { 2,3,4,1,0 };//红制造顺序
const int rule1[warrior_num] = { 3,0,1,2,4 };//蓝制造顺序
int inilife[warrior_num] = { 0 };//五种武士初始生命值先都设为0
int iniATK[warrior_num] = { 0 };//五种武士初始攻击力先都设为0  后期输入
int N, K, T;//共有N个城市 lion每前进一步，忠诚度就降低K。到时间T为止(包括T) 的所有事件
const string allcolor[2] = { "red", "blue" };
const string allkind[warrior_num] = { "dragon","ninja", "iceman", "lion", "wolf" };//种类常量数组 0 dragon 1 ninja 2 iceman 3 lion 4 wolf
const string weapon[3] = { "sword","bomb","arrow" };
//3种武器 
struct Weapon {
    Weapon() :num(0), usenum(0) {}
    Weapon(int k) {
        num = k;
        if (k == 0) {//sword可以用无限次
            usenum = -1;
        }
        else if (k == 1) {//bomb只能用1次
            usenum = 1;
        }
        else usenum = 2;//arrow只能用两次
    }
    int num;//种类编号
    int usenum;//-1表示可用无限次
    bool operator<(const Weapon& w1) const {
        if (num != w1.num) {
            return num < w1.num;
        } 
        return usenum < w1.usenum;
    }
    bool operator==(const Weapon& w1) const {
        return num == w1.num && usenum == w1.usenum;
    }
};
class Headquarter;
class Warrior {//武士类
public:
    friend class Headquarter;
    Warrior() :base_life(0) {}
    Warrior(int col, int idd, int k);//根据种族信息k初始化生命值与攻击力
    // virtual void Attack(int wp, Warrior& pW);攻击多少由什么武器及自己攻击力决定
    virtual void Hurted(int power) {
        base_life -= power;
        if (base_life <= 0) {
            base_life = 0;
            Dead();
        }
    }
    virtual void Reduce_loyal() {}
    virtual bool run() {
        return false;
    }
    virtual void loof(int h, int m, Warrior& pW) {
        if (!pW.weapons.empty() && weapons.size() < 10) {//若对方武器不为空且Wolf武器不多于10，则Wolf进行抢夺
            sort(pW.weapons.begin(), pW.weapons.end());
            vector<Weapon>::iterator it = pW.weapons.begin();
            int nowid = (*it).num;//当前武器编号
            int cnt = 0;
            for (; it != pW.weapons.end(); ) {
                if ((*it).num == nowid && weapons.size() < 10) {//抢
                    weapons.push_back(*it);
                    cnt++;
                    it = pW.weapons.erase(it);
                }
                else it++;
            }
            printf("%03d:%02d %s wolf %d took %d %s from %s %s %d in city %d\n", h, m, allcolor[color].c_str(), id, cnt,
                weapon[nowid].c_str(), allcolor[pW.color].c_str(), allkind[pW.kind].c_str(), pW.id, city);
        }
    }
    //   virtual void Print();//输出该武士信息
    virtual void GetWeapon(Warrior& pW) {//抢夺对方的武器
    }
    int Getkind() { return kind; }
    void Dead() { base_life = 0; }//死了
    void report() {
        int cnt[3] = { 0 };
        vector<Weapon>::iterator it;
        for (it = weapons.begin(); it != weapons.end(); it++) {
            cnt[(*it).num]++;
        }
        printf("%s %s %d has %d sword %d bomb %d arrow and %d elements\n", allcolor[color].c_str(), allkind[kind].c_str(), id, cnt[0], cnt[1], cnt[2], base_life);
    }
    bool isDead() { return base_life == 0; }//若死了返回true
    int color;//属于哪个基地
    int city;//现在所在城市编号 西边红为0,东边蓝为N+1
    int id;//编号
    int base_life;//生命值 为0时表示死亡
    int base_ATK;//攻击力
    int kind;//该武士种类0 dragon 1 ninja 2 iceman 3 lion 4 wolf
    vector<Weapon> weapons;
};
struct City {
    City() {
        r_w = NULL;
        b_w = NULL;
    }
    Warrior* r_w;
    Warrior* b_w;

} allcity[22];
class Headquarter {//基类 司令部
public:
    friend void LionRun(Headquarter& r, Headquarter& b);//Lion逃跑
    friend void WarriorMove(Headquarter& r, Headquarter& b);//全军出击【不是
    friend void WolfLoot(Headquarter& r, Headquarter& b);//Wolf抢夺
    friend void Fighting(Headquarter& r, Headquarter& b);//发生战斗
    friend void Warrior_Report(Headquarter& r, Headquarter& b);//武士报告
    friend void Head_Report(Headquarter& r, Headquarter& b);//基地汇报
    Headquarter() { }
    Headquarter(int m, int color1) :lifeyuan(m), condition(false), canproduce(true), index(0), totalnum(0), color(color1), hour(0), minute(0) {
        for (int i = 0; i < warrior_num; i++) warrior[i] = 0;
    }
    ~Headquarter() {
        for (int i = 0; i < totalnum; i++)
            delete pWar[i];
    }
    //初始化一个基地需要他的初始生命和颜色红/蓝
    int getlifeyuan() { return lifeyuan; }
    int getindex() { return index; }
    int GetTime() { return hour * 60 + minute; }//分钟为单位
    void setcolor(int m) { color = m; };
    void plushour() { hour++; }
    void plusminute(int min) {
        minute += min;
        if (minute >= 60) minute %= 60;
    }
    void plusindex();//改变循环序列的下标使其+1，并保证不越界
    bool born(int k);//生产一个武士 并输出该事件
    void Report() {//司令部报告它拥有的生命元数量
        printf("%03d:%02d %d elements in %s headquarter\n", hour, minute, lifeyuan, allcolor[color].c_str());
    }
    void stop();//改变状态 并输出该事件
    void Betaken() {
        condition = true;
        printf("%03d:%02d %s headquarter was taken\n", hour, minute, allcolor[color].c_str());
    }
    bool isproduce() { return canproduce; }//获取当前状态 能/不能继续产生武士
    bool getcondition() { return condition; }
private:
    int hour;//当前时间:几点
    int minute;////当前时间:几分钟
    int lifeyuan;//生命元
    bool canproduce;//当前状态 能/不能继续产生武士
    bool condition; //基地是否被占领
    int index;//当前制造序列的下标
    int totalnum;//当前武士总数 (死的也算)
    int color;//阵营 红/蓝 = 0/1
    int warrior[warrior_num];//每种武士的数量0 dragon 1 ninja 2 iceman 3 lion 4 wolf
    Warrior* pWar[maxwarrior];
};
Warrior::Warrior(int col, int idd, int k) {
    id = idd;
    kind = k;
    color = col;
    base_life = inilife[k];
    base_ATK = iniATK[k];
    if (col == 0) {
        city = 0;
        allcity[0].r_w = this;
    }
    else {
        city = N + 1;
        allcity[N + 1].b_w = this;
    }
}
class Dragon :public Warrior {
public:
    Dragon(int col, int idd, int k, int wea) :Warrior(col, idd, k) {
        Weapon w_1(wea);
        Warrior::weapons.push_back(w_1);
    }
    virtual void Print() {//输出该武士信息
        // printf("It has a %s,and it's morale is %.2f\n", weapon[w_1].c_str(), morale);
    }
    Dragon() { }
};
class Ninja :public Warrior {
public:
    Ninja(int col, int idd, int k, int w1, int w2) :Warrior(col, idd, k) {
        Weapon w_1(w1);
        Weapon w_2(w2);
        Warrior::weapons.push_back(w_1);
        Warrior::weapons.push_back(w_2);
    }
    virtual void Print() {//输出该武士信息
        // printf("It has a %s and a %s\n", weapon[w_1].c_str(), weapon[w_2].c_str());
    }
    Ninja() { }
};
class Iceman :public Warrior {
public:
    Iceman(int col, int idd, int k, int wea) :Warrior(col, idd, k) {
        Weapon w_1(wea);
        Warrior::weapons.push_back(w_1);
    }
    Iceman() { }
    //virtual void Print() {//输出该武士信息
    //    // printf("It has a %s\n", weapon[w_1].c_str());
    //}
};
class Lion :public Warrior {
public:
    Lion(int col, int idd, int k, int loy, int wea) :Warrior(col, idd, k), loyalty(loy) {
        Weapon w_1(wea);
        Warrior::weapons.push_back(w_1);
    }
    //virtual void Print() {//输出该武士信息
    //    printf("It's loyalty is %d\n", loyalty);
    //}
    Lion() { }
    virtual void Reduce_loyal() {
        loyalty -= K;
    }
    virtual bool run() { //是否要逃跑，是则返回true否则返回否
        if (loyalty <= 0) {
            Warrior::Dead();
            return true;
        }
        else return false;
    }
private:
    int loyalty;
};
class Wolf :public Warrior {
public:
    Wolf() { }
    Wolf(int col, int idd, int k) :Warrior(col, idd, k) { }
    //virtual void Print() {//输出该武士信息
    //}
};
void Headquarter::plusindex() {
    index++;
    if (index == warrior_num) index = 0;//若越界置为0
}
bool Headquarter::born(int k) {
    //想生产一个武士应当知道他的种类编号k 成功则true 减少基地中生命元 失败则返回false进行下一次尝试
    if (lifeyuan >= inilife[k]) {//生命元足够，可以初始化
        lifeyuan -= inilife[k];
        warrior[k]++;//该种类+1 
        totalnum++;//武士总量++
        int w1 = totalnum % 3;
        int w2 = (totalnum + 1) % 3;
        printf("%03d:%02d %s %s %d born\n", hour, minute, allcolor[color].c_str(), allkind[k].c_str(), totalnum);
        // printf("%d %s in %s headquarter\n", warrior[k], allkind[k].c_str(), allcolor[color].c_str());
        //eg:000 red iceman 1 born
        Dragon d(color, totalnum, k, w1);
        Ninja n(color, totalnum, k, w1, w2);
        Iceman i(color, totalnum, k, w1);
        Lion l(color, totalnum, k, lifeyuan, w1);//注意 这里多了个w1~
        Wolf w(color, totalnum, k);
        if (k == 0) {
            pWar[totalnum - 1] = new Dragon(color, totalnum, k, w1);
            *pWar[totalnum - 1] = d;
        }
        else if (k == 1) {
            pWar[totalnum - 1] = new Ninja(color, totalnum, k, w1, w2);
            *pWar[totalnum - 1] = n;
        }
        else if (k == 2) {
            pWar[totalnum - 1] = new Iceman(color, totalnum, k, w1);
            *pWar[totalnum - 1] = i;
        }
        else if (k == 3) {
            printf("Its loyalty is %d\n", lifeyuan);
            pWar[totalnum - 1] = new Lion(color, totalnum, k, lifeyuan, w1);
            *pWar[totalnum - 1] = l;
        }
        else if (k == 4) {
            pWar[totalnum - 1] = new Wolf(color, totalnum, k);
            *pWar[totalnum - 1] = w;
        }
        return true;
    }
    else
        return false;
}
void Headquarter::stop() {
    canproduce = false;
    // printf("%03d:%02d %s headquarter stops making warriors\n", hour, minute, allcolor[color].c_str());
}
void plusmin(Headquarter& r, Headquarter& b, int min = 5) {
    r.plusminute(min);
    b.plusminute(min);
}
void plush(Headquarter& r, Headquarter& b) {
    r.plushour();
    b.plushour();
}
void produce(Headquarter& r, Headquarter& b) {//生产武士
    int rindex = r.getindex();
    int bindex = b.getindex();//如果司令部中的生命元不足以制造某本该造的武士，那就从此停止制造武士。
    if (r.isproduce()) {//00 事件1 红方武士诞生
        if (r.born(rule0[rindex])) {
            r.plusindex();
        }
        else r.stop();
    }
    if (b.isproduce()) {//00 事件1 蓝方武士诞生
        if (b.born(rule1[bindex])) {
            b.plusindex();
        }
        else b.stop();
    }
}
void LionRun(Headquarter& r, Headquarter& b) {//从西向东
    for (int i = 0; i <= N + 1; i++) {
        Warrior* r_w;//此时在城市i的红武士
        if (allcity[i].r_w) {//当有红武士时
            r_w = allcity[i].r_w;
            if (!r_w->isDead() && r_w->kind == 3) {//未死 且为红Lion 
                if (r_w->run()) printf("%03d:%02d red lion %d ran away\n", r.hour, r.minute, r_w->id);
            }
        }
        Warrior* b_w;//此时在城市i的蓝武士
        if (allcity[i].b_w) {
            b_w = allcity[i].b_w;
            if (!b_w->isDead() && b_w->kind == 3) {//未死 且为蓝Lion 
                if (b_w->run()) printf("%03d:%02d blue lion %d ran away\n", b.hour, b.minute, b_w->id);
            }
        }
    }
}
void WarriorMove(Headquarter& r, Headquarter& b) {
    int h = r.hour;
    int m = r.minute;
    for (int i = 0; i <= N + 1; i++) {//移动(改动city)
        Warrior* r_w;//此时在城市i的红武士
        if (allcity[i].r_w) {//当有红武士时
            r_w = allcity[i].r_w;
            if (!r_w->isDead()) {//未死 则移动到下一个城市 红是向右边走 ++
                r_w->city++;
                if (r_w->kind == 2) {//iceman每前进一步，生命值减少10%(减少的量要去尾取整)
                    int reduce = r_w->base_life * 1 / 10;
                    r_w->Hurted(reduce);
                }
                else if (r_w->kind == 3) {//Lion每前进一步忠诚度就降低K
                    r_w->Reduce_loyal();
                }
            }
        }
        Warrior* b_w;//此时在城市i的蓝武士
        if (allcity[i].b_w) {//当有蓝武士时
            b_w = allcity[i].b_w;
            if (!b_w->isDead()) {//未死 则移动到下一个城市 蓝是向左边走 --
                b_w->city--;
                if (b_w->kind == 2) {//iceman每前进一步，生命值减少10%(减少的量要去尾取整)
                    int reduce = b_w->base_life * 1 / 10;
                    b_w->Hurted(reduce);
                }
                else if (b_w->kind == 3) {//Lion每前进一步忠诚度就降低K
                    b_w->Reduce_loyal();
                }
            }
        }
    }
    for (int i = 0; i <= N + 1; i++) {//清空所有城市指针，重新赋值
        allcity[i].b_w = NULL;
        allcity[i].r_w = NULL;
    }
    for (int i = 0; i < r.totalnum; i++) {//重新赋值
        allcity[r.pWar[i]->city].r_w = r.pWar[i];
    }
    for (int i = 0; i < b.totalnum; i++) {
        allcity[b.pWar[i]->city].b_w = b.pWar[i];
    }
    for (int i = 0; i <= N + 1; i++) {//行军后输出情况
        Warrior* r_w;//此时在城市i的红武士
        if (allcity[i].r_w) {//当有红武士时
            r_w = allcity[i].r_w;
            if (!r_w->isDead() && i == N + 1) {//未死 且到蓝方司令部了
                printf("%03d:%02d red %s %d reached blue headquarter with %d elements and force %d\n", h, m,
                    allkind[r_w->kind].c_str(), r_w->id, r_w->base_life, r_w->base_ATK);
                b.Betaken();//蓝司令部被占领 输出该事件
            }
            else if (!r_w->isDead()) {//未死 则它移动到该城市了 输出事件
                printf("%03d:%02d red %s %d marched to city %d with %d elements and force %d\n", h, m,
                    allkind[r_w->kind].c_str(), r_w->id, r_w->city, r_w->base_life, r_w->base_ATK);
            }
        }
        Warrior* b_w;//此时在城市i的蓝武士
        if (allcity[i].b_w) {//当有蓝武士时
            b_w = allcity[i].b_w;
            if (!b_w->isDead() && i == 0) {//未死 且到红方司令部了
                printf("%03d:%02d blue %s %d reached red headquarter with %d elements and force %d\n", h, m,
                    allkind[b_w->kind].c_str(), b_w->id, b_w->base_life, b_w->base_ATK);
                r.Betaken();//红司令部被占领 输出该事件
            }
            else if (!b_w->isDead()) {//未死 则它移动到该城市了 输出事件
                printf("%03d:%02d blue %s %d marched to city %d with %d elements and force %d\n", h, m,
                    allkind[b_w->kind].c_str(), b_w->id, b_w->city, b_w->base_life, b_w->base_ATK);
            }
        }
    }
}
void WolfLoot(Headquarter& r, Headquarter& b) {
    int h = r.hour;
    int m = r.minute;
    for (int i = 1; i <= N; i++) {
        Warrior* r_w;//此时在城市i的红武士
        Warrior* b_w;//此时在城市i的蓝武士
        if (allcity[i].r_w && allcity[i].b_w) {//当红武士与蓝武士同时存在时
            r_w = allcity[i].r_w;
            b_w = allcity[i].b_w;
            if (!r_w->isDead() && !b_w->isDead()) {//当红武士与蓝武士同时活着时
                if (r_w->kind == 4 && b_w->kind != 4) {//红武士为wolf 抢夺蓝武士
                    r_w->loof(h, m, (*b_w));
                }
                else if (b_w->kind == 4 && r_w->kind != 4) {//蓝武士为wolf 抢夺红武士
                    b_w->loof(h, m, (*r_w));
                }
            }
        }
    }
}
void Fighting(Headquarter& r, Headquarter& b) {
    int h = r.hour;
    int m = r.minute;
    for (int i = 1; i <= N; i++) {
        Warrior* r_w;//此时在城市i的红武士
        Warrior* b_w;//此时在城市i的蓝武士
        if (!allcity[i].r_w || !allcity[i].b_w) continue;
        r_w = allcity[i].r_w;
        b_w = allcity[i].b_w;
        if (r_w->isDead() || b_w->isDead()) continue;
        //开始battle
        if (r_w->weapons.empty() && b_w->weapons.empty()) {//若战斗开始前双方都没有武器 算活着的平局
            printf("%03d:%02d both red %s %d and blue %s %d were alive in city %d\n", h, m, allkind[r_w->kind].c_str(),
                r_w->id, allkind[b_w->kind].c_str(), b_w->id, i);
            //dragon存活会欢呼
            if (r_w->kind == 0) printf("%03d:%02d red dragon %d yelled in city %d\n", h, m, r_w->id, i);
            if (b_w->kind == 0) printf("%03d:%02d blue dragon %d yelled in city %d\n", h, m, b_w->id, i);
        }
        else {//
            if (!r_w->weapons.empty()) sort(r_w->weapons.begin(), r_w->weapons.end());
            if (!b_w->weapons.empty()) sort(b_w->weapons.begin(), b_w->weapons.end());//排序 唯一一次
            Warrior* winer = r_w;//胜利者
            Warrior* loser = b_w;//失败者
            int flag;//0 为分出胜负了，1为平局且都活着 2为平局都死了
            vector<Weapon>::iterator rt = r_w->weapons.begin();
            vector<Weapon>::iterator bt = b_w->weapons.begin();
            int rlastlife = 0;
            int blastlife = 0;
            vector<Weapon> rlast;
            vector<Weapon> blast;//记录上一次状况
            while (1) {
                if (r_w->isDead() && b_w->isDead()) {//两个都死了
                    flag = 2;
                    break;
                }
                else if (r_w->isDead()) {//红死了
                    flag = 0;
                    winer = b_w;
                    loser = r_w;
                    break;
                }
                else if (b_w->isDead()) {//蓝死了
                    flag = 0;
                    winer = r_w;
                    loser = b_w;
                    break;
                }
                if (r_w->weapons.empty() && b_w->weapons.empty()) {//武器用完了且都活着
                    flag = 1;
                    break;
                }
                if (i % 2 == 1) {//奇数城市 红先攻击 若红没有武器则到蓝 
                    if (!r_w->weapons.empty()) {//红有武器
                        int base_ATK = r_w->base_ATK;
                        if (rt == r_w->weapons.end()) rt = r_w->weapons.begin();//使用完一轮了再来一轮
                        switch ((*rt).num) {
                        case 0: {//sword
                            int power = base_ATK * 2 / 10;
                            b_w->Hurted(power);
                            rt++;//
                            break;
                        }
                        case 1: {//bomb 一旦使用就没了
                            int power = base_ATK * 4 / 10;
                            b_w->Hurted(power);
                            if (r_w->kind != 1) {//ninja 使用bomb不会让自己受伤,其他都会
                                int hit = power * 5 / 10;
                                r_w->Hurted(hit);
                            }
                            rt = r_w->weapons.erase(rt);//清除bomb
                            break;
                        }
                        case 2: {//arrow
                            int power = base_ATK * 3 / 10;
                            b_w->Hurted(power);
                            (*rt).usenum--;
                            if ((*rt).usenum == 0) rt = r_w->weapons.erase(rt);//清除arrow
                            else rt++;
                            break;
                        }
                        default: break;
                        }
                    }
                    //判断胜负
                    if (r_w->isDead() && b_w->isDead()) {//两个都死了
                        flag = 2;
                        break;
                    }
                    else if (r_w->isDead()) {//红死了
                        flag = 0;
                        winer = b_w;
                        loser = r_w;
                        break;
                    }
                    else if (b_w->isDead()) {//蓝死了
                        flag = 0;
                        winer = r_w;
                        loser = b_w;
                        break;
                    }
                    if (!b_w->weapons.empty()) {//蓝有武器
                        int base_ATK = b_w->base_ATK;
                        if (bt == b_w->weapons.end()) bt = b_w->weapons.begin();//使用完一轮了再来一轮
                        switch ((*bt).num) {
                        case 0: {//sword
                            int power = base_ATK * 2 / 10;
                            r_w->Hurted(power);
                            bt++;//
                            break;
                        }
                        case 1: {//bomb 一旦使用就没了
                            int power = base_ATK * 4 / 10;
                            r_w->Hurted(power);
                            if (b_w->kind != 1) {//ninja 使用bomb不会让自己受伤,其他都会
                                int hit = power * 5 / 10;
                                b_w->Hurted(hit);
                            }
                            bt = b_w->weapons.erase(bt);//清除bomb
                            break;
                        }
                        case 2: {//arrow
                            int power = base_ATK * 3 / 10;
                            r_w->Hurted(power);
                            (*bt).usenum--;
                            if ((*bt).usenum == 0) bt = b_w->weapons.erase(bt);//清除arrow
                            else bt++;
                            break;
                        }
                        default: break;
                        }
                    }
                }
                else {//偶数城市 蓝先攻击 若蓝没有武器则到红 
                    if (!b_w->weapons.empty()) {//蓝有武器
                        int base_ATK = b_w->base_ATK;
                        if (bt == b_w->weapons.end()) bt = b_w->weapons.begin();//使用完一轮了再来一轮
                        switch ((*bt).num) {
                        case 0: {//sword
                            int power = base_ATK * 2 / 10;
                            r_w->Hurted(power);
                            bt++;//
                            break;
                        }
                        case 1: {//bomb 一旦使用就没了
                            int power = base_ATK * 4 / 10;
                            r_w->Hurted(power);
                            if (b_w->kind != 1) {//ninja 使用bomb不会让自己受伤,其他都会
                                int hit = power * 5 / 10;
                                b_w->Hurted(hit);
                            }
                            bt = b_w->weapons.erase(bt);//清除bomb
                            break;
                        }
                        case 2: {//arrow
                            int power = base_ATK * 3 / 10;
                            r_w->Hurted(power);
                            (*bt).usenum--;
                            if ((*bt).usenum == 0) bt = b_w->weapons.erase(bt);//清除arrow
                            else bt++;
                            break;
                        }
                        default: break;
                        }
                    }
                    //判断胜负
                    if (r_w->isDead() && b_w->isDead()) {//两个都死了
                        flag = 2;
                        break;
                    }
                    else if (r_w->isDead()) {//红死了
                        flag = 0;
                        winer = b_w;
                        loser = r_w;
                        break;
                    }
                    else if (b_w->isDead()) {//蓝死了
                        flag = 0;
                        winer = r_w;
                        loser = b_w;
                        break;
                    }
                    if (!r_w->weapons.empty()) {//红有武器
                        int base_ATK = r_w->base_ATK;
                        if (rt == r_w->weapons.end()) rt = r_w->weapons.begin();//使用完一轮了再来一轮
                        switch ((*rt).num) {
                        case 0: {//sword
                            int power = base_ATK * 2 / 10;
                            b_w->Hurted(power);
                            rt++;//
                            break;
                        }
                        case 1: {//bomb 一旦使用就没了
                            int power = base_ATK * 4 / 10;
                            b_w->Hurted(power);
                            if (r_w->kind != 1) {//ninja 使用bomb不会让自己受伤,其他都会
                                int hit = power * 5 / 10;
                                r_w->Hurted(hit);
                            }
                            rt = r_w->weapons.erase(rt);//清除bomb
                            break;
                        }
                        case 2: {//arrow
                            int power = base_ATK * 3 / 10;
                            b_w->Hurted(power);
                            (*rt).usenum--;
                            if ((*rt).usenum == 0) rt = r_w->weapons.erase(rt);//清除arrow
                            else rt++;
                            break;
                        }
                        default: break;
                        }
                    }
                }
                if (r_w->weapons == rlast && r_w->base_life == rlastlife &&
                    b_w->weapons == blast && b_w->base_life == blastlife) {
                    //若双方生命值和武器状态都不再变化，则平局
                    flag = 1;
                    break;
                }
                rlast = r_w->weapons;
                blast = b_w->weapons;
                rlastlife = r_w->base_life;
                blastlife = b_w->base_life;
            }
            //判断并执行对应操作
            if (flag == 0) {//胜方缴获武器
                if (!loser->weapons.empty()) {
                    sort(loser->weapons.begin(), loser->weapons.end());
                    while (winer->weapons.size() < 10 && !loser->weapons.empty()) {
                        vector<Weapon>::iterator it = loser->weapons.begin();
                        winer->weapons.push_back(*it);
                        loser->weapons.erase(it);
                    }
                }
                printf("%03d:%02d %s %s %d killed %s %s %d in city %d remaining %d elements\n", h, m, allcolor[winer->color].c_str(),
                    allkind[winer->kind].c_str(), winer->id, allcolor[loser->color].c_str(), allkind[loser->kind].c_str(), loser->id, i, winer->base_life);
                if (winer->kind == 0)
                    printf("%03d:%02d %s dragon %d yelled in city %d\n", h, m, allcolor[winer->color].c_str(), winer->id, i);
            }
            else if (flag == 1) {//平局且都活着
                printf("%03d:%02d both red %s %d and blue %s %d were alive in city %d\n", h, m, allkind[r_w->kind].c_str(),
                    r_w->id, allkind[b_w->kind].c_str(), b_w->id, i);
                //dragon存活会欢呼
                if (r_w->kind == 0) printf("%03d:%02d red dragon %d yelled in city %d\n", h, m, r_w->id, i);
                if (b_w->kind == 0) printf("%03d:%02d blue dragon %d yelled in city %d\n", h, m, b_w->id, i);
            }
            else {//平局都死了
                printf("%03d:%02d both red %s %d and blue %s %d died in city %d\n", h, m, allkind[r_w->kind].c_str(),
                    r_w->id, allkind[b_w->kind].c_str(), b_w->id, i);
            }
        }
    }
}
void Head_Report(Headquarter& r, Headquarter& b) {
    r.Report();
    b.Report();
}
void Warrior_Report(Headquarter& r, Headquarter& b) {
    int h = r.hour;
    int m = r.minute;
    for (int i = 0; i <= N + 1; i++) {
        Warrior* r_w;//此时在城市i的红武士
        Warrior* b_w;//此时在城市i的蓝武士
        if (allcity[i].r_w) {
            r_w = allcity[i].r_w;
            if (!r_w->isDead()) {//没死就报告
                printf("%03d:%02d ", h, m);
                r_w->report();
            }
        }
        if (allcity[i].b_w) {
            b_w = allcity[i].b_w;
            if (!b_w->isDead()) {//没死就报告
                printf("%03d:%02d ", h, m);
                b_w->report();
            }
        }
    }
}
int main() {
    int t;
    cin >> t;
    for (int i = 1; i <= t; i++) {
        int M;//一开始每个司令部有M个生命元
        cin >> M >> N >> K >> T;
        for (int j = 0; j < warrior_num; j++)
            cin >> inilife[j];//设置五种武士的初始血量
        for (int j = 0; j < warrior_num; j++)
            cin >> iniATK[j];//设置五种武士的初始攻击力
        cout << "Case " << i << ":" << endl;
        Headquarter red(M, 0);
        Headquarter blue(M, 1);
        while (1) {
            produce(red, blue);//00 产生武士
            plusmin(red, blue);
            if (red.GetTime() > T || blue.GetTime() > T) break;//检查时间是否需要继续

            LionRun(red, blue);//05
            plusmin(red, blue);
            if (red.GetTime() > T || blue.GetTime() > T) break;

            WarriorMove(red, blue);//10 
            if (red.getcondition() || blue.getcondition()) break;//检查是否被攻占
            plusmin(red, blue, 25);
            if (red.GetTime() > T || blue.GetTime() > T) break;

            WolfLoot(red, blue);//35
            plusmin(red, blue);
            if (red.GetTime() > T || blue.GetTime() > T) break;

            Fighting(red, blue);//40
            plusmin(red, blue, 10);
            if (red.GetTime() > T || blue.GetTime() > T) break;

            Head_Report(red, blue);//50
            plusmin(red, blue);
            if (red.GetTime() > T || blue.GetTime() > T) break;

            Warrior_Report(red, blue);//55
            plusmin(red, blue);//60->0
            plush(red, blue);
            if (red.GetTime() > T || blue.GetTime() > T) break;
        }
        for (int i = 0; i <= N + 1; i++) {//清空所有城市指针，重新赋值
            allcity[i].b_w = NULL;
            allcity[i].r_w = NULL;
        }
    }
    return 0;
}