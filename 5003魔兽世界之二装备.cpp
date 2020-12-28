#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace std;
const int warrior_num = 5;//武士种类数量
const int maxwarrior = 1000;//每个基地最多有多少武士 题目未说
const int rule0[warrior_num] = {2,3,4,1,0};//红制造顺序
const int rule1[warrior_num] = {3,0,1,2,4};//蓝制造顺序
int inilife[warrior_num] = {0};//五种武士初始生命值先都设为为0
const string allcolor[2] = {"red", "blue"};
const string allkind[warrior_num] = {"dragon","ninja", "iceman", "lion", "wolf"};//种类常量数组 0 dragon 1 ninja 2 iceman 3 lion 4 wolf
const string weapon[3] = {"sword","bomb","arrow"};
//3种武器
class Headquarter;
class Warrior {//武士类
    public:
        Warrior (int col, int idd, int k) {
            color = col;
            id = idd;
            kind = k;
        }
        Warrior() { }
        virtual void print() { }//输出该武士信息
    private:
        int color;//所属基地
        int id;//编号
        int kind;//该武士种类0 dragon 1 ninja 2 iceman 3 lion 4 wolf
};
class Headquarter {//基类 司令部
    public:
        Headquarter(int m, int color1):lifeyuan(m), condition(true),index(0),totalnum(0),color(color1),time(0){
            for (int i = 0; i < warrior_num; i++) warrior[i] = 0;
        }
        //初始化一个基地需要他的初始生命和颜色红/蓝
        int getlifeyuan() { return lifeyuan; }
        int getindex() {    return index; }
        void setcolor(int m) { color = m;};
        void plustime(){ time++;}
        void plusindex();//改变循环序列的下标使其+1，并保证不越界
        void stop();//改变状态 并输出该事件
        bool born(int k);//生产一个武士 并输出该事件
        bool getcondition() {   return condition;}//获取当前状态
    private:
        int lifeyuan;//生命元
        bool condition;//当前状态 能/不能继续产生武士
        int index;//当前制造序列的下标
        int time;//当前时间点
        int totalnum;
        int color;//阵营 红/蓝 = 0/1
        int warrior[warrior_num];//每种武士的数量0 dragon 1 ninja 2 iceman 3 lion 4 wolf
        Warrior pWar[maxwarrior];
};
class Dragon:public Warrior {
    public:
        Dragon(int col, int idd, int k, int wea,double mor):Warrior(col, idd, k),w_1(wea),morale(mor){ }
        virtual void print() {//输出该武士信息
            printf("It has a %s,and it's morale is %.2f\n", weapon[w_1].c_str(), morale);
        }
    private:
        int w_1;//武器
        double morale;//士气
};
class Ninja:public Warrior {
    public:
        Ninja(int col, int idd, int k, int w1, int w2):Warrior(col, idd, k), w_1(w1), w_2(w2) { }
        virtual void print(){//输出该武士信息
            printf("It has a %s and a %s\n", weapon[w_1].c_str(), weapon[w_2].c_str());
        }
    private:
        int w_1;//武器1
        int w_2;//武器2
};
class Iceman:public Warrior {
    public:
        Iceman(int col, int idd, int k, int wea):Warrior(col, idd, k), w_1(wea) { }
        virtual void print(){//输出该武士信息
            printf("It has a %s\n", weapon[w_1].c_str());
        }
        private:
            int w_1;//武器
};
class Lion:public Warrior {
    public:
        Lion(int col, int idd, int k, int loy):Warrior(col, idd, k), loyalty(loy) { }
        virtual void print(){//输出该武士信息
            printf("It's loyalty is %d\n", loyalty);
        }
    private:
        int loyalty;
};
class Wolf:public Warrior {
    public:
        Wolf(int col, int idd, int k):Warrior(col, idd, k) { }
        virtual void print(){//输出该武士信息
        }
};

void Headquarter::plusindex() {
    index++;
    if (index == warrior_num) index = 0;//若越界置为0
}
bool Headquarter::born(int k) {
    //想生产一个武士应当知道他的种类编号k 成功则true 减少基地中生命元 失败则返回false进行下一次尝试
    if (lifeyuan >=inilife[k]){//生命元足够，可以初始化
        lifeyuan -=inilife[k];
        warrior[k]++;//该种类+1 
        totalnum++;//武士总量++
        int w1 = totalnum % 3;
        int w2 = (totalnum+1) % 3;
        double moral = 1.0*lifeyuan/inilife[k];
        printf("%03d %s %s %d born with strength %d,", time, allcolor[color].c_str(), allkind[k].c_str(), totalnum, inilife[k]);
        printf("%d %s in %s headquarter\n", warrior[k], allkind[k].c_str(), allcolor[color].c_str());
        //eg:000 red iceman 1 born with strength 5,1 iceman in red headquarter
        Dragon d(color, totalnum, k, w1, moral);
        Ninja n(color, totalnum, k, w1, w2);
        Iceman i(color, totalnum, k, w1);
        Lion l(color, totalnum, k, lifeyuan);
        Wolf w(color, totalnum, k);
        switch (k)
        {
        case 0:  
            d.print();
            break;
        case 1: 
            n.print();
            break;
        case 2: 
            i.print();
            break;
        case 3:
            l.print();
            break;
        case 4: 
            w.print();
            break;
        default:
            break;
        }
        return true;
    } else return false;
}
void Headquarter::stop() {
    condition = false;
    printf("%03d %s headquarter stops making warriors\n", time, allcolor[color].c_str());
}
int main() {
    int T;
    cin >> T;
    for (int i = 1; i <= T; i++) {
        int M;//一开始每个司令部有M个生命元
        cin >> M;
        for (int j = 0; j < warrior_num; j++)
            cin >> inilife[j];//设置五种武士的初始血量
        cout << "Case:" << i << endl;
        Headquarter red(M, 0);
        Headquarter blue(M, 1);
         while (red.getcondition() || blue.getcondition()) {//当两个基地都不能运作的时候循环结束
            int rindex = red.getindex();
            int bindex = blue.getindex();
            if (red.getcondition()) {
                for (int cnt = rindex; cnt < rindex + warrior_num; cnt++) {
                    int num = red.getindex();
                    if (red.born(rule0[num]))  {
                        red.plusindex();
                        break;//成功了的话break;
                    } else if (cnt == rindex + warrior_num - 1 && (!red.born(rule0[num]))){
                        red.stop();
                    } else {//这次未成功 尝试制造下一个
                        red.plusindex();
                    }
                }
            }
            if (blue.getcondition()) {
                for (int cnt = bindex; cnt < bindex + warrior_num; cnt++) {
                    int num = blue.getindex();
                    if (blue.born(rule1[num]))  {
                        blue.plusindex();
                        break;//成功了的话break;
                    } else if (cnt == bindex + warrior_num - 1 && (!blue.born(rule1[num]))){
                        blue.stop();
                    } else {//这次未成功 尝试制造下一个
                        blue.plusindex();
                    }
                }
            }
            red.plustime();
            blue.plustime();
        }
    }
    return 0;
}
//通过码：332d42df1ccbdce871633240e5dce59f97a8b2a833a8a1429bc990db11dd3b80